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
 *****************************************************************************/

#ifndef LIBEBLEARN_H
#define LIBEBLEARN_H

#include "bbox.h"
#include "detector.h"
#include "ebl_answer.h"
#include "ebl_arch.h"
#include "ebl_basic.h"
#include "ebl_defines.h"
#include "ebl_ebm.h"
#include "ebl_energy.h"
#include "ebl_layers.h"
#include "ebl_logger.h"
#include "ebl_lua.h"
#include "ebl_machines.h"
#include "ebl_march.h"
#include "ebl_merge.h"
#include "ebl_module.h"
#include "ebl_nonlinearity.h"
#include "ebl_normalization.h"
#include "ebl_parameters.h"
#include "ebl_pooling.h"
#include "ebl_preprocessing.h"
#include "ebl_state.h"
#include "ebl_utils.h"
#include "libidx.h"

#ifndef __NOSTL__
#include "ebl_tester.h"
#include "ebl_trainer.h"
#include "datasource.h"
#endif

#ifdef __CUDA__
#include "ebl_cudaops.h"
#include "ebl_cudabasic.h"
#include "ebl_cudapooling.h"
#include "ebl_cudanonlinearity.h"
#include "ebl_cudanormalization.h"
#include "ebl_cudamerge.h"
#include "ebl_cudautils.h"
#endif

#endif // LIBEBLEARN_H
