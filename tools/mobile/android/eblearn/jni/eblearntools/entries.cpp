/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include <jni.h>
#include <android/bitmap.h>

#include "libidx.h"
#include "libeblearn.h"
#include "libeblearntools.h"

using namespace ebl;

extern "C" {

  typedef float t_net;

  JNIEXPORT jint JNICALL
  Java_com_eblearn_eblearn_detect(JNIEnv* env, jobject obj,
                                  jobject bitmap,
				  jstring jconfname,
                                  jstring jassetspath,
                                  jobject jbb) {
    ////////////////////////////////////////////////////////////////////////////
    // read weight matrix from resources
    configuration conf;
    std::string conf_name;
    std::string assets_path;

    // get conf filename and path to assets
    const char *jftemp = env->GetStringUTFChars(jconfname, 0);
    conf_name = jftemp;
    env->ReleaseStringUTFChars(jconfname, jftemp);
    jftemp = env->GetStringUTFChars(jassetspath, 0);
    assets_path = jftemp;
    env->ReleaseStringUTFChars(jassetspath, jftemp);
    
    ////////////////////////////////////////////////////////////////////////////
    // read image
    AndroidBitmapInfo info;
    void *pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
      eblwarn("AndroidBitmap_getInfo() failed ! error=" << ret << std::endl);
      return -1; }
    eblprint( "Image height " << info.height << " width " << info.width
              << " format: " << info.format << " stride: "
              << info.stride << std::endl);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
      LOGE("Bitmap format is not ARGB_8888 !");
      return -1;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
      eblwarn("AndroidBitmap_lockPixels() failed ! error=" << ret << std::endl);
      return -1; }
    typedef ubyte intype;
    idx<ubyte> im(info.height, info.width, 3);
    ubyte *ptr = (ubyte*)im.idx_ptr();
    intype *p = (intype*) pixels, v;
    for (size_t i = 0; i < info.height * info.width; ++i) {
      *(ptr++) = *(p++); // r
      *(ptr++) = *(p++); // g
      *(ptr++) = *(p++); // b
      p++; // a
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    eblprint( "Created idx image " << im << " (range: " << (int) idx_min(im)
              << ", " << (int) idx_max(im) << ")" << std::endl);
    ////////////////////////////////////////////////////////////////////////////
    // start detection
    
    // load configuration and set root
    std::string conf_fullpath;
    conf_fullpath <<  assets_path << conf_name;
    conf.read(conf_fullpath.c_str(), false, false, true);
    conf.set("root", assets_path.c_str());
    conf.set("output_dir", assets_path.c_str());
    conf.resolve(true);
    

    // detection
    mutex mut;
    detection_thread<t_net> dt(conf, &mut, "detection thread");
    bboxes bb;
    idx<ubyte> detframe;
    idx<uint> total_saved(1);
    std::string processed_fname;
    std::string imfname = "face.jpg";
    uint cnt=0;
    dt.start();
    while (!dt.set_data(im, imfname, imfname, cnt)) millisleep(5);
    bool updated = false;
    while (!updated) {
      updated = dt.get_data(bb, detframe, *(total_saved.idx_ptr()),
			    processed_fname);
      millisleep(5);
    }
    sleep(2);
    dt.stop(true);
    
    eblprint( "Detection finished" << std::endl);

    jclass cls = env->GetObjectClass(jbb);
    jmethodID mid = env->GetMethodID(cls, "add", "(FIIII)V");
    if (mid == 0) {
      eblerror("failed to get add method from JNI");
      return -1;
    }
    bbox *b;
    for (size_t i = 0; i < bb.size(); ++i) {
      b = &bb[i];
      env->CallVoidMethod(jbb, mid, b->confidence, (int)b->h0, (int)b->w0,
                          (int)b->height,
			  (int)b->width);
    }
    
    return (jint) bb.size();
  }

}

