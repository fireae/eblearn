/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
 *   All rights reserved.
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

#ifndef CAMERA_DIRECTORY_H_
#define CAMERA_DIRECTORY_H_

#include "camera.h"
#include "tools_utils.h"

#ifdef __BOOST__
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#endif

namespace ebl {

  //! The camera_directory class interfaces with images found (recursively)
  //! in a directory, grabbing all images into idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata> class camera_directory : public camera<Tdata> {
  public:
    // constructors/allocation /////////////////////////////////////////////////

    //! Initialize a directory camera from a root directory.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param directory The root directory to recursively search.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    //! \param randomize Randomize image list if true.
    //! \param npasses Repeat list this number of times.
    //! \param file_pattern The regexp defining the files patterns to search.
    //! \param files An optional list of files to search for in 'directory'
    //!    instead of using the file pattern.
    camera_directory(const char *directory, int height_ = -1, int width_ = -1,
		     bool randomize = false, uint npasses = 1,
		     std::ostream &out = std::cout,
		     std::ostream &err = std::cerr,
		     const char *file_pattern = IMAGE_PATTERN_MAT,
		     const std::list<std::string> *files = NULL);
    //! Initialize a directory camera without a root directory. This constructor
    //! requires a subsequent call to read_directory to initialize images.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    //! \param randomize Randomize image list if true.
    //! \param npasses Repeat list this number of times.
    //! \param file_pattern The regexp defining the files patterns to search.
    camera_directory(int height_ = -1, int width_ = -1,
		     bool randomize = false, uint npasses = 1,
		     std::ostream &out = std::cout,
		     std::ostream &err = std::cerr,
		     const char *file_pattern = IMAGE_PATTERN_MAT);
    //! Find all images recursively from this directory.
    bool read_directory(const char *directory);
    //! Destructor.
    virtual ~camera_directory();

    // frame grabbing //////////////////////////////////////////////////////////

    //! Return a new frame.
    virtual idx<Tdata> grab();
    //! Do not read the file, instead return the filename to be grabbed.
    virtual std::string grab_filename();
    //! Move to the next frame, without returning the frame.
    //! This is called by grab before grabbing.
    //! This can be used to get frames infos without grabbing.
    virtual void next();
    //! Move to the previous frame, without returning the frame.
    virtual void previous();
    //! Return true until all images have been processed.
    virtual bool empty();
    //! Skip n frames.
    virtual void skip(uint n);
    //! Return a name for current frame.
    virtual std::string frame_name();
    //! Return the complete filename for current frame (including path).
    virtual std::string frame_fullname();
    //! Return the subdirectory name for the current frame,
    //! relative to the global input directory.
    virtual std::string get_subdir();
    //! Return the number of frames left to process, -1 if unknown.
    virtual int remaining();
    //! Return the total number of frames to process from the initialization,
    //! of the camera, -1 if unknown.
    virtual int size();

    // members /////////////////////////////////////////////////////////////////
  protected:
    using camera<Tdata>::frame;	//!< frame buffer
    using camera<Tdata>::frame_id_;	//!< frame counter
    using camera<Tdata>::frame_name_;	//!< frame name
    using camera<Tdata>::out;	//!< output stream
    using camera<Tdata>::err;	//!< error output stream
    files_list		*fl;	//!< list of images
    files_list::iterator fli;   //!< Iterator on the list of images.
    std::string		 indir; //!< input directory name
    std::string		 fdir;	//!< directory name
    std::string		 fname;	//!< file name
    std::string          subdir;//!< subdirs to indir.
    std::ostringstream	 oss;	//!< temporary string
    uint                 flsize;	//!< original size of list
    bool                 randomize; //!< Randomize order of images or not.
    uint                 npasses; //!< Number of passes on the list.
    const char          *file_pattern; //!< File search regex.
  };

} // end namespace ebl

#include "camera_directory.hpp"


#endif /* CAMERA_DIRECTORY_H_ */
