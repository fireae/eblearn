/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef EBL_DEFINES_H
#define EBL_DEFINES_H

#include "defines.h"

#define err_not_implemented() {						\
    eblerror("member function not implemented for this class"); }

//! see numerics.h for description
extern bool drand_ini;

#define check_drand_ini() {					      \
    if (!drand_ini) printf("You have not initialized random sequence. \
Please call init_drand(time(NULL)) before using this function !\n"); }

#ifdef __DEBUG__
// in debug mode, check that backward tensor has the right size
#define DEBUG_CHECK_DX(in) {                                             \
    if (!in.allocated_dx())                                              \
      eblerror("state::b should have the same size as state::f in " << in); \
  }
#else
#define DEBUG_CHECK_DX(in)
#endif

#ifdef __DEBUG__
// in debug mode, check that backward tensor has the right size
#define DEBUG_CHECK_DDX(in) {                                            \
    if (!in.allocated_ddx())                                             \
      eblerror("state::b should have the same size as state::f in " << in); \
  }
#else
#define DEBUG_CHECK_DDX(in)
#endif

#endif /* EBL_DEFINES */
