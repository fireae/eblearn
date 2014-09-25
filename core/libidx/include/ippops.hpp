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

#ifndef IPPOPS_HPP_
#define IPPOPS_HPP_

#include "numerics.h"
#include "stl.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////////////
  // init

  void ipp_init(int ncores) {
#ifdef __IPP__
    if (ncores > 0)
      ippSetNumThreads(ncores);
    ippGetNumThreads(&ncores);
    std::cout << "Using Intel IPP with " << ncores << " core(s)." << std::endl;
#else
    std::cout << "Not using Intel IPP." << std::endl;
#endif
  }

#ifdef __IPP__

  ////////////////////////////////////////////////////////////////////////
  // ipp checks

#ifdef __IPP_CHECKS__
  template <typename T>
  inline void ipp_checks1(const idx<T> &in) {
    idx_check_contiguous1(in);
    if (in.nelements() > INT_MAX)
      eblerror("TODO: Cannot use long with IPP.");
  }

  template <typename T>
  inline void ipp_checks2(const idx<T> &in1, const idx<T> &in2) {
    idx_checknelems2_all(in1, in2);
    idx_check_contiguous2(in1, in2);
    if (in1.nelements() > INT_MAX)
      eblerror("TODO: Cannot use long with IPP.");
  }
  
  template <typename T>
  inline void ipp_checks3(const idx<T> &in1, const idx<T> &in2,
			  const idx<T> &in3) {
    idx_checknelems3_all(in1, in2, in3);
    idx_check_contiguous3(in1, in2, in3);
    if (in1.nelements() > INT_MAX)
      eblerror("TODO: Cannot use long with IPP.");
  }
#else
  #define ipp_checks1(a)
  #define ipp_checks2(a, b)
  #define ipp_checks3(a, b, c)
