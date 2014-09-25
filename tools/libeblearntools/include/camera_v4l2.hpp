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

#ifndef CAMERA_V4L2_HPP_
#define CAMERA_V4L2_HPP_

#ifdef __LINUX__

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

#endif

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_v4l2<Tdata>::camera_v4l2(const char *device, int height_, int width_,
				  bool grayscale_, bool mode_rgb_)
    : camera<Tdata>(height_, width_), started(false),
      nbuffers(grayscale_ ? 1 : 3), buffers(new void*[nbuffers]),
      sizes(new int[nbuffers]), mode_rgb(mode_rgb_) {
    std::cout << "Initializing V4l2 camera from device " << device
	 << " to " << height_ << "x" << width_ << std::endl;
    if (grayscale_)
      std::cout << "V4l2 output is set to grayscale." << std::endl;
#ifndef __LINUX__
    eblerror("V4l2 is for linux only");
#else
    int fps = 30;
    int height1 = -1; // height returned by camera
    int width1 = -1; // width returned by camera

    fd = open(device, O_RDWR);
    if (fd == -1) eblerror("could not open v4l2 device: " << device);
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    memset((void*) &cap, 0, sizeof(struct v4l2_capability));
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
      //      (==> this cleanup)
      eblerror("could not query v4l2 device");
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
      // (==> this cleanup)
      eblerror("v4l2 device does not support video capture");
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
      // (==> this cleanup)
      eblerror("v4l2 device does not support streaming i/o");
    }
    // resetting cropping to full frame
    memset((void*) &cropcap, 0, sizeof(struct v4l2_cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 == ioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
      crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      crop.c = cropcap.defrect;
      ioctl(fd, VIDIOC_S_CROP, &crop);
    }
    // get list of supported image formats
        memset((void*) &fmt, 0, sizeof(struct v4l2_format));
        ioctl(fd, VIDIOC_G_FMT, &fmt);

    // set format
    memset((void*) &fmt, 0, sizeof(struct v4l2_format));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // TODO: error when ratio not correct
    fmt.fmt.pix.width       = width_;
    fmt.fmt.pix.height      = height_;
    // Looks like most cams dont support RGB output, converting it by hand
    //    if(mode_rgb)
    //      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB;//V4L2_PIX_FMT_RGB32;
    // else
      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_ANY;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
      // (==> this cleanup)
      eblerror("unable to set v4l2 format");
    }
    height1 = fmt.fmt.pix.height;
    width1 = fmt.fmt.pix.width;
    if (height != height1 || width != width1) {
      std::cout << "Warning: requested resolution " << height << "x" << width
	   << " but camera changed it to " << height1 << "x" << width1 << std::endl;
      // enabling resizing as postprocessing
      bresize = true;
    } else // already resized to our target, disable resizing
      bresize = false;

    // set framerate
    struct v4l2_streamparm setfps;
    memset((void*) &setfps, 0, sizeof(struct v4l2_streamparm));
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.numerator = 1;
    setfps.parm.capture.timeperframe.denominator = fps;
    ioctl(fd, VIDIOC_S_PARM, &setfps);
    // allocate and map the buffers
    struct v4l2_requestbuffers rb;
    rb.count = nbuffers;
    rb.type =  V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_REQBUFS, &rb);
    if (ret < 0) {
      // (==> this cleanup)
      eblerror("could not allocate v4l2 buffers");
    }
    ret = 0;
    for (int i = 0; i < nbuffers; i++) {
      struct v4l2_buffer buf;
      int r;
      memset((void*) &buf, 0, sizeof(struct v4l2_buffer));
      buf.index = i;
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      r = ioctl(fd, VIDIOC_QUERYBUF, &buf);
      //       printf("i=%u, length: %u, offset: %u, r=%d\n", i, buf.length, buf.m.offset, r);
      if (r < 0)
	ret = -(i+1);
      if (ret == 0) {
	buffers[i] = mmap(0, buf.length, PROT_READ + PROT_WRITE, MAP_SHARED,
			  fd, buf.m.offset);
	sizes[i] = buf.length;
	if (buffers[i] == MAP_FAILED)
	  ret = -(i+1000);
      }
    }
    if (ret < 0) {
      std::cout << "ret = " << ret << std::endl;
      if (ret > -1000) {
	std::cout << "query buffer " << - (1 + ret) << std::endl;
	//(==> this cleanup)
	eblerror("could not query v4l2 buffer");
      } else {
	std::cout << "map buffer " << - (1000 + ret) << std::endl;
	//(==> this cleanup)
    	eblerror("could not map v4l2 buffer");
      }
    }
    frame = idx<Tdata>(height1, width1, nbuffers);
    print_controls();
    set_boolean_control(V4L2_CID_AUTOGAIN, false);
    set_boolean_control(V4L2_CID_AUTO_WHITE_BALANCE, false);
    //    set_integer_control(V4L2_CID_BACKLIGHT_COMPENSATION, 0);
    print_controls();
