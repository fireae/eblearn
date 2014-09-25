/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

#ifndef FILTERS_HPP_
#define FILTERS_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Filters

  // TODO: cleanup
  template <typename T>
  idx<T> create_mexican_hat(double s, int n) {
    idx<T> m(n, n);
    T vinv = (T) (1/(s*s));
    T total = 0;
    int cx = n/2;
    int cy = n/2;
    for(int x = 0; x < n; x++){
      for(int y = 0; y < n; y++){
	int dx = x - cx;
	int dy = y - cy;
	m.set(-exp(-sqrt(vinv*(dx*dx + dy*dy))), x, y);
	total += m.get(x, y);
      }
    }
    //! set center valus so it's zero sum
    m.set(m.get(cx, cy) - total, cx, cy);
    //! normalize so that energy is 1
    T energy = sqrt(idx_sumsqr(m));
    idx_dotc(m, 1/energy, m);
    return m;
  }

  // TODO: cleanup
  template <typename T>
  idx<T> create_mexican_hat2(int h, int w, double sigma, double sigma_scale) {
    if (h % 2 == 0 || w % 2 == 0) eblerror("expected odd kernel sizes");
    idx<T> m(h, w);
    double sum=0;
    double cons=2.0/(sqrt(3*sigma)*pow(PI,0.75));

    int xoffset=(int)std::floor(w/2.0);
    int yoffset=(int)std::floor(h/2.0);
    double ymult=(double)xoffset/yoffset;
    double maxd=sqrt(pow((double)(w-xoffset-1),2)
		     + pow((double) (ymult*(h-yoffset-1)),2));
    //maxd=Math.max(w-xoffset-1,ymult*(h-yoffset-1));
    for (int x=0;x<w;x++){
      for (int y=0;y<h;y++){
	double t=sigma_scale*sigma
	  *sqrt(pow((double) (x-xoffset),2)
		+ pow((double) (ymult*(y-yoffset)),2))/maxd;
	//t=1.3*sigma*std::max(std::abs(x-xoffset),std::abs((y-yoffset)*ymult))/maxd;
	double v=cons*(1.0-(pow(t,2)/pow(sigma,2)))
	  *exp((-pow(t,2))/(2*pow(sigma,2)));
	sum+=v;
	m.set(v, x, y);
      }
    }
    sum=std::abs(sum);
    idx_dotc(m, 1/std::abs(sum), m);
    return m;
  }

  template <typename T>
  idx<T> create_gaussian_kernel2(uint n, double sig) {
    idx<T> g(n);
    idx_fill_index(g);
    // if not odd, shift by half to get equal peaks at the center
    if (n % 2 == 0) idx_addc(g, -.5, g);
    idx_addc(g, (T) (- (int) ((n - 1) / 2)), g);
    idx_mul(g, g, g);
    idx_dotc(g, -0.5, g);
    idx_dotc(g, 1 / (sig * sig), g);
    idx_exp(g);
    idx_dotc(g, 1 / idx_sum(g), g);
    return g;
  }

  template <typename T>
  idx<T> create_gaussian_kernel2(uint h, uint w, double sig) {
    if (h != w)
      eblerror("this function only handles square filters, "
	       << "use create_gaussian_kernel2()");
    idx<T> g1 = create_gaussian_kernel<T>(h, sig);
    idx<T> g2(h, h);
    idx_m1extm1(g1, g1, g2);
    return g2;
  }

  template <typename T>
  idx<T> create_gaussian_kernel(idxdim &d, double mode) {
    // if (d.order() != 1 && d.order() != 2)
    //   eblerror("unsupported gaussian kernel dimensions " << d);
    // if (d.order() == 1)

    if (d.order() != 2)
      eblerror("unsupported gaussian kernel dimensions " << d);
    if (d.dim(0) == d.dim(1))
      return create_gaussian_kernel<T>((uint) d.dim(0), mode);
    else
      return create_gaussian_kernel<T>((uint) d.dim(0), (uint) d.dim(1), mode);
  }

  template <typename T>
  idx<T> create_burt_adelson_kernel(double a) {
    idx<T> filter(5, 5), f1d(5);
    f1d.set((T) (.25 - a / 2.0), 0);
    f1d.set((T) .25, 1);
    f1d.set((T) a, 2);
    f1d.set(f1d.get(1), 3);
    f1d.set(f1d.get(0), 4);
    idx_m1extm1(f1d, f1d, filter);
    return filter;
  }

  // TODO: cleanup
  template <typename T>
  idx<T> create_gaussian_kernel(uint n_, double mode) {
    int n = n_;
    idx<T> m(n, n);
    double s = ((double)n)/4;
    T vinv;

    if (mode == 0)
      vinv = (T) (1/(s*s));
    else
      vinv = (T) (1/(mode*s));
    //      vinv = (T) (1/(2*s));

    T total = 0;
    int cx = n/2;
    int cy = n/2;
    for(int x = 0; x < n; x++){
      for(int y = 0; y < n; y++){
	int dx = x - cx;
	int dy = y - cy;
	m.set((T) -exp((double)-(vinv*(dx*dx + dy*dy))), x, y);
	total += m.get(x, y);
      }
    }
    //! set center valus so it's zero sum
    //    m.set(m.get(cx, cy) - total, cx, cy);
    //! normalize so that energy is 1
    //    T energy = sqrt(idx_sumsqr(m));
    idx_dotc(m, 1/total, m);
    return m;
  }

  // TODO: cleanup
  template <typename T>
  idx<T> create_gaussian_kernel(uint h, uint w, double mode) {
    idx<T> m(h, w);
    uint min = MIN(h, w); // use smallest dim for gaussian
    double s = (double)(min)/4;
    T vinv;

    if (mode == 0)
      vinv = (T) (1/(s*s));
    else
      vinv = (T) (1/(mode*s));
    //      vinv = (T) (1/(2*s));

    T total = 0;
    int cx = min/2;
    int cy = min/2;
    for(uint x = 0; x < h; x++){
      for(uint y = 0; y < w; y++){
	int dx = x - cx;
	int dy = y - cy;
#ifdef __WINDOWS__
	m.set((T) (-exp((double)(-(vinv*(dx*dx + dy*dy))))), x, y);
#else
	m.set((T) (-exp(-(vinv*(dx*dx + dy*dy)))), x, y);
#endif
	total += m.get(x, y);
      }
    }
    //! set center valus so it's zero sum
    //    m.set(m.get(cx, cy) - total, cx, cy);
    //! normalize so that energy is 1
    //    T energy = sqrt(idx_sumsqr(m));
    idx_dotc(m, 1/total, m);
    return m;
  }

} // end namespace ebl

#endif /* FILTERS_HPP_ */