#endif

  ////////////////////////////////////////////////////////////////////////
  // ipp functions templates without specialization

  template <typename T>
  void ipp_convolution(const idx<T> &in, const idx<T> &ker, idx<T> &out) {
    eblerror("ipp_convolution : type not available for IPP. Available types are ubyte, int16, float32");
  }

  template <typename T>
  void ipp_copy(const idx<T> &in, idx<T> &out) {
    eblerror("ipp_copy : type not available for IPP. Available types are ubyte, uint16, int16, int32, float32");
  }

  template <typename T>
  void ipp_clear(idx<T> &inp) {
    eblerror("ipp_clear : type not available for IPP. Available types are ubyte, uint16, int16, int32, float32");
  }

  template <typename T>
  void ipp_fill(idx<T> &inp) {
    eblerror("ipp_fill : type not available for IPP. Available types are ubyte, uint16, int16, int32, float32");
  }

  template <typename T>
  void ipp_minus(const idx<T> &inp, idx<T> &out) {
    ipp_checks2(inp, out);
    ipp_clear(out);
    ipp_sub(out, inp);
  }

  template <typename T>
  void ipp_add(const idx<T> &in, idx<T> &out) {
    eblerror("ipp_add in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_add(const idx<T> &in, const idx<T> &in2, idx<T> &out) {
    eblerror("ipp_add not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_addc(idx<T> &in, T c) {
    eblerror("ipp_addc in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_addc(const idx<T> &in, T c, idx<T> &out) {
    eblerror("ipp_addc not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_subc(idx<T> &in, T c) {
    eblerror("ipp_subc in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_subc(const idx<T> &in, T c, idx<T> &out) {
    eblerror("ipp_subc not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_sub(idx<T> &in1, const idx<T> &in2) {
    eblerror("ipp_sub in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_sub(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    eblerror("ipp_sub not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_mul(const idx<T> &in1, idx<T> &in2) {
    eblerror("ipp_mul in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_mul(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    eblerror("ipp_mul not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_m2dotm1(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    eblerror("ipp_mul not-in-place : type not available for IPP. Available types are float32 and float64");
  }

  template <typename T>
  void ipp_m2dotm2(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    eblerror("ipp_mul not-in-place : type not available for IPP. Available types are float32 and float 64");
  }

  template <typename T>
  void ipp_dotc(idx<T> &in1, T c) {
    eblerror("ipp_dotc in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_dotc(const idx<T> &in1, T c, idx<T> &out) {
    eblerror("ipp_mul not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_div(idx<T> &in1, const idx<T> &in2) {
    eblerror("ipp_div in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_div(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    eblerror("ipp_div not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_abs(idx<T> &inp) {
    eblerror("ipp_abs in-place : type not available for IPP. Available types are int16, float32");
  }

  template <typename T>
  void ipp_abs(const idx<T> &in, idx<T> &out) {
    eblerror("ipp_abs not-in-place : type not available for IPP. Available types are int16, float32");
  }

  template <typename T>
  void ipp_sqrt(idx<T> &inp) {
    eblerror("ipp_sqrt in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_sqrt(const idx<T> &in, idx<T> &out) {
    eblerror("ipp_sqrt not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_exp(idx<T> &inp) {
    eblerror("ipp_exp in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_exp(const idx<T> &in, idx<T> &out) {
    eblerror("ipp_exp not-in-place : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  float64 ipp_sum(const idx<T> &inp) {
    eblerror("ipp_sum : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  float64 ipp_sumacc(const idx<T> &in, idx<T> &acc) {
    if (acc.order() != 0)
      eblerror("ipp_sumacc : acc must have order 0");
    float64 sum = ipp_sum(in) + (float64)acc.get();
    acc.set(saturate(sum, T));
    return sum;
  }

  template <typename T>
  float64 ipp_l2norm(const idx<T> &in) {
    eblerror("ipp_l3norm : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  float64 ipp_mean(const idx<T> &in) {
    eblerror("ipp_mean : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_std_normalize(const idx<T> &in, idx<T> &out, T* mean) {
    eblerror("ipp_std_normalize : type not available for IPP. Available types are ubyte, float32");
  }

#ifdef __IPP_DOT__
  template <typename T>
  float64 ipp_dot(const idx<T> &in1, const idx<T> &in2) {
    eblerror("ipp_dot : type not available for IPP. Available types are ubyte, byte, uint16, int16, uint32, int32, float32");
  }

  template <typename T>
  float64 ipp_dotacc(const idx<T> &in1, const idx<T> &in2,
		     idx<T> &acc) {
    if (acc.order() != 0)
      eblerror("ipp_sumacc : acc must have order 0");
    float64 sum = ipp_dot(in1, in2) + (float64)acc.get();
    acc.set(saturate(sum, T));
    return sum;
  }
#endif

  template <typename T>
  T ipp_max(const idx<T> &inp) {
    eblerror("ipp_max : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  intg ipp_indexmax(const idx<T> &inp) {
    eblerror("ipp_indexmax : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  T ipp_min(const idx<T> &inp) {
    eblerror("ipp_min : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  intg ipp_indexmin(const idx<T> &inp) {
    eblerror("ipp_indexmin : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_maxevery(const idx<T> &in1, idx<T> &in2) {
    eblerror("ipp_maxevery : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_maxevery(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    ipp_copy(in2, out);
    ipp_maxevery(in1, out);
  }

  template <typename T>
  float64 ipp_sqrdist(const idx<T> &i1, const idx<T> &i2) {
    eblerror("ipp_sqrdist : type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  float64 ipp_sqrdist(const idx<T> &in1, const idx<T> &in2, idx<T> &out) {
    if (out.order() != 0)
      eblerror("ipp_sqrdist : out must have order 0");
    float64 ret = ipp_sqrdist(in1, in2);
    out.set(saturate(ret, T));
    return ret;
  }

  template <typename T>
  void ipp_threshold_lt(idx<T> &in, T th) {
    eblerror("ipp_threshold_lt (in-place): type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_threshold_lt(const idx<T> &in, T th, idx<T> &out) {
    eblerror("ipp_threshold_lt (not-in-place): type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_threshold_gt(idx<T> &in, T th) {
    eblerror("ipp_threshold_gt (in-place): type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  template <typename T>
  void ipp_threshold_gt(const idx<T> &in, T th, idx<T> &out) {
    eblerror("ipp_threshold_gt (not-in-place): type not available for IPP. Available types are ubyte, uint16, int16, float32");
  }

  ////////////////////////////////////////////////////////////////////////
  // ipp implementations

  // templates for ipp_convolution
  // TODO: handle non contiguous?
  
  //TODO : remove in release ?
  template <typename T>
  inline void ipp_convolution_check_size (const idx<T> &in, const idx<T> &ker,
					  idx<T> &out) {
    ipp_checks1(in);
    ipp_checks1(ker);
    ipp_checks1(out);
    if ((in.dim(0) > INT_MAX) || (in.dim(1) > INT_MAX) ||
	(ker.dim(0) > INT_MAX) || (ker.dim(1) > INT_MAX) ||
	(in.mod(0) > INT_MAX) || (ker.mod(0) > INT_MAX) ||
	(out.mod(0) > INT_MAX))
      eblerror("TODO: Cannot use long with IPP.");
  }
    
  //   templates
  template <>
  void ipp_convolution(const idx<ubyte> &in, const idx<ubyte> &ker,
		      idx<ubyte> &out) {
    ipp_convolution_check_size(in, ker, out);
    IppiSize insize, kersize;
    insize.height = in.dim(0);
    insize.width = in.dim(1);
    kersize.height = ker.dim(0);
    kersize.width = ker.dim(1);
    ippiConvValid_8u_C1R(in.idx_ptr(),
			 sizeof(ubyte) * (int)in.mod(0), //instep
			 insize, ker.idx_ptr(),
			 sizeof(ubyte) * (int)ker.mod(0), //kerstep
				kersize, out.idx_ptr(),
			 sizeof(ubyte) * (int)out.mod(0), //outstep
			 1);
  }

  template <>
  void ipp_convolution(const idx<int16> &in, const idx<int16> &ker,
		      idx<int16> &out) {
    ipp_convolution_check_size(in, ker, out);
    IppiSize insize, kersize;
    insize.height = in.dim(0);
    insize.width = in.dim(1);
    kersize.height = ker.dim(0);
    kersize.width = ker.dim(1);
    ippiConvValid_16s_C1R(in.idx_ptr(),
			  sizeof(int16) * (int)in.mod(0), //instep
			  insize, ker.idx_ptr(),
			  sizeof(int16) * (int)ker.mod(0), //kerstep
			  kersize, out.idx_ptr(),
			  sizeof(int16) * (int)out.mod(0), //outstep
			  1);
  }

  template <>
  void ipp_convolution(const idx<float32> &in, const idx<float32> &ker,
		      idx<float32> &out) {
    ipp_convolution_check_size(in, ker, out);
    IppiSize insize, kersize;
    insize.height = in.dim(0);
    insize.width = in.dim(1);
    kersize.height = ker.dim(0);
    kersize.width = ker.dim(1);
    ippiConvValid_32f_C1R(in.idx_ptr(),
			  sizeof(float32) * (int)in.mod(0), //instep
			  insize, ker.idx_ptr(),
			  sizeof(float32) * (int)ker.mod(0), //kerstep
			  kersize, out.idx_ptr(),
			  sizeof(float32) * (int)out.mod(0)); //outstep
  }

  // templates for ipp_copy

  template <>
  void ipp_copy(const idx<ubyte> &in, idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiCopy_8u_C1R(in.idx_ptr(),  sizeof(ubyte),
		    out.idx_ptr(), sizeof(ubyte),
		    insize);
  }

  template <>
  void ipp_copy(const idx<uint16> &in, idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiCopy_16u_C1R(in.idx_ptr(),  sizeof(uint16),
		     out.idx_ptr(), sizeof(uint16),
		     insize);
  }

  template <>
  void ipp_copy(const idx<int16> &in, idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiCopy_16s_C1R(in.idx_ptr(),  sizeof(int16),
		     out.idx_ptr(), sizeof(int16),
		     insize);
  }

  template <>
  void ipp_copy(const idx<int32> &in, idx<int32> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiCopy_32s_C1R(in.idx_ptr(),  sizeof(int32),
		     out.idx_ptr(), sizeof(int32),
		     insize);
  }


  template <>
  void ipp_copy(const idx<float32> &in, idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiCopy_32f_C1R(in.idx_ptr(),  sizeof(float32),
		     out.idx_ptr(), sizeof(float32),
		     insize);
  }

  // templates for ipp_clear

  template <>
  void ipp_clear(idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_8u_C1R(0, inp.idx_ptr(), sizeof(ubyte), insize);
  }

  template <>
  void ipp_clear(idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_16u_C1R(0, inp.idx_ptr(), sizeof(uint16), insize);
  }

  template <>
  void ipp_clear(idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_16s_C1R(0, inp.idx_ptr(), sizeof(int16), insize);
  }

  template <>
  void ipp_clear(idx<int32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_32s_C1R(0, inp.idx_ptr(), sizeof(int32), insize);
  }

  template <>
  void ipp_clear(idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_32f_C1R(0, inp.idx_ptr(), sizeof(float32), insize);
  }

  // templates for ipp_fill

  template <>
  void ipp_fill(idx<ubyte> &inp, ubyte v) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_8u_C1R(v, inp.idx_ptr(), sizeof(ubyte), insize);
  }

  template <>
  void ipp_fill(idx<uint16> &inp, uint16 v) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_16u_C1R(v, inp.idx_ptr(), sizeof(uint16), insize);
  }

  template <>
  void ipp_fill(idx<int16> &inp, int16 v) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_16s_C1R(v, inp.idx_ptr(), sizeof(int16), insize);
  }

  template <>
  void ipp_fill(idx<int32> &inp, int32 v) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_32s_C1R(v, inp.idx_ptr(), sizeof(int32), insize);
  }

  template <>
  void ipp_fill(idx<float32> &inp, float32 v) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSet_32f_C1R(v, inp.idx_ptr(), sizeof(float32), insize);
  }

  // templates for in-place ipp_add

  template <>
  void ipp_add(const idx<float32> &in1, idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_32f_C1IR(in1.idx_ptr(), sizeof(float32),
		     in2.idx_ptr(), sizeof(float32),
		     insize);
  }

  template <>
  void ipp_add(const idx<ubyte> &in1, idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_8u_C1IRSfs(in1.idx_ptr(), sizeof(ubyte),
		       in2.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }

  template <>
  void ipp_add(const idx<uint16> &in1, idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_16u_C1IRSfs(in1.idx_ptr(), sizeof(uint16),
			in2.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_add(const idx<int16> &in1, idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_16s_C1IRSfs(in1.idx_ptr(), sizeof(int16),
			in2.idx_ptr(), sizeof(int16),
			insize, 0);
  }


  // templates for not in place ipp_add

  template <>
  void ipp_add(const idx<float32> &in1, const idx<float32> &in2,
	      idx<float32> &out) {
    ipp_checks3(in1, in2, out);

    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_32f_C1R(in1.idx_ptr(), sizeof(float32),
		    in2.idx_ptr(), sizeof(float32),
		    out.idx_ptr(), sizeof(float32),
		    insize);
  }

  template <>
  void ipp_add(const idx<ubyte> &in1, const idx<ubyte> &in2,
	      idx<ubyte> &out) {
    ipp_checks3(in1, in2, out);

    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_8u_C1RSfs(in1.idx_ptr(), sizeof(ubyte),
		      in2.idx_ptr(), sizeof(ubyte),
		      out.idx_ptr(), sizeof(ubyte),
		      insize, 0);
  }

  template <>
  void ipp_add(const idx<uint16> &in1, const idx<uint16> &in2,
	      idx<uint16> &out) {
    ipp_checks3(in1, in2, out);

    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_16u_C1RSfs(in1.idx_ptr(), sizeof(uint16),
		       in2.idx_ptr(), sizeof(uint16),
		       out.idx_ptr(), sizeof(uint16),
		       insize, 0);
  }

  template <>
  void ipp_add(const idx<int16> &in1, const idx<int16> &in2,
	      idx<int16> &out) {
    ipp_checks3(in1, in2, out);

    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiAdd_16s_C1RSfs(in1.idx_ptr(), sizeof(int16),
		       in2.idx_ptr(), sizeof(int16),
		       out.idx_ptr(), sizeof(int16),
		       insize, 0);
  }

  // templates for in-place ipp_addc

  template <>
  void ipp_addc(idx<float32> &in, float32 c) {
    ipp_checks1(in);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_32f_C1IR(c, in.idx_ptr(), sizeof(float32), insize);
  }

  template <>
  void ipp_addc(idx<ubyte> &in, ubyte c) {
    ipp_checks1(in);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_8u_C1IRSfs(c, in.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_addc(idx<int16> &in, int16 c) {
    ipp_checks1(in);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_16s_C1IRSfs(c, in.idx_ptr(), sizeof(int16), insize, 0);
  }

  template <>
  void ipp_addc(idx<uint16> &in, uint16 c) {
    ipp_checks1(in);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_16u_C1IRSfs(c, in.idx_ptr(), sizeof(uint16), insize, 0);
  }

  // templates for not-in-place addc

  template <>
  void ipp_addc(const idx<float32> &in, float32 c, idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_32f_C1R(in.idx_ptr(), sizeof(float32), c,
		     out.idx_ptr(), sizeof(float32), insize);
  }

  template <>
  void ipp_addc(const idx<ubyte> &in, ubyte c, idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_8u_C1RSfs(in.idx_ptr(), sizeof(ubyte), c,
		       out.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_addc(const idx<uint16> &in, uint16 c, idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_16u_C1RSfs(in.idx_ptr(), sizeof(uint16), c,
			out.idx_ptr(), sizeof(uint16), insize, 0);
  }

  template <>
  void ipp_addc(const idx<int16> &in, int16 c, idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiAddC_16s_C1RSfs(in.idx_ptr(), sizeof(int16), c,
			out.idx_ptr(), sizeof(int16), insize, 0);
  }

  // templates for in-place ipp_subc

  template <>
  void ipp_subc(idx<ubyte> &in, ubyte c) {
    ipp_checks1(in);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_8u_C1IRSfs(c, in.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_subc(idx<uint16> &in, uint16 c) {
    ipp_checks1(in);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_16u_C1IRSfs(c, in.idx_ptr(), sizeof(uint16), insize, 0);
  }

  template <>
  void ipp_subc(idx<int16> &in, int16 c) {
    ipp_checks1(in);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_16s_C1IRSfs(c, in.idx_ptr(), sizeof(int16), insize, 0);
  }

  template <>
  void ipp_subc(idx<float32> &in, float32 c) {
    ipp_checks1(in);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_32f_C1IR(c, in.idx_ptr(), sizeof(float32), insize);
  }

  // templates for not-in-place ipp_subc

  template <>
  void ipp_subc(const idx<ubyte> &in, ubyte c, idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_8u_C1RSfs(in.idx_ptr(), sizeof(ubyte),
		       c, out.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }

  template <>
  void ipp_subc(const idx<uint16> &in, uint16 c, idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_16u_C1RSfs(in.idx_ptr(), sizeof(uint16),
			c, out.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_subc(const idx<int16> &in, int16 c, idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_16s_C1RSfs(in.idx_ptr(), sizeof(int16),
			c, out.idx_ptr(), sizeof(int16),
			insize, 0);
  }

  template <>
  void ipp_subc(const idx<float32> &in, float32 c, idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiSubC_32f_C1R(in.idx_ptr(), sizeof(float32),
		     c, out.idx_ptr(), sizeof(float32),
		     insize);
  }

  // templates for ipp_sub

  template <>
  void ipp_sub(idx<float32> &in1, const idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_32f_C1IR(in2.idx_ptr(), sizeof(float32),
		     in1.idx_ptr(), sizeof(float32),
		     insize);
  }

  template <>
  void ipp_sub(idx<ubyte> &in1, const idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_8u_C1IRSfs(in2.idx_ptr(), sizeof(ubyte),
		       in1.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }

  template <>
  void ipp_sub(idx<uint16> &in1, const idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_16u_C1IRSfs(in2.idx_ptr(), sizeof(uint16),
			in1.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_sub(idx<int16> &in1, const idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_16s_C1IRSfs(in2.idx_ptr(), sizeof(int16),
			in1.idx_ptr(), sizeof(int16),
			insize, 0);
  }


  // templates for not-in-place ipp_sub

  template <>
  void ipp_sub(const idx<float32> &in1, const idx<float32> &in2,
	      idx<float32> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_32f_C1R(in2.idx_ptr(), sizeof(float32),
		    in1.idx_ptr(), sizeof(float32),
		    out.idx_ptr(), sizeof(float32),
		    insize);
  }

  template <>
  void ipp_sub(const idx<ubyte> &in1, const idx<ubyte> &in2,
	      idx<ubyte> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_8u_C1RSfs(in2.idx_ptr(), sizeof(ubyte),
		      in1.idx_ptr(), sizeof(ubyte),
		      out.idx_ptr(), sizeof(ubyte),
		      insize, 0);
  }

  template <>
  void ipp_sub(const idx<uint16> &in1, const idx<uint16> &in2,
	      idx<uint16> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_16u_C1RSfs(in2.idx_ptr(), sizeof(uint16),
		       in1.idx_ptr(), sizeof(uint16),
		       out.idx_ptr(), sizeof(uint16),
		       insize, 0);
  }

  template <>
  void ipp_sub(const idx<int16> &in1, const idx<int16> &in2,
	      idx<int16> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiSub_16s_C1RSfs(in2.idx_ptr(), sizeof(int16),
		       in1.idx_ptr(), sizeof(int16),
		       out.idx_ptr(), sizeof(int16),
		       insize, 0);
  }

  // templates for in-place ipp_mul

  template <>
  void ipp_mul(const idx<float32> &in1, idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_32f_C1IR(in1.idx_ptr(), sizeof(float32),
		     in2.idx_ptr(), sizeof(float32),
		     insize);
  }

  template <>
  void ipp_mul(const idx<ubyte> &in1, idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_8u_C1IRSfs(in1.idx_ptr(), sizeof(ubyte),
		       in2.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }

  template <>
  void ipp_mul(const idx<uint16> &in1, idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_16u_C1IRSfs(in1.idx_ptr(), sizeof(uint16),
			in2.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_mul(const idx<int16> &in1, idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_16s_C1IRSfs(in1.idx_ptr(), sizeof(int16),
			in2.idx_ptr(), sizeof(int16),
			insize, 0);
  }

  // templates for not-in-place ipp_mul

  template <>
  void ipp_mul(const idx<float32> &in1, const idx<float32> &in2,
	      idx<float32> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_32f_C1R(in1.idx_ptr(), sizeof(float32),
		    in2.idx_ptr(), sizeof(float32),
		    out.idx_ptr(), sizeof(float32),
		    insize);
  }

  template <>
  void ipp_mul(const idx<ubyte> &in1, const idx<ubyte> &in2,
	      idx<ubyte> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_8u_C1RSfs(in1.idx_ptr(), sizeof(ubyte),
		      in2.idx_ptr(), sizeof(ubyte),
		      out.idx_ptr(), sizeof(ubyte),
		      insize, 0);
  }


  template <>
  void ipp_mul(const idx<uint16> &in1, const idx<uint16> &in2,
	      idx<uint16> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_16u_C1RSfs(in1.idx_ptr(), sizeof(uint16),
		       in2.idx_ptr(), sizeof(uint16),
		       out.idx_ptr(), sizeof(uint16),
		       insize, 0);
  }


  template <>
  void ipp_mul(const idx<int16> &in1, const idx<int16> &in2,
	      idx<int16> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMul_16s_C1RSfs(in1.idx_ptr(), sizeof(int16),
		       in2.idx_ptr(), sizeof(int16),
		       out.idx_ptr(), sizeof(int16),
		       insize, 0);
  }

  // templates for not-in-place ipp_m2dotm1 ////////////////////////////////////

  template <>
  void ipp_m2dotm1(const idx<float32> &in1, const idx<float32> &in2,
		   idx<float32> &y){
    idx_check_contiguous3(in1, in2, y);
    int so = sizeof (float32);
    ippmMul_mv_32f(in1.idx_ptr(), (int) in1.mod(0) * so, (int) in1.mod(1) * so,
		   (int) in1.dim(1), (int) in1.dim(0),
		   in2.idx_ptr(), (int) in2.mod(0) * so, (int) in2.dim(0),
		   y.idx_ptr(), (int) y.mod(0) * so);
  }

  template <>
  void ipp_m2dotm1(const idx<float64> &in1, const idx<float64> &in2,
		   idx<float64> &y){
    idx_check_contiguous3(in1, in2, y);
    int so = sizeof (float64);
    ippmMul_mv_64f(in1.idx_ptr(), (int) in1.mod(0) * so, (int) in1.mod(1) * so,
		   (int) in1.dim(1), (int) in1.dim(0),
		   in2.idx_ptr(), (int) in2.mod(0) * so, (int) in2.dim(0),
		   y.idx_ptr(), (int) y.mod(0) * so);
  }

  // templates for not-in-place ipp_m2dotm1 ////////////////////////////////////

  template <>
  void ipp_m2dotm2(const idx<float32> &in1, const idx<float32> &in2,
		   idx<float32> &y){
    std::cout << "using ipp m2m2" << std::endl;
    idx_check_contiguous3(in1, in2, y);
    int so = sizeof (float32);
    ippmMul_mm_32f(in1.idx_ptr(), (int) in1.mod(0) * so, (int) in1.mod(1) * so,
		   (int) in1.dim(1), (int) in1.dim(0),
		   in2.idx_ptr(), (int) in2.mod(0) * so, (int) in1.mod(1) * so,
		   (int) in2.dim(1), (int) in2.dim(0),
		   y.idx_ptr(), (int) y.mod(0) * so, (int) y.mod(1) * so);
  }

  template <>
  void ipp_m2dotm2(const idx<float64> &in1, const idx<float64> &in2,
		   idx<float64> &y){
    std::cout << "using ipp m2m2" << std::endl;
    idx_check_contiguous3(in1, in2, y);
    int so = sizeof (float64);
    ippmMul_mm_64f(in1.idx_ptr(), (int) in1.mod(0) * so, (int) in1.mod(1) * so,
		   (int) in1.dim(1), (int) in1.dim(0),
		   in2.idx_ptr(), (int) in2.mod(0) * so, (int) in1.mod(1) * so,
		   (int) in2.dim(1), (int) in2.dim(0),
		   y.idx_ptr(), (int) y.mod(0) * so, (int) y.mod(1) * so);
  }

  // templates for in-place ipp_dotc

  template <>
  void ipp_dotc(idx<float32> &inp, float32 c) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiMulC_32f_C1IR(c, inp.idx_ptr(), sizeof(float32), insize);
  }

  template <>
  void ipp_dotc(idx<ubyte> &inp, ubyte c) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiMulC_8u_C1IRSfs(c, inp.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_dotc(idx<uint16> &inp, uint16 c) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiMulC_16u_C1IRSfs(c, inp.idx_ptr(), sizeof(uint16), insize, 0);
  }

  template <>
  void ipp_dotc(idx<int16> &inp, int16 c) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiMulC_16s_C1IRSfs(c, inp.idx_ptr(), sizeof(int16), insize, 0);
  }

  // templates for not-in-place ipp_dotc

  template <>
  void ipp_dotc(const idx<float32> &in, float32 c, idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiMulC_32f_C1R(in.idx_ptr(), sizeof(float32), c,
		     out.idx_ptr(), sizeof(float32), insize);
  }

  template <>
  void ipp_dotc(const idx<ubyte> &in, ubyte c, idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiMulC_8u_C1RSfs(in.idx_ptr(), sizeof(ubyte), c,
		       out.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_dotc(const idx<uint16> &in, uint16 c, idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiMulC_16u_C1RSfs(in.idx_ptr(), sizeof(uint16), c,
			out.idx_ptr(), sizeof(uint16), insize, 0);
  }

  template <>
  void ipp_dotc(const idx<int16> &in, int16 c, idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize	insize;
    insize.height = in.nelements();
    insize.width = 1; 
    ippiMulC_16s_C1RSfs(in.idx_ptr(), sizeof(int16), c,
			out.idx_ptr(), sizeof(int16), insize, 0);
  }


  // templates for in-place ipp_div

  template <>
  void ipp_div(idx<float32> &in1, const idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_32f_C1IR(in2.idx_ptr(), sizeof(float32),
		     in1.idx_ptr(), sizeof(float32),
		     insize);
  }

  template <>
  void ipp_div(idx<ubyte> &in1, const idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_8u_C1IRSfs(in2.idx_ptr(), sizeof(ubyte),
		       in1.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }

  template <>
  void ipp_div(idx<uint16> &in1, const idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_16u_C1IRSfs(in2.idx_ptr(), sizeof(uint16),
			in1.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_div(idx<int16> &in1, const idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_16s_C1IRSfs(in2.idx_ptr(), sizeof(int16),
			in1.idx_ptr(), sizeof(int16),
			insize, 0);
  }

  // templates for not-in-place ipp_div

  template <>
  void ipp_div(const idx<float32> &in1, const idx<float32> &in2,
	      idx<float32> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_32f_C1R(in2.idx_ptr(), sizeof(float32),
		    in1.idx_ptr(), sizeof(float32),
		    out.idx_ptr(), sizeof(float32),
		    insize);
  }

  template <>
  void ipp_div(const idx<ubyte> &in1, const idx<ubyte> &in2,
	      idx<ubyte> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_8u_C1RSfs(in2.idx_ptr(), sizeof(ubyte),
		      in1.idx_ptr(), sizeof(ubyte),
		      out.idx_ptr(), sizeof(ubyte),
		      insize, 0);
  }

  template <>
  void ipp_div(const idx<uint16> &in1, const idx<uint16> &in2,
	      idx<uint16> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_16u_C1RSfs(in2.idx_ptr(), sizeof(uint16),
		       in1.idx_ptr(), sizeof(uint16),
		       out.idx_ptr(), sizeof(uint16),
		       insize, 0);
  }

  template <>
  void ipp_div(const idx<int16> &in1, const idx<int16> &in2,
	      idx<int16> &out) {
    ipp_checks3(in1, in2, out);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiDiv_16s_C1RSfs(in2.idx_ptr(), sizeof(int16),
		       in1.idx_ptr(), sizeof(int16),
		       out.idx_ptr(), sizeof(int16),
		       insize, 0);
  }

  // ipp_inv

  void ipp_inv(const idx<float32> &in, idx<float32> &out) {
    ipp_fill(out, 1.0f);
    ipp_div(out, in);
  }

  // templates for in-place ipp_abs

  template <>
  void ipp_abs(idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiAbs_16s_C1IR(inp.idx_ptr(), sizeof(int16), insize);
  }

  template <>
  void ipp_abs(idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiAbs_32f_C1IR(inp.idx_ptr(), sizeof(float32), insize);
  }

  // templates for not-in-place ipp_abs

  template <>
  void ipp_abs(const idx<int16> &inp, idx<int16> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiAbs_16s_C1R(inp.idx_ptr(), sizeof(int16),
		    out.idx_ptr(), sizeof(int16),
		    insize);
  }

  template <>
  void ipp_abs(const idx<float32> &inp, idx<float32> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiAbs_32f_C1R(inp.idx_ptr(), sizeof(float32),
		    out.idx_ptr(), sizeof(float32),
		    insize);
  }
  
  // templates for in-place ipp_sqrt

  template <>
  void ipp_sqrt(idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_8u_C1IRSfs(inp.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_sqrt(idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_16u_C1IRSfs(inp.idx_ptr(), sizeof(uint16), insize, 0);
  } 

  template <>
  void ipp_sqrt(idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_16s_C1IRSfs(inp.idx_ptr(), sizeof(int16), insize, 0);
  }

  template <>
  void ipp_sqrt(idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_32f_C1IR(inp.idx_ptr(), sizeof(float32), insize);
  }

  // templates for not-in-place ipp_sqrt

  template <>
  void ipp_sqrt(const idx<ubyte> &inp, idx<ubyte> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_8u_C1RSfs(inp.idx_ptr(), sizeof(ubyte),
		       out.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }
  
  template <>
  void ipp_sqrt(const idx<uint16> &inp, idx<uint16> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_16u_C1RSfs(inp.idx_ptr(), sizeof(uint16),
			out.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_sqrt(const idx<int16> &inp, idx<int16> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_16s_C1RSfs(inp.idx_ptr(), sizeof(int16),
			out.idx_ptr(), sizeof(int16),
			insize, 0);
  }

  template <>
  void ipp_sqrt(const idx<float32> &inp, idx<float32> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiSqrt_32f_C1R(inp.idx_ptr(), sizeof(float32),
		     out.idx_ptr(), sizeof(float32),
		     insize);
  }

  // templates for in-place ipp_exp

  template <>
  void ipp_exp(idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_8u_C1IRSfs(inp.idx_ptr(), sizeof(ubyte), insize, 0);
  }

  template <>
  void ipp_exp(idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_16u_C1IRSfs(inp.idx_ptr(), sizeof(uint16), insize, 0);
  } 

  template <>
  void ipp_exp(idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_16s_C1IRSfs(inp.idx_ptr(), sizeof(int16), insize, 0);
  }

  template <>
  void ipp_exp(idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_32f_C1IR(inp.idx_ptr(), sizeof(float32), insize);
  }

  // templates for not-in-place ipp_exp

  template <>
  void ipp_exp(const idx<ubyte> &inp, idx<ubyte> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_8u_C1RSfs(inp.idx_ptr(), sizeof(ubyte),
		       out.idx_ptr(), sizeof(ubyte),
		       insize, 0);
  }
  
  template <>
  void ipp_exp(const idx<uint16> &inp, idx<uint16> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_16u_C1RSfs(inp.idx_ptr(), sizeof(uint16),
			out.idx_ptr(), sizeof(uint16),
			insize, 0);
  }

  template <>
  void ipp_exp(const idx<int16> &inp, idx<int16> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_16s_C1RSfs(inp.idx_ptr(), sizeof(int16),
			out.idx_ptr(), sizeof(int16),
			insize, 0);
  }

  template <>
  void ipp_exp(const idx<float32> &inp, idx<float32> &out) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ippiExp_32f_C1R(inp.idx_ptr(), sizeof(float32),
		     out.idx_ptr(), sizeof(float32),
		     insize);
  }

  // templates for ipp_sum

  template <>
  float64 ipp_sum(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiSum_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sum(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiSum_16u_C1R(inp.idx_ptr(), sizeof(uint16),
		    insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sum(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiSum_16s_C1R(inp.idx_ptr(), sizeof(int16),
		    insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sum(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
#ifdef __IPPFAST__
    ippiSum_32f_C1R(inp.idx_ptr(), sizeof(float32),
		    insize, &ret, ippAlgHintFast);
#else
#ifndef __IPPACC__
#error either __IPPFAST__ or __IPPACC__ must be defined
#endif
    ippiSum_32f_C1R(inp.idx_ptr(), sizeof(float32),
		    insize, &ret, ippAlgHintAccurate);
#endif
    return ret;
  }

  // templates for ipp_numabs

  template <>
  float64 ipp_sumabs(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiNorm_L1_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		       insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sumabs(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiNorm_L1_16u_C1R(inp.idx_ptr(), sizeof(uint16),
			insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sumabs(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiNorm_L1_16s_C1R(inp.idx_ptr(), sizeof(int16),
			insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sumabs(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
#ifdef __IPPFAST__
    ippiNorm_L1_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &ret, ippAlgHintFast);
#else
#ifndef __IPPACC__
#error either __IPPFAST__ or __IPPACC__ must be defined
#endif
    ippiNorm_L1_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &ret, ippAlgHintAccurate);
#endif
    return ret;
  }

  // templates for ipp_l2norm

  template <>
  float64 ipp_l2norm(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiNorm_L2_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		       insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_l2norm(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiNorm_L2_16u_C1R(inp.idx_ptr(), sizeof(uint16),
			insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_l2norm(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiNorm_L2_16s_C1R(inp.idx_ptr(), sizeof(int16),
			insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_l2norm(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
#ifdef __IPPFAST__
    ippiNorm_L2_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &ret, ippAlgHintFast);
#else
#ifndef __IPPACC__
#error either __IPPFAST__ or __IPPACC__ must be defined
#endif
    ippiNorm_L2_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &ret, ippAlgHintAccurate);
#endif
    return ret;
  }

  // templates for ipp_mean

  template <>
  float64 ipp_mean(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiMean_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_mean(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiMean_16u_C1R(inp.idx_ptr(), sizeof(uint16),
		    insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_mean(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
    ippiMean_16s_C1R(inp.idx_ptr(), sizeof(int16),
		    insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_mean(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 ret;
#ifdef __IPPFAST__
    ippiMean_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &ret, ippAlgHintFast);
#else
#ifndef __IPPACC__
#error either __IPPFAST__ or __IPPACC__ must be defined
#endif
    ippiMean_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &ret, ippAlgHintAccurate);
#endif
    return ret;
  }

  // templates for ipp_std_normalize

  template <>
  void ipp_std_normalize(const idx<ubyte> &inp, idx<ubyte> &out,
			 ubyte* mean_p) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 mean, std_dev;
    if (mean_p == NULL) {
      ippiMean_StdDev_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
			     insize, &mean, &std_dev);
    } else {
      ippiMean_StdDev_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
			     insize, NULL, &std_dev);
      mean = *mean_p;
    }
    ipp_subc(inp, (ubyte)mean, out);
    idxdim size = idxdim(inp);
    idx<ubyte> temp(size);
    ipp_fill(temp, (ubyte)std_dev);
    ipp_div(out, temp);
  }

  template <>
  void ipp_std_normalize(const idx<float32> &inp, idx<float32> &out,
			 float32* mean_p) {
    ipp_checks2(inp, out);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float64 mean, std_dev;
    if (mean_p == NULL) {
      ippiMean_StdDev_32f_C1R(inp.idx_ptr(), sizeof(float32),
			      insize, &mean, &std_dev);
    } else {
      ippiMean_StdDev_32f_C1R(inp.idx_ptr(), sizeof(float32),
			      insize, NULL, &std_dev);
      mean = *mean_p;
    }
    ipp_addc(inp, - (float32)mean, out);
    ipp_dotc(out, (float32)(1.0 / std_dev));
  }

#ifdef __IPP_DOT__
  // templates for ipp_dot

  template <>
  float64 ipp_dot(const idx<ubyte> &in1, const idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
    ippiDotProd_8u64f_C1R(in1.idx_ptr(), sizeof(ubyte),
			  in2.idx_ptr(), sizeof(ubyte),
			  insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_dot(const idx<byte> &in1, const idx<byte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
    ippiDotProd_8s64f_C1R(in1.idx_ptr(), sizeof(byte),
			  in2.idx_ptr(), sizeof(byte),
			  insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_dot(const idx<uint16> &in1, const idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
    ippiDotProd_16u64f_C1R(in1.idx_ptr(), sizeof(uint16),
			   in2.idx_ptr(), sizeof(uint16),
			   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_dot(const idx<int16> &in1, const idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
    ippiDotProd_16s64f_C1R(in1.idx_ptr(), sizeof(int16),
			   in2.idx_ptr(), sizeof(int16),
			   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_dot(const idx<uint32> &in1, const idx<uint32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
    ippiDotProd_32u64f_C1R(in1.idx_ptr(), sizeof(uint32),
			   in2.idx_ptr(), sizeof(uint32),
			   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_dot(const idx<int32> &in1, const idx<int32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
    ippiDotProd_32s64f_C1R(in1.idx_ptr(), sizeof(int32),
			   in2.idx_ptr(), sizeof(int32),
			   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_dot(const idx<float32> &in1, const idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    float64 ret;
#ifdef __IPPFAST__
    ippiDotProd_32f64f_C1R(in1.idx_ptr(), sizeof(float32),
			   in2.idx_ptr(), sizeof(float32),
			   insize, &ret, ippAlgHintFast);
#else
#ifndef __IPPACC__
#error either __IPPFAST__ or __IPPACC__ must be defined
#endif
    ippiDotProd_32f64f_C1R(in1.idx_ptr(), sizeof(float32),
			   in2.idx_ptr(), sizeof(float32),
			   insize, &ret, ippAlgHintAccurate);
#endif
    return ret;
  }
#endif

  // templates for ipp_max

  template <>
  ubyte ipp_max(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ubyte ret;
    ippiMax_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		   insize, &ret);
    return ret;
  }

  template <>
  uint16 ipp_max(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    uint16 ret;
    ippiMax_16u_C1R(inp.idx_ptr(), sizeof(uint16),
		    insize, &ret);
    return ret;
  }

  template <>
  int16 ipp_max(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    int16 ret;
    ippiMax_16s_C1R(inp.idx_ptr(), sizeof(int16),
		   insize, &ret);
    return ret;
  }

  template <>
  float32 ipp_max(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float32 ret;
    ippiMax_32f_C1R(inp.idx_ptr(), sizeof(float32),
		    insize, &ret);
    return ret;
  }

  // templates for ipp_indexmax
  
  template <>
  intg ipp_indexmax(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    ubyte m;
    ippiMaxIndx_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		       insize, &m, &retx, &rety);
    return (intg)retx;
  }

  template <>
  intg ipp_indexmax(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    uint16 m;
    ippiMaxIndx_16u_C1R(inp.idx_ptr(), sizeof(uint16),
			insize, &m, &retx, &rety);
    return (intg)retx;
  }

  template <>
  intg ipp_indexmax(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    int16 m;
    ippiMaxIndx_16s_C1R(inp.idx_ptr(), sizeof(int16),
			insize, &m, &retx, &rety);
    return (intg)retx;
  }

  template <>
  intg ipp_indexmax(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    float32 m;
    ippiMaxIndx_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &m, &retx, &rety);
    return (intg)retx;
  }

  // templates for ipp_min

  template <>
  ubyte ipp_min(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    ubyte ret;
    ippiMin_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		   insize, &ret);
    return ret;
  }

  template <>
  uint16 ipp_min(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    uint16 ret;
    ippiMin_16u_C1R(inp.idx_ptr(), sizeof(uint16),
		    insize, &ret);
    return ret;
  }

  template <>
  int16 ipp_min(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    int16 ret;
    ippiMin_16s_C1R(inp.idx_ptr(), sizeof(int16),
		   insize, &ret);
    return ret;
  }

  template <>
  float32 ipp_min(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.height = inp.nelements();
    insize.width = 1;
    float32 ret;
    ippiMin_32f_C1R(inp.idx_ptr(), sizeof(float32),
		    insize, &ret);
    return ret;
  }

  // templates for ipp_indexmin
  
  template <>
  intg ipp_indexmin(const idx<ubyte> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    ubyte m;
    ippiMinIndx_8u_C1R(inp.idx_ptr(), sizeof(ubyte),
		       insize, &m, &retx, &rety);
    return (intg)retx;
  }

  template <>
  intg ipp_indexmin(const idx<uint16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    uint16 m;
    ippiMinIndx_16u_C1R(inp.idx_ptr(), sizeof(uint16),
			insize, &m, &retx, &rety);
    return (intg)retx;
  }

  template <>
  intg ipp_indexmin(const idx<int16> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    int16 m;
    ippiMinIndx_16s_C1R(inp.idx_ptr(), sizeof(int16),
			insize, &m, &retx, &rety);
    return (intg)retx;
  }

  template <>
  intg ipp_indexmin(const idx<float32> &inp) {
    ipp_checks1(inp);
    IppiSize insize;
    insize.width = inp.nelements();
    insize.height = 1;
    int retx, rety;
    float32 m;
    ippiMinIndx_32f_C1R(inp.idx_ptr(), sizeof(float32),
			insize, &m, &retx, &rety);
    return (intg)retx;
  }

  // templates for ipp_maxevery

  template <>
  void ipp_maxevery(const idx<ubyte> &in1, idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMaxEvery_8u_C1IR(in1.idx_ptr(), sizeof(ubyte),
			 in2.idx_ptr(), sizeof(ubyte),
			 insize);
  }

  template <>
  void ipp_maxevery(const idx<uint16> &in1, idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMaxEvery_16u_C1IR(in1.idx_ptr(), sizeof(uint16),
			  in2.idx_ptr(), sizeof(uint16),
			  insize);
  }

  template <>
  void ipp_maxevery(const idx<int16> &in1, idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMaxEvery_16s_C1IR(in1.idx_ptr(), sizeof(int16),
			  in2.idx_ptr(), sizeof(int16),
			  insize);
  }

  template <>
  void ipp_maxevery(const idx<float32> &in1, idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.height = in1.nelements();
    insize.width = 1;
    ippiMaxEvery_32f_C1IR(in1.idx_ptr(), sizeof(float32),
			  in2.idx_ptr(), sizeof(float32),
			  insize);
  }

  // templates for ipp_sqrdist

  template <>
  float64 ipp_sqrdist(const idx<ubyte> &in1, const idx<ubyte> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.width = in1.nelements();
    insize.height = 1;
    float64 ret;
    ippiNormDiff_L2_8u_C1R(in1.idx_ptr(), sizeof(ubyte),
			   in2.idx_ptr(), sizeof(ubyte),
			   insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sqrdist(const idx<uint16> &in1, const idx<uint16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.width = in1.nelements();
    insize.height = 1;
    float64 ret;
    ippiNormDiff_L2_16u_C1R(in1.idx_ptr(), sizeof(uint16),
			    in2.idx_ptr(), sizeof(uint16),
			    insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sqrdist(const idx<int16> &in1, const idx<int16> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.width = in1.nelements();
    insize.height = 1;
    float64 ret;
    ippiNormDiff_L2_16s_C1R(in1.idx_ptr(), sizeof(int16),
			    in2.idx_ptr(), sizeof(int16),
			    insize, &ret);
    return ret;
  }

  template <>
  float64 ipp_sqrdist(const idx<float32> &in1, const idx<float32> &in2) {
    ipp_checks2(in1, in2);
    IppiSize insize;
    insize.width = in1.nelements();
    insize.height = 1;
    float64 ret;
#ifdef __IPPFAST__
    ippiNormDiff_L2_32f_C1R(in1.idx_ptr(), sizeof(float32),
			    in2.idx_ptr(), sizeof(float32),
			    insize, &ret, ippAlgHintFast);
#else
#ifndef __IPPACC__
#error either __IPPFAST__ or __IPPACC__ must be defined
#endif
    ippiNormDiff_L2_32f_C1R(in1.idx_ptr(), sizeof(float32),
			    in2.idx_ptr(), sizeof(float32),
			    insize, &ret, ippAlgHintAccurate);
#endif
    return ret;
  }

  // templates for ipp_threshold_lt (in-place)

  template <>
  void ipp_threshold_lt(idx<ubyte> &in, ubyte th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_8u_C1IR(in.idx_ptr(), sizeof(ubyte),
			     insize, th);
  }

  template <>
  void ipp_threshold_lt(idx<uint16> &in, uint16 th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_16u_C1IR(in.idx_ptr(), sizeof(uint16),
			      insize, th);
  }

  template <>
  void ipp_threshold_lt(idx<int16> &in, int16 th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_16s_C1IR(in.idx_ptr(), sizeof(int16),
			      insize, th);
  }

  template <>
  void ipp_threshold_lt(idx<float32> &in, float32 th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_32f_C1IR(in.idx_ptr(), sizeof(float32),
			      insize, th);
  }

  // templates for ipp_threshold_lt (not-in-place)

  template <>
  void ipp_threshold_lt(const idx<ubyte> &in, ubyte th, idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_8u_C1R(in.idx_ptr(), sizeof(ubyte),
			    out.idx_ptr(), sizeof(ubyte),
			    insize, th);
  }

  template <>
  void ipp_threshold_lt(const idx<uint16> &in, uint16 th, idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_16u_C1R(in.idx_ptr(), sizeof(uint16),
			     out.idx_ptr(), sizeof(uint16),
			     insize, th);
  }

  template <>
  void ipp_threshold_lt(const idx<int16> &in, int16 th, idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_16s_C1R(in.idx_ptr(), sizeof(int16),
			     out.idx_ptr(), sizeof(int16),
			     insize, th);
  }

  template <>
  void ipp_threshold_lt(const idx<float32> &in, float32 th,
			idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LT_32f_C1R(in.idx_ptr(), sizeof(float32),
			     out.idx_ptr(), sizeof(float32),
			     insize, th);
  }

  // templates for ipp_threshold_gt (in-place)

  template <>
  void ipp_threshold_gt(idx<ubyte> &in, ubyte th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_8u_C1IR(in.idx_ptr(), sizeof(ubyte),
			     insize, th);
  }

  template <>
  void ipp_threshold_gt(idx<uint16> &in, uint16 th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_16u_C1IR(in.idx_ptr(), sizeof(uint16),
			      insize, th);
  }

  template <>
  void ipp_threshold_gt(idx<int16> &in, int16 th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_16s_C1IR(in.idx_ptr(), sizeof(int16),
			      insize, th);
  }

  template <>
  void ipp_threshold_gt(idx<float32> &in, float32 th) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_32f_C1IR(in.idx_ptr(), sizeof(float32),
			      insize, th);
  }

  // templates for ipp_threshold_gt (not-in-place)

  template <>
  void ipp_threshold_gt(const idx<ubyte> &in, ubyte th, idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_8u_C1R(in.idx_ptr(), sizeof(ubyte),
			    out.idx_ptr(), sizeof(ubyte),
			    insize, th);
  }

  template <>
  void ipp_threshold_gt(const idx<uint16> &in, uint16 th, idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_16u_C1R(in.idx_ptr(), sizeof(uint16),
			     out.idx_ptr(), sizeof(uint16),
			     insize, th);
  }

  template <>
  void ipp_threshold_gt(const idx<int16> &in, int16 th, idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_16s_C1R(in.idx_ptr(), sizeof(int16),
			     out.idx_ptr(), sizeof(int16),
			     insize, th);
  }

  template <>
  void ipp_threshold_gt(const idx<float32> &in, float32 th,
			idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GT_32f_C1R(in.idx_ptr(), sizeof(float32),
			     out.idx_ptr(), sizeof(float32),
			     insize, th);
  }

  // templates for ipp_threshold_lt (with value, in-place)

  template <>
  void ipp_threshold_lt(idx<ubyte> &in, ubyte th, ubyte value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_8u_C1IR(in.idx_ptr(), sizeof(ubyte),
				insize, th, value);
  }

  template <>
  void ipp_threshold_lt(idx<uint16> &in, uint16 th, uint16 value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_16u_C1IR(in.idx_ptr(), sizeof(uint16),
				 insize, th, value);
  }

  template <>
  void ipp_threshold_lt(idx<int16> &in, int16 th, int16 value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_16s_C1IR(in.idx_ptr(), sizeof(int16),
				 insize, th, value);
  }

  template <>
  void ipp_threshold_lt(idx<float32> &in, float32 th, float32 value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_32f_C1IR(in.idx_ptr(), sizeof(float32),
				 insize, th, value);
  }

  // templates for ipp_threshold_lt (with value, not-in-place)

  template <>
  void ipp_threshold_lt(const idx<ubyte> &in, ubyte th, ubyte value,
			idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_8u_C1R(in.idx_ptr(), sizeof(ubyte),
			       out.idx_ptr(), sizeof(ubyte),
			       insize, th, value);
  }

  template <>
  void ipp_threshold_lt(const idx<uint16> &in, uint16 th, uint16 value,
			idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_16u_C1R(in.idx_ptr(), sizeof(uint16),
				out.idx_ptr(), sizeof(uint16),
				insize, th, value);
  }

  template <>
  void ipp_threshold_lt(const idx<int16> &in, int16 th, int16 value,
			idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_16s_C1R(in.idx_ptr(), sizeof(int16),
				out.idx_ptr(), sizeof(int16),
				insize, th, value);
  }

  template <>
  void ipp_threshold_lt(const idx<float32> &in, float32 th,
			float32 value, idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_LTVal_32f_C1R(in.idx_ptr(), sizeof(float32),
				out.idx_ptr(), sizeof(float32),
				insize, th, value);
  }

  // templates for ipp_threshold_gt (with value, in-place)

  template <>
  void ipp_threshold_gt(idx<ubyte> &in, ubyte th, ubyte value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_8u_C1IR(in.idx_ptr(), sizeof(ubyte),
				insize, th, value);
  }

  template <>
  void ipp_threshold_gt(idx<uint16> &in, uint16 th, uint16 value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_16u_C1IR(in.idx_ptr(), sizeof(uint16),
				 insize, th, value);
  }

  template <>
  void ipp_threshold_gt(idx<int16> &in, int16 th, int16 value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_16s_C1IR(in.idx_ptr(), sizeof(int16),
				 insize, th, value);
  }

  template <>
  void ipp_threshold_gt(idx<float32> &in, float32 th, float32 value) {
    ipp_checks1(in);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_32f_C1IR(in.idx_ptr(), sizeof(float32),
				 insize, th, value);
  }

  // templates for ipp_threshold_gt (with value, not-in-place)

  template <>
  void ipp_threshold_gt(const idx<ubyte> &in, ubyte th, ubyte value,
			idx<ubyte> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_8u_C1R(in.idx_ptr(), sizeof(ubyte),
			       out.idx_ptr(), sizeof(ubyte),
			       insize, th, value);
  }

  template <>
  void ipp_threshold_gt(const idx<uint16> &in, uint16 th, uint16 value,
			idx<uint16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_16u_C1R(in.idx_ptr(), sizeof(uint16),
				out.idx_ptr(), sizeof(uint16),
				insize, th, value);
  }

  template <>
  void ipp_threshold_gt(const idx<int16> &in, int16 th, int16 value,
			idx<int16> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_16s_C1R(in.idx_ptr(), sizeof(int16),
				out.idx_ptr(), sizeof(int16),
				insize, th, value);
  }

  template <>
  void ipp_threshold_gt(const idx<float32> &in, float32 th,
			float32 value, idx<float32> &out) {
    ipp_checks2(in, out);
    IppiSize insize;
    insize.width = in.nelements();
    insize.height = 1;
    ippiThreshold_GTVal_32f_C1R(in.idx_ptr(), sizeof(float32),
				out.idx_ptr(), sizeof(float32),
				insize, th, value);
  }

#endif /* __IPP__ */

} /* ebl namespace */

#endif /* IPPOPS_HPP_ */
