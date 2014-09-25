/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala    *
 *   soumith@gmail.com                         *
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

#include "matlab.h"

#ifdef __MATLAB__
namespace ebl {
  
  matlab::matlab(const char* fname) : filename(fname),mat(NULL) {}

  matlab::~matlab() {
  }
  

  size_t* intg_array_to_size_t_array(const intg *in, intg order) {
    size_t *out = (size_t *) malloc(sizeof(size_t) * order);
    for (uint i=0; i < order; ++i) {
      if ( in[i] > INT_MAX || in[i] < 0) {
        eblerror ("MATIO supports dimensions within unsigned integer limits only :(");
      }
      else
        out[i] = in[i];
    }
    return out;
  }

#define CREATE_MATVAR_MACRO(T1, T2, T3)                                 \
  template <>                                                           \
  matvar_t* matlab::create_matvar(idx<T1> in, char* name) {             \
    size_t * intdims = intg_array_to_size_t_array(in.dims(), in.order()); \
    matvar_t* retval = Mat_VarCreate(name, T2, T3,            \
                                   in.order(), intdims, in.idx_ptr(), 0); \
    free(intdims);                                                      \
    return retval;                                                      \
  }

  CREATE_MATVAR_MACRO(ubyte,  MAT_C_UINT8, MAT_T_UINT8)
  CREATE_MATVAR_MACRO(uint16, MAT_C_UINT16, MAT_T_UINT16)
  CREATE_MATVAR_MACRO(uint32, MAT_C_UINT32, MAT_T_UINT32)
  CREATE_MATVAR_MACRO(uint64, MAT_C_UINT64, MAT_T_UINT64)
  CREATE_MATVAR_MACRO(int16,  MAT_C_INT16, MAT_T_INT16)
  CREATE_MATVAR_MACRO(int32,  MAT_C_INT32, MAT_T_INT32)
  CREATE_MATVAR_MACRO(int64,  MAT_C_INT64, MAT_T_INT64)
  CREATE_MATVAR_MACRO(float,  MAT_C_SINGLE, MAT_T_SINGLE)
  CREATE_MATVAR_MACRO(double, MAT_C_DOUBLE, MAT_T_DOUBLE)

} //end of namespace ebl

#endif