#endif
  }

  template <typename Tdata>
  camera_v4l2<Tdata>::~camera_v4l2() {
    if (buffers)
      delete buffers;
    if (sizes)
      delete sizes;
  }

#ifdef __LINUX__

  static void
  enumerate_menu(int fd, struct v4l2_queryctrl &queryctrl,
		 struct v4l2_querymenu &querymenu) {
    printf ("  Menu items:\n");
    memset (&querymenu, 0, sizeof (querymenu));
    querymenu.id = queryctrl.id;

    for (querymenu.index = queryctrl.minimum;
	 (int) querymenu.index <= queryctrl.maximum; querymenu.index++) {
      if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
	printf ("  %s\n", querymenu.name);
      } else {
	eblerror ("VIDIOC_QUERYMENU");
      }
    }
  }

  template <typename Tdata>
  void camera_v4l2<Tdata>::print_controls() {
    std::cout << "__V4l2 camera controls___________________________________"
              << std::endl;

    struct v4l2_queryctrl queryctrl;
    struct v4l2_querymenu querymenu;

    memset (&queryctrl, 0, sizeof (queryctrl));
    for (queryctrl.id = V4L2_CID_BASE;
	 queryctrl.id < V4L2_CID_LASTP1;
	 queryctrl.id++) {
      if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	  continue;

	std::cout << queryctrl.name << " (" << queryctrl.id << "): "
	     << get_control(queryctrl.id) << std::endl;

	if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
	  enumerate_menu (fd, queryctrl, querymenu);
      } else {
	if (errno == EINVAL)
	  continue;
	eblerror ("VIDIOC_QUERYCTRL");
      }
    }

    for (queryctrl.id = V4L2_CID_PRIVATE_BASE;;
	 queryctrl.id++) {
      if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
	if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	  continue;

	printf ("Control %s\n", queryctrl.name);

	if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
	  enumerate_menu (fd, queryctrl, querymenu);
      } else {
	if (errno == EINVAL)
	  break;
	eblerror ("VIDIOC_QUERYCTRL");
      }
    }
    std::cout << "_________________________________________________________"
              << std::endl;
  }

  template <typename Tdata>
  int camera_v4l2<Tdata>::get_control(int id) {
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset (&queryctrl, 0, sizeof (queryctrl));
    queryctrl.id = id;
    if (-1 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
      if (errno != EINVAL) {
	eblerror("VIDIOC_QUERYCTRL");
      } else {
	printf ("V4L2_CID_BRIGHTNESS is not supported\n");
      }
    } else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
      printf ("V4L2_CID_BRIGHTNESS is not supported\n");
    } else {
      memset (&control, 0, sizeof (control));
      control.id = id;
      if (-1 == ioctl (fd, VIDIOC_G_CTRL, &control)) {
	eblerror ("VIDIOC_S_CTRL");
      }
    }
    return control.value;
  }

  template <typename Tdata>
  void camera_v4l2<Tdata>::set_integer_control(int id, int val) {
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset (&queryctrl, 0, sizeof (queryctrl));
    queryctrl.id = id;
    if (-1 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
      if (errno != EINVAL) {
	eblerror("VIDIOC_QUERYCTRL");
      } else {
	printf ("V4L2_CID_BRIGHTNESS is not supported\n");
      }
    } else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
      printf ("V4L2_CID_BRIGHTNESS is not supported\n");
    } else {
      memset (&control, 0, sizeof (control));
      control.id = id;
      control.value = val;
      if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)) {
	eblerror ("VIDIOC_S_CTRL");
      }
    }
  }

  template <typename Tdata>
  void camera_v4l2<Tdata>::set_boolean_control(int id, bool val) {
    struct v4l2_control control;

    memset (&control, 0, sizeof (control));
    control.id = id;
    if (0 == ioctl (fd, VIDIOC_G_CTRL, &control)) {
      control.value += 1;
      /* The driver may clamp the value or return ERANGE, ignored here */
      if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)
	  && errno != ERANGE) {
	eblerror ("VIDIOC_S_CTRL");
      }
      /* Ignore if V4L2_CID_CONTRAST is unsupported */
    } else if (errno != EINVAL) {
      eblerror ("VIDIOC_G_CTRL");
      exit (EXIT_FAILURE);
    }
    control.id = id;
    control.value = val;
    /* Errors ignored */
    ioctl (fd, VIDIOC_S_CTRL, &control);
  }

  ////////////////////////////////////////////////////////////////
  // frame grabbing

  template <typename Tdata>
  void camera_v4l2<Tdata>::start() {
    int ret = 0;
    for (int i = 0; i < nbuffers; ++i) {
      struct v4l2_buffer buf;
      memset((void*) &buf, 0, sizeof(struct v4l2_buffer));
      buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory      = V4L2_MEMORY_MMAP;
      buf.index       = i;
      ret += ioctl(fd, VIDIOC_QBUF, &buf);
    }
    if (ret < 0)
      std::cout << "WARNING: could not enqueue v4l2 buffers" << std::endl;
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
      std::cout << "WARNING: could not start v4l2 capture" << std::endl;
      started = false;
    } else
      started = true;
  }

