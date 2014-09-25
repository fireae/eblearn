/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
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

namespace ebl {

// parameter /////////////////////////////////////////////////////////////////

template <typename T> parameter<T>::parameter(intg initial_size)
    : state<T>(initial_size),
      deltax(initial_size), epsilons(initial_size), ddeltax(initial_size) {
  // this class is not meant to be used for training
  this->set_forward_only();
  // clear buffers
  idx_clear(deltax);
  idx_clear(epsilons);
  idx_clear(ddeltax);
  resize_parameter(0);
}

template <typename T>
parameter<T>::parameter(const char *param_filename)
    : state<T>(1), deltax(1), epsilons(1), ddeltax(1) {
  // this class is not meant to be used for training
  this->set_forward_only();
  // load data
  if (!load_x(param_filename)) {
    eblwarn( "failed to open " << param_filename << std::endl);
    eblerror("failed to load bbparameter file in bbparameter constructor");
  }
}

template <typename T>
parameter<T>::~parameter() {
}

// TODO-0: BUG: a bbparameter object casted in state_idx* and called
// with resize(n) calls state_idx::resize instead of
// parameter<T>::resize
// a temporary unclean solution is to use the same bbparameters as
// in state_idx::resize in parameter<T>::resize:
// resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
//		intg s6, intg s7);
template <typename T>
void parameter<T>::resize_parameter(intg s0) {
  x[0].resize(s0);
  if (!dx.empty()) dx[0].resize(s0);
  if (!ddx.empty()) ddx[0].resize(s0);
  deltax.resize(s0);
  epsilons.resize(s0);
  ddeltax.resize(s0);
  idx_clear(deltax);
  idx_clear(epsilons);
  idx_clear(ddeltax);
}

// I/O methods ///////////////////////////////////////////////////////////////

template <typename T>
bool parameter<T>::load_x(std::vector<std::string> &files) {
  if (files.size() == 0) eblerror("expected at least 1 file to load");
  try {
    idx<T> w = load_matrix<T>(files[0]);
    for (uint i = 1; i < files.size(); ++i) {
      idx<T> tmp = load_matrix<T>(files[i]);
      w = idx_concat(w, tmp);
    }
    eblprint( "Concatenated " << files.size() << " matrices into 1: "
              << w << " from " << files << std::endl);
    load_x(w);
    return true;
  } eblcatcherror_msg("failed to load weights");
  return false;
}

template <typename T>
bool parameter<T>::load_x(const char *s) {
  try {
    idx<T> m = load_matrix<T>(s);
    if ((idx<T>::dim(0) != 1) // param has been enlarged by network building
        && (idx<T>::dim(0) != m.dim(0))) // trying to load incompatible network
      eblerror("Trying to load a network with " << m.dim(0)
               << " parameters into a network with " << idx<T>::dim(0)
               << " parameters");
    this->resize_parameter(m.dim(0));
    idx_copy(m, *this);
    eblprint( "Loaded weights from " << s << ": " << *this << std::endl);
    return true;
  } eblcatcherror_msg("failed to load weights");
  return false;
}

template <typename T>
bool parameter<T>::load_x(idx<T> &m) {
  if ((idx<T>::dim(0) != 1) // param has been enlarged by network building
      && (idx<T>::dim(0) != m.dim(0))) { // trying to load incompatible net
    eblerror("Trying to load a network with " << m.dim(0)
             << " parameters into a network with " << idx<T>::dim(0)
             << " parameters");
  }
  this->resize_parameter(m.dim(0));
  idx_copy(m, *this);
  eblprint( "Loaded weights from " << m << ": " << *this << std::endl);
  return true;
}

template <typename T>
bool parameter<T>::save_x(const char *s) {
  if (!save_matrix(*this, s))
    return false;
  return true;
}

template <typename T>
void parameter<T>::permute_x(std::vector<intg> &blocks,
                             std::vector<uint> &permutations) {
  if (blocks.size() != permutations.size())
    eblerror("expected same number of elements in " << blocks
             << " and " << permutations);
  // copy all blocks into temporaries
  svector<idx<T> > copies;
  intg offset = 0;
  for (uint i = 0; i < blocks.size(); ++i) {
    intg sz = blocks[i] - offset;
    idx<T> *m = new idx<T>(sz);
    idx<T> tmp = x[0].narrow(0, sz, offset);
    idx_copy(tmp, *m);
    copies.push_back(m);
    offset += sz;
  }
  // permute list of blocks
  copies.permute(permutations);
  // copy blocks back
  offset = 0;
  for (uint i = 0; i < copies.size(); ++i) {
    intg sz = copies[i].nelements();
    idx<T> tmp = x[0].narrow(0, sz, offset);
    idx_copy(copies[i], tmp);
    offset += sz;
  }
  eblprint( "Permuted weight blocks " << blocks
            << " with permutation vector " << permutations << std::endl);
}

// weights manipulation ////////////////////////////////////////////////////

template <typename T>
void parameter<T>::clear_deltax() {
  idx_clear(deltax);
}

template <typename T>
void parameter<T>::clear_ddeltax() {
  idx_clear(ddeltax);
}

template <typename T>
void parameter<T>::set_epsilon(T m) {
  idx_fill(epsilons, m);
}

template <typename T>
void parameter<T>::compute_epsilons(T mu) {
  idx_addc(ddeltax, mu, epsilons);
  idx_inv(epsilons, epsilons);
}

template <typename T>
void parameter<T>::update(gd_param &arg) {
  update_weights(arg);
}
template <typename T>
void parameter<T>::update_ddeltax(T knew, T kold) {
  idx_lincomb(this->ddx[0], knew, ddeltax, kold, ddeltax);
}

// protected methods /////////////////////////////////////////////////////////

template <typename T>
void parameter<T>::update_weights(gd_param &arg) {
  EDEBUG("updating weights with dx[0] " << dx[0].info()
         << " epsilons " << epsilons.info() << " deltax " << deltax.info());
  if (dx.empty()) eblerror("gradient tensors not found");
  // L2 gradients regularization
  if (arg.decay_l2 > 0) idx_dotcacc(x[0], arg.decay_l2, dx[0]);
  // L1 gradients regularization
  if (arg.decay_l1 > 0) idx_signdotcacc(x[0], (T) arg.decay_l1, dx[0]);
  // weights update
  if (arg.inertia == 0) {
    idx_mul(dx[0], epsilons, dx[0]);
    idx_dotcacc(dx[0], -arg.eta, x[0]);
  } else {
    update_deltax((T) (1 - arg.inertia), (T) arg.inertia);
    idx_mul(deltax, epsilons, dx[0]);
    idx_dotcacc(dx[0], -arg.eta, x[0]);
  }
}

template <typename T>
void parameter<T>::update_deltax(T knew, T kold) {
  if (dx.empty()) eblerror("gradient tensors not found");
  EDEBUG("updating deltax " << knew << " * " << dx[0].info()
         << " + " << kold << " * " << deltax.info());
  idx_lincomb(dx[0], knew, deltax, kold, deltax);
  EDEBUG("updated deltax " << deltax.info());
}

// dparameter /////////////////////////////////////////////////////////////////

template <typename T> dparameter<T>::dparameter(intg initial_size)
    : parameter<T>(initial_size) {
  // initialize backward tensors
  this->dx.push_back(new idx<T>(this->get_idxdim()));
  this->resize_parameter(0);
}

template <typename T>
dparameter<T>::dparameter(const char *param_filename) : state<T>(1) {
  // initialize backward tensors
  this->dx.push_back(new idx<T>(this->get_idxdim()));
  // load data
  if (!this->load_x(param_filename)) {
    eblwarn( "failed to open " << param_filename << std::endl);
    eblerror("failed to load bbdparameter file in bbdparameter constructor");
  }
}

template <typename T>
dparameter<T>::~dparameter() {
}

// ddparameter /////////////////////////////////////////////////////////////////

template <typename T> ddparameter<T>::ddparameter(intg initial_size)
    : parameter<T>(initial_size) {
  // initialize backward tensors
  this->dx.push_back(new idx<T>(this->get_idxdim()));
  this->ddx.push_back(new idx<T>(this->get_idxdim()));
  this->resize_parameter(0);
}

template <typename T>
ddparameter<T>::ddparameter(const char *param_filename) : parameter<T>(1) {
  // initialize backward tensors
  this->dx.push_back(new idx<T>(this->get_idxdim()));
  this->ddx.push_back(new idx<T>(this->get_idxdim()));
  // load data
  if (!this->load_x(param_filename)) {
    eblwarn("failed to open " << param_filename << std::endl);
    eblerror("failed to load bbbbparameter file in bbbbparameter constructor");
  }
}

template <typename T>
ddparameter<T>::~ddparameter() {
}

} // end namespace ebl
