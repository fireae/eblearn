/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>

#ifdef __OPENCV__
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include "libeblearn.h"
#include "libidx.h"
#include "eblapp.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

#include "similar_patches.h"

#define MAX_SIMILAR_PATCHES 5
#define NFEATURES 50
#define FEATURE_WIDTH 44
#define FEATURE_HEIGHT 44
#define FRAME_JUMP 5
#define FEATURE_ZOOM 2

//namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Feature tracking using Lucas-Kanade optical flow in OpenCV.
  // Thanks to David Stavens for his demo of optical flow with OpenCV
  // ( http://ai.stanford.edu/~dstavens/cs223b/ ).

#ifdef __OPENCV__

  /* This is just an inline that allocates images.  I did this to reduce clutter in the
   * actual computer vision algorithmic code.  Basically it allocates the requested image
   * unless that image is already non-NULL.  It always leaves a non-NULL image as-is even
   * if that image's size, depth, and/or channels are different than the request.
   */
  inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
  {
    if ( *img != NULL )	return;

    *img = cvCreateImage( size, depth, channels );
    if ( *img == NULL )
      {
	fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");
	exit(-1);
      }
  }
#endif

#ifdef __GUI__
  MAIN_QTHREAD(int, argc, char **, argv) {
#else
  int main(int argc, char **argv) {
#endif

#ifndef __OPENCV__
    cerr << "please recompile with opencv." << endl;
#else
    if (argc <= 1) {
      cerr << "Usage example: videoffinder video.avi" << endl;
      return -1;
    }


    cout << "input video: " << argv[1] << endl;
    /* Create an object that decodes the input video stream. */
    CvCapture *input_video = cvCaptureFromFile(argv[1]);
    if (input_video == NULL)
      {
	/* Either the video didn't exist OR it uses a codec OpenCV
	 * doesn't support.
	 */
	fprintf(stderr, "Error: Can't open video.\n");
	return -1;
      }

    /* Read the video's frame size out of the AVI. */
    CvSize frame_size;
    frame_size.height =
      (int) cvGetCaptureProperty( input_video, CV_CAP_PROP_FRAME_HEIGHT );
    frame_size.width =
      (int) cvGetCaptureProperty( input_video, CV_CAP_PROP_FRAME_WIDTH );


    similar_patches sp(NFEATURES, MAX_SIMILAR_PATCHES, FEATURE_HEIGHT, FEATURE_WIDTH, 
		       frame_size.height, frame_size.width);



    /* Determine the number of frames in the AVI. */
    long number_of_frames;
    /* Go to the end of the AVI (ie: the fraction is "1") */
    cvSetCaptureProperty( input_video, CV_CAP_PROP_POS_AVI_RATIO, 1.0);
    /* Now that we're at the end, read the AVI position in frames */
    number_of_frames = 200; //(int) cvGetCaptureProperty( input_video, CV_CAP_PROP_POS_FRAMES );
    /* Return to the beginning */
    cvSetCaptureProperty( input_video, CV_CAP_PROP_POS_FRAMES, 0. );

    /* Create a windows called "Optical Flow" for visualizing the output.
     * Have the window automatically change its size to match the output.
     */

    //  cvNamedWindow("Optical Flow", CV_WINDOW_AUTOSIZE);
#ifdef __GUI__
    cout << "new windows." << endl;
    unsigned int mainwin = new_window("first and second frames");
    //unsigned int featwin = gui.new_window("features");
    gui << "window size: " << FEATURE_HEIGHT << "x" << FEATURE_WIDTH << endl;
    select_window(mainwin);
#endif
    long current_frame = 0;
    while(true)
      {
	static IplImage *frame = NULL, *frame1 = NULL, *frame1_1C = NULL, *frame2_1C = NULL, *eig_image = NULL, *temp_image = NULL, *pyramid1 = NULL, *pyramid2 = NULL;

	//	unsigned int h0 = 0, w0 = 0;

	/* Go to the frame we want.  Important if multiple frames are queried in
	 * the loop which they of course are for optical flow.  Note that the very
	 * first call to this is actually not needed. (Because the correct position
	 * is set outsite the for() loop.)
	 */
	cvSetCaptureProperty( input_video, CV_CAP_PROP_POS_FRAMES, current_frame );

	/* Get the next frame of the video.
	 * IMPORTANT!  cvQueryFrame() always returns a pointer to the _same_
	 * memory location.  So successive calls:
	 * frame1 = cvQueryFrame();
	 * frame2 = cvQueryFrame();
	 * frame3 = cvQueryFrame();
	 * will result in (frame1 == frame2 && frame2 == frame3) being true.
	 * The solution is to make a copy of the cvQueryFrame() output.
	 */
	frame = cvQueryFrame( input_video );
	if (frame == NULL) {
	  cout << "end of video." << endl;
#ifdef __GUI__
	  enable_window_updates();
#endif
	  break ;
	}

// 	ostringstream fname;
// 	fname << "img_" << setfill('0') << setw(5) << current_frame << ".jpg";
// 	cout << "writing " << fname.str() << endl;
// 	cvSaveImage(fname.str().c_str(), frame);

	/* Allocate another image if not already allocated.
	 * Image has ONE channel of color (ie: monochrome) with 8-bit "color" depth.
	 * This is the image format OpenCV algorithms actually operate on (mostly).
	 */
	allocateOnDemand( &frame1_1C, frame_size, IPL_DEPTH_8U, 1 );
	/* Convert whatever the AVI image format is into OpenCV's preferred format.
	 * AND flip the image vertically.  Flip is a shameless hack.  OpenCV reads
	 * in AVIs upside-down by default.  (No comment :-))
	 */
	cvConvertImage(frame, frame1_1C);//, CV_CVTIMG_FLIP);

	/* We'll make a full color backup of this frame so that we can draw on it.
	 * (It's not the best idea to draw on the static memory space of cvQueryFrame().)
	 */
	allocateOnDemand( &frame1, frame_size, IPL_DEPTH_8U, 3 );
	cvConvertImage(frame, frame1, CV_CVTIMG_FLIP);

	idx<ubyte> im1 = ipl_to_idx(frame1_1C);
#ifdef __GUI__
	float zoom = 1;
	select_window(mainwin);
	disable_window_updates();
	clear_window();
	unsigned int imh0 = 0, imw0 = 0;
	draw_matrix(im1, imh0, imw0, zoom, zoom);
	imh0 += im1.dim(0) * zoom + 5;
	gui << "frame #" << current_frame;
#endif

	current_frame += FRAME_JUMP;
	cvSetCaptureProperty( input_video, CV_CAP_PROP_POS_FRAMES, current_frame);
	/* Get the second frame of video.  Same principles as the first. */
	frame = cvQueryFrame( input_video );
	if (frame == NULL) {
	  cout << "end of video." << endl;
#ifdef __GUI__
	  enable_window_updates();
#endif
	  break ;
	}
	allocateOnDemand( &frame2_1C, frame_size, IPL_DEPTH_8U, 1 );
	cvConvertImage(frame, frame2_1C);//, CV_CVTIMG_FLIP);

	idx<ubyte> im2 = ipl_to_idx(frame2_1C);
#ifdef __GUI__
	draw_matrix(im2, imh0, imw0, zoom, zoom);
	gui << at(imh0, imw0) << "frame #" << current_frame << endl;
	cout << endl;
#endif
	/* Shi and Tomasi Feature Tracking! */

	/* Preparation: Allocate the necessary storage. */
	allocateOnDemand( &eig_image, frame_size, IPL_DEPTH_32F, 1 );
	allocateOnDemand( &temp_image, frame_size, IPL_DEPTH_32F, 1 );

	/* Preparation: BEFORE the function call this variable is the array size
	 * (or the maximum number of features to find).  AFTER the function call
	 * this variable is the number of features actually found.
	 */
	int number_of_features;
		
	/* I'm hardcoding this at 400.  But you should make this a #define so that you can
	 * change the number of features you use for an accuracy/speed tradeoff analysis.
	 */
	number_of_features = NFEATURES;

	/* Preparation: This array will contain the features found in frame 1. */
	CvPoint2D32f frame1_features[NFEATURES];

	/* Actually run the Shi and Tomasi algorithm!!
	 * "frame1_1C" is the input image.
	 * "eig_image" and "temp_image" are just workspace for the algorithm.
	 * The first ".01" specifies the minimum quality of the features (based on the eigenvalues).
	 * The second ".01" specifies the minimum Euclidean distance between features.
	 * "NULL" means use the entire input image.  You could point to a part of the image.
	 * WHEN THE ALGORITHM RETURNS:
	 * "frame1_features" will contain the feature points.
	 * "number_of_features" will be set to a value <= 400 indicating the number of feature points found.
	 */
	cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image, frame1_features, &number_of_features, .01, .01, NULL);

	// reset frame1_features
	for(unsigned int i = 0; i < sp.max_current_patches; i++) {
	  if (sp.current_patch_empty(i)) {
	    // add a new feature to track
	  } else { // replace with latest coordinates
	    frame1_features[i].x = sp.current_patches_xy[i].first;
	    frame1_features[i].y = sp.current_patches_xy[i].second;
	  }
	}
	
	/* Pyramidal Lucas Kanade Optical Flow! */

	/* This array will contain the locations of the points from frame 1 in frame 2. */
	CvPoint2D32f frame2_features[NFEATURES];

	/* The i-th element of this array will be non-zero if and only if the i-th feature of
	 * frame 1 was found in frame 2.
	 */
	char optical_flow_found_feature[NFEATURES];

	/* The i-th element of this array is the error in the optical flow for the i-th feature
	 * of frame1 as found in frame 2.  If the i-th feature was not found (see the array above)
	 * I think the i-th entry in this array is undefined.
	 */
	float optical_flow_feature_error[NFEATURES];

	/* This is the window size to use to avoid the aperture problem (see slide "Optical Flow: Overview"). */
	CvSize optical_flow_window = cvSize(FEATURE_WIDTH,FEATURE_HEIGHT);
		
	/* This termination criteria tells the algorithm to stop when it has either done 20 iterations or when
	 * epsilon is better than .3.  You can play with these parameters for speed vs. accuracy but these values
	 * work pretty well in many situations.
	 */
	CvTermCriteria optical_flow_termination_criteria
	  = cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

	/* This is some workspace for the algorithm.
	 * (The algorithm actually carves the image into pyramids of different resolutions.)
	 */
	allocateOnDemand( &pyramid1, frame_size, IPL_DEPTH_8U, 1 );
	allocateOnDemand( &pyramid2, frame_size, IPL_DEPTH_8U, 1 );

	/* Actually run Pyramidal Lucas Kanade Optical Flow!!
	 * "frame1_1C" is the first frame with the known features.
	 * "frame2_1C" is the second frame where we want to find the first frame's features.
	 * "pyramid1" and "pyramid2" are workspace for the algorithm.
	 * "frame1_features" are the features from the first frame.
	 * "frame2_features" is the (outputted) locations of those features in the second frame.
	 * "number_of_features" is the number of features in the frame1_features array.
	 * "optical_flow_window" is the size of the window to use to avoid the aperture problem.
	 * "5" is the maximum number of pyramids to use.  0 would be just one level.
	 * "optical_flow_found_feature" is as described above (non-zero iff feature found by the flow).
	 * "optical_flow_feature_error" is as described above (error in the flow for this feature).
	 * "optical_flow_termination_criteria" is as described above (how long the algorithm should look).
	 * "0" means disable enhancements.  (For example, the second array isn't pre-initialized with guesses.)
	 */
	cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2, frame1_features, frame2_features, number_of_features, optical_flow_window, 5, optical_flow_found_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0 );
		
	// add found patches
	for(unsigned int i = 0; i < sp.max_current_patches; i++) {
	  if ( optical_flow_found_feature[i] != 0 ) { // feature found
	    if (sp.current_patch_empty(i))	
	      sp.add_similar_patch(im1, frame1_features[i].y, frame1_features[i].x, i);
	    sp.add_similar_patch(im2, frame2_features[i].y, frame2_features[i].x, i);
	  }
	}

#ifdef __GUI__
	// draw arrows
	for(unsigned int i = 0; i < sp.max_current_patches; i++) {
	  if ( optical_flow_found_feature[i] != 0 ) { // feature found
	    draw_arrow(frame1_features[i].x, frame1_features[i].y, 
			  frame2_features[i].x, frame2_features[i].y);
	  }
	}
	enable_window_updates();
	// draw dataset
	if (current_frame % (FRAME_JUMP * 50) == 0)
	  sp.display_dataset(600, 800);


// 	    gui.select_window(featwin);
// 	    idx<ubyte> f1 = im1.narrow(0, optical_flow_window.height, f1y);
// 	    f1 = f1.narrow(1, optical_flow_window.width, f1x);
// 	    idx<ubyte> f2 = im2.narrow(0, optical_flow_window.height, f2y);
// 	    f2 = f2.narrow(1, optical_flow_window.width, f2x);
// 	    gui.draw_matrix(f1, h0, w0, FEATURE_ZOOM, FEATURE_ZOOM); 
// 	    w0 += optical_flow_window.width * FEATURE_ZOOM + 1;
// 	    gui.draw_matrix(f2, h0, w0, FEATURE_ZOOM, FEATURE_ZOOM); 
// 	    w0 += optical_flow_window.width * FEATURE_ZOOM + 4;
// 	    if (w0 > 900) {
// 	      w0 = 0;
// 	      h0 += optical_flow_window.height * FEATURE_ZOOM + 4;
// 	    }
#endif
	  }
	//	sleep(2);
#endif /* __OPENCV__ */
    sp.save_dataset(".");
    return 0;
  }

  //} // end namespace ebl
