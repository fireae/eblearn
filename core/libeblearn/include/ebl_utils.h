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

#ifndef EBL_UTILS_H_
#define EBL_UTILS_H_

#include "ebl_defines.h"
#include "libidx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Table functions

  //! Creates a table of full connections between indices [a0 .. a - 1]
  //! and [b0 .. b - 1].
  EXPORT idx<intg> full_table(intg a, intg b, intg a0 = 0, intg b0 = 0);
  //! Creates a table of that connects indices [0 .. n - 1] to
  //! [0 .. n - 1] in a 1 to 1 mapping, e.g. 0 -> 0, 1 -> 1, ..., n - 1 ->
  //! n - 1.
  EXPORT idx<intg> one2one_table(intg n);
  //! Return the concatenation of two connection tables.
  EXPORT idx<intg> concat_tables(idx<intg> &t0, idx<intg> &t1);
  //! Return a random table from [a0 .. a - 1] to [b0 .. b - 1] with
  //! each output being connected to 'fanin' inputs.
  EXPORT idx<intg> random_table(intg a, intg b, std::vector<intg> &fanin,
				intg a0 = 0, intg b0 = 0);
  //! Create a table for a first convolution layer on yuv input,
  //! fully and independently connecting layer y (0) to
  //! [0 .. yend - 1], u (1) to [yend .. uend - 1] and v(2) to
  //! [uend .. vend - 1].
  EXPORT idx<intg> yuv_table0(intg yend, intg uend, intg vend, intg yuvend,
			     intg uvend, intg &maxend);
  //! Create a table for a first convolution layer on uv input,
  //! fully and independently connecting  u (1) to [0 .. uend - 1] and v(2) to
  //! [uend .. vend - 1].
  EXPORT idx<intg> uv_table0(intg uend, intg vend);
  //! Create a table for a second convolution layer on the output
  //! of a first convolution on yuv input, randomly connecting
  //! [0 .. yend - 1] to [0 .. p0 - 1] with fanin of size 'fanin_y',
  //! [0 .. vend - 1] to [p0 .. p1 - 1] with  fanin of size 'fanin_yuv', and
  //! [uend .. vend - 1] to [p1 .. p2 - 1] with  fanin of size 'fanin_uv',
  EXPORT idx<intg> yuv_table1(intg yend, intg uend, intg vend,
			      intg p0, intg p1, intg p2, intg fanin_y,
			      intg fanin_yuv, intg fanin_uv);
  //! Create a table for stereo pair, with a full table from the first
  //! channel to [0 .. lend-1], from the 2nd channel to [lend .. rend-1]
  //! and finally a fully connected table from both channels to
  //! [rend .. lrend].
  EXPORT idx<intg> stereo_table(intg lend, intg rend, intg lrend);
  //! Create a table for a temporal triplet, with a full table from the first
  //! channel to [0 .. end1-1], from the 2nd channel to [end1 .. end2-1],
  //! the 3rd to [end2 .. end3-1], from channels 1 & 2 to [end3 .. end12-1],
  //! from chans 2 & 3 to [end12 .. end23-1], from chans 1, 2 & 3 to
  //! [end23 .. end123-1].
  EXPORT idx<intg> temporal3_table(intg end1, intg end2, intg end3,
				   intg end12, intg end23, intg end123,
				   intg &maxend);
  //! Create a table from a file with semi-colon separated csv values.
  //! for example, the contents can be 0,0; 0,1; 1,2; 3,1, 4,2; 1,5
  EXPORT idx<intg> text_table(std::string filename);

  //! Checks if duplicate inputs exist for each output and prints
  //! a warning for each duplicate.
  //! If duplicates are found, this returns true, false otherwise.
  EXPORT bool check_table_duplicates(idx<intg> &table);

} // namespace ebl {

#endif /* EBL_UTILS_H_ */
