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

// tell header that we are in the libidx scope
#define LIBIDX

#include <stdio.h>

#ifndef __WINDOWS__
#include <inttypes.h>
#endif

#include "imageIO.h"
#include "idxops.h"
#include "idx.h"
#include "stl.h"
#include "utils.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////

  //! skip comments lines starting with <start>
  void skip_comments(ubyte start, FILE *fp) {
    int c;
    for (;;) {
      c = getc(fp);
      while (c==' ' || c=='\n' || c=='\t' || c=='\r') c = getc(fp);
      if (c != start) break;
      while (c != '\n')  c = getc(fp);
    }
    ungetc(c,fp);
  }

  idxdim read_pnm_header(FILE *fp, int &type, int &vmax) {
    // read pnm type
    type = -1;
    char *s = fscan_str(fp);
    if (!strcmp("P1", s) || !strcmp("P2", s) || !strcmp("P3", s) ||
	!strcmp("P4", s) || !strcmp("P5", s) || !strcmp("P6", s))
      type = (int) s[1] - '0';
    else {
      std::string e;
      e << "invalid binary PNM file type: " << s;
      delete[] s;
      eblthrow(e);
    }
    if (s) delete[] s;
    skip_comments(35, fp);
    // read dimensions
    int ncol = fscan_int(fp), nlin = fscan_int(fp);
    vmax = fscan_int(fp);
    getc(fp);
    idxdim dims(nlin, ncol, 3);
    return dims;
  }

  idx<ubyte> pnm_read(FILE *fp, idx<ubyte> *out_) {
    int type, vmax = 0;
    size_t expected_size, read_size;
    idxdim dims = read_pnm_header(fp, type, vmax);
    idx<ubyte> out;
    idx<ubyte> *pout = &out;
    if (!out_) // allocate buffer if out is empty
      out = idx<ubyte>(dims);
    else
      pout = out_;
    idx_checkorder1(*pout, 3); // allow only 3D buffers
    // resize out if necessary
    if (pout->get_idxdim() != dims)
      pout->resize(dims);
    // sizes
    size_t sz = (vmax == 65535) ? 2 : 1;
    expected_size = dims.nelements();

    switch (type) {
    // case 3: // PPM ASCII
    //   uint val;
    //   { idx_aloop1(o, *pout, ubyte) {
    // 	in >> val;
    // 	// downcasting automatically scales values if vmax > 255
    // 	*o = (unsigned char) val;
    // 	}}
    //   break ;
    case 6: // PPM binary
      if (pout->contiguousp()) {
	if (sz == 2) { // 16 bits per pixel
	  idx<short> out2(dims);
	  read_size = fread((char *) out2.idx_ptr(), 1, sz * expected_size, fp);
	  idx_copy(out2, *pout);
	} else { // 8 bits per pixel
	  read_size = fread((char *) pout->idx_ptr(), 1, sz * expected_size,fp);
	}
	read_size /= sz;
	if (expected_size != read_size) {
	  eblthrow("image read: not enough items read. expected "
		   << (int) expected_size << " but found " << (int) read_size);
	}
      } else {
	{ idx_bloop1(ou, *pout, ubyte) {
	    { idx_bloop1(o, ou, ubyte) {
	    o.set(getc(fp));
	      }}}}
      }
      break ;
      // TODO: implement pnm formats
      /*  case 4: // PBM image
	  ((= head "P4")
	  (let* ((ncol (fscan-int f))
	  (nlin (fscan-int f))
	  (n 0) (c 0))
	  ((-int-) ncol nlin n c)
	  (when (or (<> ncol ncolo) (<> nlin nlino) (< ncmpo 3))
	  (idx-u3resize out nlin ncol (max ncmpo 3)))
	  (getc f)
	  (cinline-idx2loop out "unsigned char" "p" "i" "j"
	  #{{ unsigned char v;
	  if ((j == 0) || ($n == 0)) { $c = getc((FILE *)$f); $n=0; }
	  v = ($c & 128) ? 0 : 255 ;
	  p[0]= p[1]= p[2]= v ;
	  $n = ($n == 7) ? 0 : $n+1 ;
	  $c = $c << 1;
	  } #} )))
	  // PGM image
	  ((= head "P5")
	  (let* ((ncol (fscan-int f))
	  (nlin (fscan-int f)))
	  ((-int-) ncol nlin)
	  (when (or (<> ncol ncolo) (<> nlin nlino) (< ncmpo 3))
	  (idx-u3resize out nlin ncol (max ncmpo 3)))
	  (fscan-int f)
	  (getc f)
	  (cinline-idx2loop out "unsigned char" "p" "i" "j"
	  #{ p[0]=p[1]=p[2]=getc((FILE *)$f) #}
	  )))
      */
    default:
      eblwarn( "Format P" << type << " not implemented." << std::endl);
    }
    return *pout;
  }

} // end namespace ebl