#endif

  template <typename Tdata>
  idx<Tdata> camera_v4l2<Tdata>::grab() {
#ifdef __LINUX__
    if (!started)
      this->start();
    int ret = 0;
    int m0 = frame.mod(0);
    int m1 = frame.mod(1);
    struct v4l2_buffer buf;
    unsigned char *src;
    Tdata *dst = frame.idx_ptr();
    int i, j, k;
    memset((void*) &buf, 0, sizeof(struct v4l2_buffer));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_DQBUF, &buf);
    src = (unsigned char *)(buffers[buf.index]);
    for (i=0; i < height; i++) {
      for (j=0, k=0; j < width; j++, k+=m1) {
	int j2;
	j2 = j<<1;
	dst[k] = (Tdata) src[j2];
	if (j & 1) {
	  dst[k+1] = (Tdata) src[j2-1];
	  dst[k+2] = (Tdata) src[j2+1];
	} else {
	  dst[k+1] = (Tdata) src[j2+1];
	  dst[k+2] = (Tdata) src[j2+3];
	}
      }
      src += width << 1;
      dst += m0;
    }
    ret += ioctl(fd, VIDIOC_QBUF, &buf);
    if(mode_rgb) {
      // convert yuv to rgb
      idx<float>  fyuv(height,width,nbuffers);
      idx_copy(frame,fyuv);
      idx<float> frame_frgb(height,width,nbuffers);
      yuv_to_rgb(fyuv ,frame_frgb);
      idx<Tdata> frame_rgb(height,width,nbuffers);
      idx_copy(frame_frgb,frame_rgb);
      frame = frame_rgb;
    }
    // todo: resize in postprocessing if different size than expected
    //        e.g.: illegal ratio
#endif
    frame_id_++;
    return this->postprocess();
  }

} // end namespace ebl

#endif /* CAMERA_V4L2_HPP_ */
