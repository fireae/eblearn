/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet *
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

#ifndef IDXIO_HPP_
#define IDXIO_HPP_

#include "idxops.h"
#include "string_utils.h"

namespace ebl {

////////////////////////////////////////////////////////////////
// helper functions

// template functions returning the magic number associated with a
// particular type
template <class T> inline int get_magic() {
  eblerror("matrix type not implemented."); return 0; }
template <> inline int get_magic<ubyte>()       {return MAGIC_BYTE_MATRIX; }
template <> inline int get_magic<int>()         {return MAGIC_INTEGER_MATRIX;}
template <> inline int get_magic<float>()       {return MAGIC_FLOAT_MATRIX; }
template <> inline int get_magic<double>()      {return MAGIC_DOUBLE_MATRIX; }
template <> inline int get_magic<long>()        {return MAGIC_LONG_MATRIX; }
template <> inline int get_magic<uint>()        {return MAGIC_UINT_MATRIX; }
template <> inline int get_magic<uint64>()      {return MAGIC_UINT64_MATRIX; }

// Pascal Vincent type
template <class T> inline int get_magic_vincent() {
  eblerror("matrix type not implemented"); return 0; }
template <> inline int get_magic_vincent<ubyte>(){return MAGIC_UBYTE_VINCENT;}
template <> inline int get_magic_vincent<int>()  {return MAGIC_INT_VINCENT;}
template <> inline int get_magic_vincent<float>(){return MAGIC_FLOAT_VINCENT;}
template <> inline int get_magic_vincent<double>() {
  return MAGIC_DOUBLE_VINCENT; }
template <> inline int get_magic_vincent<long>() {return 0x0000; }

// type to string function for debug message.
inline std::string get_magic_str(int magic) {
  switch (magic) {
    // standard format
    case MAGIC_BYTE_MATRIX: 	return "ubyte";
    case MAGIC_PACKED_MATRIX:	return "packed";
    case MAGIC_SHORT_MATRIX: 	return "short";
    case MAGIC_SHORT8_MATRIX: 	return "short8";
    case MAGIC_ASCII_MATRIX: 	return "ascii";
    case MAGIC_INTEGER_MATRIX: 	return "int";
    case MAGIC_FLOAT_MATRIX: 	return "float";
    case MAGIC_DOUBLE_MATRIX: 	return "double";
    case MAGIC_LONG_MATRIX:	return "long";
      // non standard
    case MAGIC_UINT_MATRIX: 	return "uint";
    case MAGIC_UINT64_MATRIX: 	return "uint64";
    case MAGIC_INT64_MATRIX: 	return "int64";
      // pascal vincent format
    case MAGIC_BYTE_VINCENT: 	return "byte (pascal vincent)";
    case MAGIC_UBYTE_VINCENT: 	return "ubyte (pascal vincent)";
    case MAGIC_SHORT_VINCENT: 	return "short (pascal vincent)";
    case MAGIC_INT_VINCENT: 	return "int (pascal vincent)";
    case MAGIC_FLOAT_VINCENT: 	return "float (pascal vincent)";
    case MAGIC_DOUBLE_VINCENT: 	return "double (pascal vincent)";
    default:
      std::string s;
      s << "unknown type (magic: " << reinterpret_cast<void*>(magic) << ")";
      return s;
  }
}

/*! Reverses order of bytes in <n> items of size <sz>
  starting at memory location <ptr>
  This is a tool for writing/reading file formats that are portable
  across systems with processors that represent long-words
  differently in memory (Sparc vs Intel-Pentium for ex.)
  It can be called from inline-C as C_reverse8(ptr,n);

  ptr: pointer to the block of memory that must be reversed.
  n: number of elements to reverse. */
template <class T> inline void reverse_n(T *ptr, int n) {
  char *mptr = (char *) ptr;
  while(n--)
  {
    char tmp;
    char *uptr = mptr + sizeof (T);
    if (sizeof (T) >= 2)
    { tmp = mptr[0]; mptr[0]=uptr[-1]; uptr[-1]=tmp; }
    if (sizeof (T) >= 4)
    { tmp = mptr[1]; mptr[1]=uptr[-2]; uptr[-2]=tmp; }
    if (sizeof (T) >= 6)
    { tmp = mptr[2]; mptr[2]=uptr[-3]; uptr[-3]=tmp; }
    if (sizeof (T) >= 8)
    { tmp = mptr[3]; mptr[3]=uptr[-4]; uptr[-4]=tmp; }
    mptr = uptr;
  }
}

template <class T> inline T endian(T ptr) {
  T v = ptr;
  // endianess test
  int endiantest = 1;
  if (*(char*)&endiantest) reverse_n(&v, 1);
  return v;
}

template <typename T, typename T2>
void read_cast_matrix(FILE *fp, idx<T2> &out) {
  idx<T> m(out.get_idxdim());
  read_matrix_body(fp, m);
  idx_copy(m, out);
}

template <typename T>
void read_matrix_body(FILE *fp, idx<T> &m) {
  size_t read_count;
  idx_aloop1(i, m, T) {
    read_count = fread(&(*i), sizeof (T), 1, fp);
    if (read_count != 1)
      eblerror("Read incorrect number of bytes ");
  }
}

// loading ///////////////////////////////////////////////////////////////////

template <typename T>
idx<T> load_matrix(const char *filename) {
  // open file
  FILE *fp = fopen(filename, "rb");
  if (!fp)
    eblthrow("load_matrix failed to open " << filename);
  // read it
  idx<T> m;
  try {
    m = load_matrix<T>(fp);
    fclose(fp);
  } catch(eblexception &e) {
    eblthrow(" while loading " << filename)
        }
  return m;
}

template <typename T>
idx<T> load_matrix(const std::string &filename) {
  return load_matrix<T>(filename.c_str());
}

template <typename T>
idx<T> load_matrix(const std::vector<std::string> &files, intg cdim) {
  if (files.size() == 0) eblerror("expected at least 1 file to load");
  idx<T> w = load_matrix<T>(files[0]);
  for (uint i = 1; i < files.size(); ++i) {
    idx<T> tmp = load_matrix<T>(files[i]);
    w = idx_concat(w, tmp, cdim);
  }
  return w;
}

template <typename T>
void load_matrix(idx<T>& m, const std::string &filename) {
  load_matrix(m, filename.c_str());
}

template <typename T>
void load_matrix(idx<T>& m, const char *filename) {
  // open file
  FILE *fp = fopen(filename, "rb");
  if (!fp)
    eblthrow("load_matrix failed to open " << filename);
  // read it
  m = load_matrix<T>(fp, &m);
  fclose(fp);
}

template <typename T>
idx<T> load_matrix(FILE *fp, idx<T> *out_) {
  int magic;
  idxdim dims = read_matrix_header(fp, magic);
  idx<T> out;
  idx<T> *pout = &out;
  if (!out_) // if no input matrix, allocate new one
    out = idx<T>(dims);
  else // otherwise use given one
    pout = out_;

  // resize out if necessary
  if (pout->get_idxdim() != dims) { // different order/dimensions
    // if order is different, it's from the input matrix, error
    if (pout->order() != dims.order())
      eblerror("error: different orders: " << *pout << " " << dims);
    // resize output idx
    pout->resize(dims);
  }
  //! if out matrix is same type as current, read directly
  if ((magic == get_magic<T>()) || (magic == get_magic_vincent<T>())) {
    // read
    read_matrix_body(fp, *pout);
  } else { // different type, read original type, then copy/cast into out
    switch (magic) {
      case MAGIC_BYTE_MATRIX:
      case MAGIC_UBYTE_VINCENT:
	read_cast_matrix<ubyte>(fp, *pout);
	break ;
      case MAGIC_INTEGER_MATRIX:
      case MAGIC_INT_VINCENT:
	read_cast_matrix<int>(fp, *pout);
	break ;
      case MAGIC_FLOAT_MATRIX:
      case MAGIC_FLOAT_VINCENT:
	read_cast_matrix<float>(fp, *pout);
	break ;
      case MAGIC_DOUBLE_MATRIX:
      case MAGIC_DOUBLE_VINCENT:
	read_cast_matrix<double>(fp, *pout);
	break ;
      case MAGIC_LONG_MATRIX:
	read_cast_matrix<long>(fp, *pout);
	break ;
      case MAGIC_UINT_MATRIX:
	read_cast_matrix<uint>(fp, *pout);
	break ;
      case MAGIC_UINT64_MATRIX:
	read_cast_matrix<uint64>(fp, *pout);
	break ;
      case MAGIC_INT64_MATRIX:
	read_cast_matrix<int64>(fp, *pout);
	break ;
      default:
	eblerror("unknown magic number");
    }
  }
  return *pout;
}

template <typename T>
midx<T> load_matrices(const std::string &filename, bool ondemand){
  if (!has_multiple_matrices(filename.c_str()))
    eblthrow("trying to load multiple matrices from single-matrix file "
             << filename);
  // open file
  std::file *f = new std::file(filename.c_str(), "rb");
  FILE *fp = f->get_fp();
  // first read offsets matrix
  idx<int64> p = load_matrix<int64>(fp);
  // switch loading mode
  if (ondemand) { // do not load data now
    fseek(fp, 0, SEEK_SET); // reset fp to beginning
    midx<T> all(p.get_idxdim(), f, &p);
    return all;
  } else { // load all data now
    eblprint( "Loading the whole dataset into memory" <<std::endl);
    // read all present matrices
    midx<T> all(p.dim(0));
    for (uint i = 0; i < p.dim(0); ++i) {
      if (p.get(i) > 0) { // matrix is present, get it
        if (fseek(fp, p.get(i), SEEK_SET)) {
          fseek(fp, 0, SEEK_END);
          fpos_t fppos;
          fgetpos(fp, &fppos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
          eblerror("fseek to position " << p.get(i) << " failed, "
                   << "file is " << (intg) fppos << " big");
#else
          eblerror("fseek to position " << p.get(i) << " failed, "
                   << "file is " << (intg) fppos.__pos << " big");
#endif
        }
        // move fp to matrix beginning
        idx<T> m = load_matrix<T>(fp);
        all.mset(m, i);
      }
    }
    delete f;
    return all;
  }
}

template <typename T>
idx<T> load_csv_matrix(const char *filename, bool ignore_first_line,
		       bool ignore_missing_value_lines) {
  char sep = ',';
  T default_value = 0;
  // open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) eblthrow("load_csv_matrix failed to open " << filename);
  // read it
  idx<T> m;
  char *ret = NULL, buf[4096];
  intg line = 0;
  intg n = 1;
  intg i = 0, j = 0;
  // figure out geometry
  try {
    while ((ret = fgets(buf, 4096, fp))) {
      if (line == 0 && ignore_first_line) {
	line++;
	continue ;
      }
      if (line == 1) {
	std::string s = buf;
	size_t pos = 0;
	size_t len = s.size();
	while (pos < len && ((pos = s.find(sep, pos)) != std::string::npos)) {
	  n++;
	  pos++;
	}
      }
      line++;
      i++;
    }
    EDEBUG("found a " << i << "x" << n << " matrix");
    m = idx<T>(i, n);
    idx_clear(m);
    fseek(fp, 0, SEEK_SET); // reset fp to beginning
    i = 0;
    line = 0;
    while ((ret = fgets(buf, 4096, fp))) {
      if (line == 0 && ignore_first_line) {
	line++;
	continue ;
      }
      std::string s = buf, tmp;
      size_t pos0 = 0;
      size_t pos1 = 0;
      size_t len = s.size();
      bool missing = false;
      j = 0;
      while (pos0 < len && ((pos1 = s.find(sep, pos0)) != std::string::npos)) {
	if (pos1 - pos0 == 0) {
	  eblwarn("no value at position " << i << ", " << j << ", setting to "
		  << default_value);
	  m.set(default_value, i, j);
	  missing = true;
	} else {
	  tmp = s.substr(pos0, pos1-pos0);
	  EDEBUG("string to double from: " << tmp);
	  try {
	    m.set((T) string_to_double(tmp), i, j);
	  } eblcatchwarn_extra(missing = true;);
	}
	j++;
	pos0 = pos1 + 1;
      }
      if (len - pos0 - 1 == 0) {
	eblwarn("no value at position " << i << ", " << j << ", setting to "
		<< default_value);
	m.set(default_value, i, j);
	missing = true;
      } else {
	tmp = s.substr(pos0, len-pos0);
	EDEBUG("string to double from: " << tmp);
	try {
	  m.set((T) string_to_double(tmp), i, j);
	} eblcatchwarn_extra(missing = true;);
      }
      idx<T> mtmp = m.select(0, i);
      EDEBUG("row: " << mtmp.str());
      if (n != m.dim(1))
	eblerror("expected " << m.dim(1) << " values but found " << n);
      line++;
      if (missing && ignore_missing_value_lines) {
	eblwarn("ignoring line " << i
		<< " because it contains a missing value");
	m = m.narrow(0, m.dim(0) - 1, 0);
      } else
	i++;
    }    
    fclose(fp);
  } catch(eblexception &e) { eblthrow(" while loading " << filename) }
  return m;
}

// saving //////////////////////////////////////////////////////////////////////

template <typename T>
bool save_matrix(idx<T>& m, const std::string &filename) {
  return save_matrix(m, filename.c_str());
}

template <typename T2, typename T>
bool save_matrix(idx<T>& m, const std::string &filename) {
  idx<T2> m2(m.get_idxdim());
  idx_copy(m, m2);
  return save_matrix(m2, filename.c_str());
}

// TODO: intg support
template <typename T> bool save_matrix(idx<T>& m, const char *filename) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  bool ret = save_matrix(m, fp);
  if (!ret)
    eblwarn( "failed to write matrix " << m << " to " << filename << "."
             << std::endl);
  fclose(fp);
  return ret;
}

// TODO: intg support
template <typename T> bool save_matrix(idx<T>& m, FILE *fp) {
  int v, i;
  // write header
  v = get_magic<T>();
  if (fwrite(&v, sizeof (int), 1, fp) != 1) return false;
  v = m.order();
  if (fwrite(&v, sizeof (int), 1, fp) != 1) return false;
  for (i = 0; (i < m.order()) || (i < 3); ++i) {
    if (i < m.order())
      v = m.dim(i);
    else
      v = 1;
    if (fwrite(&v, sizeof (int), 1, fp) != 1) return false;
  }
  // write body
  idx_aloop1(k, m, T)
    fwrite(&(*k), sizeof (T), 1, fp);
  return true;
}

template <typename T>
bool save_matrix(midx<T> m, const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "wb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  // determine matrices dims
  if (m.true_order() != 1) eblerror("expected order 1");
  m.remove_trailing_dims();
  if (m.dim(0) < 1) eblerror("expected at least 1 sample");
  idxdim alld(m.dim(0));
  idx<T> e = m.mget(0);
  if (e.order() != 3) eblerror("expected only 3 channels");
  idxdim d = e.get_idxdim();
  for (uint i = 0; i < d.order(); ++i)
    alld.insert_dim(alld.order(), d.dim(i));
  // allocate matrix
  idx<T> all(alld);
  idx_clear(all);
  // add all matrices
  for (long i = 0; i < m.dim(0); ++i) {
    idx<T> e = all.select(0,i);
    idx_copy(m.mget(i), e);
  }
  // save matrix
  return save_matrix(all, filename);
}

template <typename T>
bool save_matrices(svector<idx<T> >& v, const std::string &filename) {
  if (v.empty()) eblerror("cannot save an empty vector");
  midx<T> m(v.size());
  for (uint i = 0; i < v.size(); ++i) m.mset(v[i], i);
  return save_matrices(m, filename);
}

template <typename T>
bool save_matrices(midx<T>& m, const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "wb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  // allocate offset matrix
  idx<int64> offsets(m.get_idxdim());
  idx_clear(offsets);
  // save offsets matrix first
  bool ret = save_matrix(offsets, fp);
  if (!ret) {
    eblwarn( "failed to write matrix " << offsets << " to " << filename << "."
             << std::endl);
    fclose(fp);
    return false;
  }
  // then save all matrices contained in m
  fpos_t pos;
  // TODO: implement generic looping for midx
  if (m.order() == 1) {
    for (uint i = 0; i < m.dim(0); ++i) {
      if (m.exists(i)) {
        // save offset of matrix
        fgetpos(fp, &pos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
        offsets.set((int64) pos, i);
#else
        offsets.set((int64) pos.__pos, i);
#endif
        // save matrix to file
        idx<T> e = m.mget(i);
        ret = save_matrix(e, fp);
        if (!ret) {
          eblwarn( "failed to write matrix " << e << " to " << filename << "."
                   << std::endl);
          fclose(fp);
          return false;
        }
      }
    }
  } else if (m.order() == 2) {
    for (uint i = 0; i < m.dim(0); ++i) {
      for (uint j = 0; j < m.dim(1); ++j) {
        if (m.exists(i, j)) {
          // save offset of matrix
          fgetpos(fp, &pos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
          offsets.set((int64) pos, i, j);
#else
          offsets.set((int64) pos.__pos, i, j);
#endif
          // save matrix to file
          idx<T> e = m.mget(i, j);
          ret = save_matrix(e, fp);
          if (!ret) {
            eblwarn( "failed to write matrix " << e << " to "
                     << filename << "." << std::endl);
            fclose(fp);
            return false;
          }
        }
      }
    }
  }
  fclose(fp);
  // finally rewrite offset matrix at beginning of file
  fp = fopen(filename.c_str(), "rb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  ret = save_matrix(offsets, fp);
  if (!ret) {
    eblwarn( "failed to write matrix " << offsets << " to "
             << filename << "." << std::endl);
    fclose(fp);
    return false;
  }
  fclose(fp);
  return ret;
}

template <typename T>
bool save_matrices_individually(midx<T>& m, const std::string &root,
                                bool print) {
  intg id = 0;
  // TODO: implement generic looping for midx
  if (m.order() == 1) {
    for (uint i = 0; i < m.dim(0); ++i) {
      if (m.exists(i)) {
        // save matrix to file
        std::string fname; fname << root << "_" << id++ << ".mat";
        idx<T> e = m.mget(i);
        if (!save_matrix(e, fname)) return false;
        if (print) eblprint( "saved " << fname << std::endl);
      }
    }
  } else if (m.order() == 2) {
    for (uint i = 0; i < m.dim(0); ++i) {
      for (uint j = 0; j < m.dim(1); ++j) {
        if (m.exists(i, j)) {
          // save matrix to file
          std::string fname; fname << root << "_" << id++ << ".mat";
          idx<T> e = m.mget(i, j);
          if (!save_matrix(e, fname)) return false;
          if (print) eblprint( "saved " << fname << std::endl);
        }
      }
    }
  }
  return true;
}

template <typename T>
bool save_matrices(midx<T>& m1, midx<T> &m2, const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "wb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  m1.remove_trailing_dims();
  m2.remove_trailing_dims();
  // target dimensions
  idxdim d1 = m1, d2 = m2, dd1 = m1, dd2 = m2;
  dd1.remove_dim(0); dd2.remove_dim(0);
  if (dd1 != dd2)
    eblerror("expected same dimensions but got " << dd1 << " and " << dd2);
  idxdim dall(d1);
  dall.setdim(0, d1.dim(0) + d2.dim(0));
  // allocate offset matrix
  idx<int64> offsets(dall);
  idx_clear(offsets);
  // save offsets matrix first
  bool ret = save_matrix(offsets, fp);
  if (!ret) {
    eblwarn( "failed to write matrix " << offsets << " to "
             << filename << "." << std::endl);
    fclose(fp);
    return false;
  }
  // then save all matrices contained in m
  fpos_t pos;
  // TODO: implement generic looping for midx
  midx<T> *m = &m1;
  if (dall.order() == 1) {
    for (uint i = 0, k = 0; i < dall.dim(0); ++i, ++k) {
      // switch to second matrix
      if (i == m1.dim(0)) {
        m = &m2;
        k = 0; // reset iterator
      }
      if (m->exists(k)) {
        // save offset of matrix
        fgetpos(fp, &pos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
        offsets.set((int64) pos, i);
#else
        offsets.set((int64) pos.__pos, i);
#endif
        // save matrix to file
        idx<T> e = m->mget(k);
        ret = save_matrix(e, fp);
        if (!ret) {
          eblwarn( "failed to write matrix " << e << " to " <<
                   filename << "." << std::endl);
          fclose(fp);
          return false;
        }
      }
    }
  } else if (m->order() == 2) {
    for (uint i = 0, k = 0; i < dall.dim(0); ++i, ++k) {
      // switch to second matrix
      if (i == m1.dim(0)) {
        m = &m2;
        k = 0; // reset iterator
      }
      for (uint j = 0; j < m->dim(1); ++j) {
        if (m->exists(k, j)) {
          // save offset of matrix
          fgetpos(fp, &pos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
          offsets.set((int64) pos, i, j);
#else
          offsets.set((int64) pos.__pos, i, j);
#endif
          // save matrix to file
          idx<T> e = m->mget(k, j);
          ret = save_matrix(e, fp);
          if (!ret) {
            eblwarn("failed to write matrix " << e << " to "
                    << filename << "." << std::endl);
            fclose(fp);
            return false;
          }
        }
      }
    }
  }
  fclose(fp);
  // finally rewrite offset matrix at beginning of file
  fp = fopen(filename.c_str(), "rb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  ret = save_matrix(offsets, fp);
  if (!ret) {
    eblwarn( "failed to write matrix " << offsets << " to " << filename << "."
             << std::endl);
    fclose(fp);
    return false;
  }
  fclose(fp);
  return ret;
}

template <typename T>
bool save_matrices(std::list<std::string> &filenames,
                   const std::string &filename) {
  if (filenames.size() == 0) eblerror("expected a non-empty list");
  FILE *fp = fopen(filename.c_str(), "wb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  // determine matrices dims
  idxdim alld(filenames.size());
  std::string s = filenames.front();
  bool multi = has_multiple_matrices(s.c_str());
  if (multi) {
    midx<T> e = load_matrices<T>(s);
    idxdim d = e.get_idxdim();
    for (uint i = 0; i < d.order(); ++i)
      alld.insert_dim(d.order(), d.dim(i));
  }
  // allocate offsets matrix
  idx<int64> offsets(alld);
  idx_clear(offsets);
  // put this matrix first in the file
  bool ret = save_matrix(offsets, fp);
  if (!ret) {
    eblwarn( "failed to write matrix " << offsets << " to " << filename << "."
             << std::endl);
    fclose(fp);
    return false;
  }
  // then save all matrices
  fpos_t pos;
  intg j = 0;
  for (std::list<std::string>::iterator i = filenames.begin();
       i != filenames.end(); ++i, ++j) {
    if (multi) { // each matrix is composed of multiple matrices
      // TODO: check if this is breakng in 32.bit, false);
      midx<T> e = load_matrices<T>(*i);
      for (uint k = 0; k < e.dim(0); ++k) {
        // save offset of matrix
        fgetpos(fp, &pos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
        offsets.set((int64) pos, j, k);
#else
        offsets.set((int64) pos.__pos, j, k);
#endif
        // save matrix into file
        idx<T> ee = e.mget(k);
        ret = save_matrix(ee, fp);
        if (!ret) {
          eblwarn( "failed to write matrix " << k << " to " << filename << "."
                   << std::endl);
          fclose(fp);
          return false;
        }
      }
    } else {
      idx<T> e = load_matrix<T>(*i);
      // save offset of matrix
      fgetpos(fp, &pos);
#if defined(__WINDOWS__) || defined(__MAC__) || defined(__ANDROID__)
      offsets.set((int64) pos, j);
#else
      offsets.set((int64) pos.__pos, j);
#endif
      // save matrix into file
      ret = save_matrix(e, fp);
      if (!ret) {
        eblwarn( "failed to write matrix " << e << " to " << filename << "."
                 << std::endl);
        fclose(fp);
        return false;
      }
    }
  }
  fclose(fp);
  // finally rewrite offset matrix at beginning of file
  fp = fopen(filename.c_str(), "rb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  ret = save_matrix(offsets, fp);
  if (!ret) {
    eblwarn( "failed to write matrix " << offsets << " to " << filename << "."
             << std::endl);
    fclose(fp);
    return false;
  }
  fclose(fp);
  return ret;
}

template <typename T>
bool save_matrix(std::list<std::string> &filenames,
		 const std::string &filename) {
  if (filenames.size() == 0) eblerror("expected a non-empty list");
  FILE *fp = fopen(filename.c_str(), "wb+");
  if (!fp) {
    eblwarn( "save_matrix failed (" << filename << "): ");
    perror("");
    return false;
  }
  // determine matrices dims
  idxdim alld(filenames.size());
  std::string s = filenames.front();
  bool multi = has_multiple_matrices(s.c_str());
  if (multi)
    eblerror("expected a single-matrix file in " << s.c_str());
  idx<T> e = load_matrix<T>(s);
  idxdim d = e.get_idxdim();
  for (uint i = 0; i < d.order(); ++i)
    alld.insert_dim(alld.order(), d.dim(i));
  // allocate matrix
  idx<T> all(alld);
  idx_clear(all);
  // add all matrices
  intg j = 0;
  for (std::list<std::string>::iterator i = filenames.begin();
       i != filenames.end(); ++i, ++j) {
    if (multi)
      eblerror("expected a single-matrix file in " << s.c_str());
    idx<T> e = all.select(0, j);
    load_matrix<T>(e, *i);
  }
  // save matrix
  return save_matrix(all, filename);
}

template <typename T>
bool save_csv_matrix(idx<T> &m, const std::string &fname) {
  char separator = ',';
  std::ofstream of(fname.c_str(), std::ofstream::trunc | std::ofstream::out);
  if (m.order() == 1) {
    idx_bloop1(e, m, T) { of << e.get() << separator << std::endl; }
  } else if (m.order() == 2) {
    idx_bloop1(v, m, T) {
      idx_bloop1(e, v, T) { of << e.get() << separator; }
      of << std::endl;
    }
  } else {
    eblerror("expected a vector or matrix only: " << m);
    of.close();
    return false;
  }
  of.close();
  return true;
}

// saving to strings ///////////////////////////////////////////////////////////

template <typename T> std::string save_matrix_to_string(idx<T> &m) {
  uint header_size = (2 + std::max(m.order(), 3)) * sizeof (int);
  uint body_size = m.nelements() * sizeof (T);
  uint total_size = header_size + body_size;
  std::string s(total_size, 0);
  char *p = &(s[0]);
  int v, i;
  // write header
  v = get_magic<T>();
  memcpy(p, (const void*) &v, sizeof (int));
  p += sizeof (int);
  v = m.order();
  memcpy(p, (const void*) &v, sizeof (int));
  p += sizeof (int);
  for (i = 0; (i < m.order()) || (i < 3); ++i) {
    if (i < m.order()) v = m.dim(i);
    else v = 1;
    memcpy(p, (const void*) &v, sizeof (int));
    p += sizeof (int);
  }
  // write body
  idx_aloop1(k, m, T) {
    memcpy(p, (const void*) &(*k), sizeof (T));
    p += sizeof (T);
  }
  return s;
}

template <typename T>
std::string save_matrices_to_string(svector<idx<T> > &v) {
  if (v.empty()) eblerror("cannot save an empty vector");
  midx<T> m(v.size());
  for (uint i = 0; i < v.size(); ++i) m.mset(v[i], i);
  return save_matrices_to_string(m);
}

template <typename T>
std::string save_matrices_to_string(std::list<idx<T>*> &l) {
  if (l.size() == 0) eblerror("cannot save an empty list");
  midx<T> m(l.size());
  intg j = 0;
  for (typename std::list<idx<T>*>::iterator i = l.begin();
       i != l.end(); ++i, j++)
    if (*i) m.mset(**i, j);
  return save_matrices_to_string(m);
}

template <typename T>
std::string save_matrices_to_string(midx<T> &m) {
  std::string soffsets, sdata;
  // allocate offset matrix
  idx<int64> offsets(m.get_idxdim());
  idx_clear(offsets);
  // save offsets matrix first
  soffsets += save_matrix_to_string(offsets);
  // then save all matrices contained in m
  // TODO: implement generic looping for midx
  if (m.order() == 1) {
    for (uint i = 0; i < m.dim(0); ++i) {
      if (m.exists(i)) {
        // save offset of matrix
        offsets.set((int64) soffsets.size() + sdata.size(), i);
        // save matrix to file
        idx<T> e = m.mget(i);
        sdata += save_matrix_to_string(e);
      }
    }
  } else if (m.order() == 2) {
    for (uint i = 0; i < m.dim(0); ++i) {
      for (uint j = 0; j < m.dim(1); ++j) {
        if (m.exists(i, j)) {
          // save offset of matrix
          offsets.set((int64) soffsets.size() + sdata.size(), i, j);
          // save matrix to file
          idx<T> e = m.mget(i, j);
          sdata += save_matrix_to_string(e);
        }
      }
    }
  }
  // finally rewrite offset matrix at beginning of file
  soffsets = save_matrix_to_string(offsets);
  std::string final = soffsets;
  final += sdata;
  return final;
}

} // end namespace ebl

#endif /* IDXIO_HPP_ */
