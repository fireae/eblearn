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

// tell header that we are in the libidx scope
#define LIBIDX

#include "numerics.h"
#include "defines.h"
#include "utils.h"

int isinf_local(double x){
#ifdef __MAC__
	return std::isinf(x);
#else
	return isinf(x);
#endif
}

namespace ebl {

//#ifdef __WINDOWS__
#include <time.h>
//#endif

// tanh ////////////////////////////////////////////////////////////////////////

// derivative of tanh
double dtanh(double x) {
	double e = exp(-2*(double)(x));
	double e1 = 1+e;
	e1 = e1*e1;
	if (isinf_local(e1)) return 0.0;
	return ((4*e)/e1);
}

double arccot(double x) {
	if (x == 0)
		return PI_OVER2;
	if (x > 0)
		return atan(1/x);
	return PI + atan(1/x);
}

// standard sigmoid ////////////////////////////////////////////////////////////

// Rational polynomial for computing 1.71593428 tanh (0.66666666 x)

#define PR  ((float)0.66666666)
#define PO  ((float)1.71593428)
#define A0   ((float)(1.0))
#define A1   ((float)(0.125*PR))
#define A2   ((float)(0.0078125*PR*PR))
#define A3   ((float)(0.000325520833333*PR*PR*PR))
#define Fone	((float)1.0)
#define Fzero 	((float)0.0)

float stdsigmoid(float x) {
	register float y;

	if (x >= Fzero)
		if (x < (float)13)
			y = A0+x*(A1+x*(A2+x*(A3)));
		else
			return PO;
	else
		if (x > -(float)13)
			y = A0-x*(A1-x*(A2-x*(A3)));
		else
			return -PO;

	y *= y;
	y *= y;
	y *= y;
	y *= y;
	return (x > Fzero) ? PO*(y-Fone)/(y+Fone) : PO*(Fone-y)/(y+Fone);
}

float dstdsigmoid(float x) {
	if (x < Fzero)
		x = -x;
	if (x < (float)13)
	{
		register float y;
		y = A0+x*(A1+x*(A2+x*(A3)));
		y *= y;
		y *= y;
		y *= y;
		y *= y;
		y = (y-Fone)/(y+Fone);
		return PR*PO - PR*PO*y*y;
	}
	else
		return Fzero;
}

#undef PR
#undef PO
#undef A0
#undef A1
#undef A2
#undef A3

#define PR  ((double)0.66666666)
#define PO  ((double)1.71593428)
#define A0   ((double)(1.0))
#define A1   ((double)(0.125*PR))
#define A2   ((double)(0.0078125*PR*PR))
#define A3   ((double)(0.000325520833333*PR*PR*PR))

double stdsigmoid(double x) {
	register double y;

	if (x >= 0.0)
		if (x < (double)13)
			y = A0+x*(A1+x*(A2+x*(A3)));
		else
			return PO;
	else
		if (x > -(double)13)
			y = A0-x*(A1-x*(A2-x*(A3)));
		else
			return -PO;
	y *= y;
	y *= y;
	y *= y;
	y *= y;
	return (x > 0.0) ? PO*(y-1.0)/(y+1.0) : PO*(1.0-y)/(y+1.0);
}

// derivative of the above
double dstdsigmoid(double x) {
	if (x < 0.0)
		x = -x;
	if (x < (double)13)
	{
		register double y;
		y = A0+x*(A1+x*(A2+x*(A3)));
		y *= y;
		y *= y;
		y *= y;
		y *= y;
		y = (y-1.0)/(y+1.0);
		return PR*PO - PR*PO*y*y;
	}
	else
		return 0.0;
}

#undef PR
#undef PO
#undef A0
#undef A1
#undef A2
#undef A3

// random number generator /////////////////////////////////////////////////////

#define MMASK  0x7fffffffL
#define MSEED  161803398L
#define FAC    ((float)(1.0/(1.0+MMASK)))
#define FAC2   ((float)(1.0/0x01000000L))

static int inext, inextp;
static int ma[56];		/* Should not be modified */

bool drand_ini = false;

void init_drand(int x) {
	EDEBUG("init_drand with seed " << x);
	drand_ini = true;
	dseed(x);
	srand(x);
}

int dynamic_init_drand(int argc, char **argv) {
	uint seed = (uint) time(NULL);
	init_drand(seed);
	// use command line arguments to introduce more randomization
	for (int i = 0; i < argc; ++i) {
		std::string s = argv[i];
		// note: we don't care about overflow here.
		for (uint j = 0; j < s.size(); ++j) {
			seed += (uint) drand(limits<uint32>::min(),
													 limits<uint32>::max()) * (uint32) s[j];
		}
	}
	// use pid to introduce more randomization
	seed += (uint) drand(limits<uint32>::min(),
											 limits<uint32>::max()) * (uint32) pid();
	init_drand((int) seed);
	return (int) seed;
}

int fixed_init_drand(){
	int seed = 0;
	init_drand(seed);
	return seed;
}

void dseed(int x) {
	int mj, mk;
	int i, ii;

	mj = MSEED - (x < 0 ? -x : x);
	mj &= MMASK;
	ma[55] = mj;
	mk = 1;
	for (i = 1; i <= 54; i++) {
		ii = (21 * i) % 55;
		ma[ii] = mk;
		mk = (mj - mk) & MMASK;
		mj = ma[ii];
	}
	for (ii = 1; ii <= 4; ii++)
		for (i = 1; i < 55; i++) {
			ma[i] -= ma[1 + (i + 30) % 55];
			ma[i] &= MMASK;
		}
	inext = 0;
	inextp = 31;			/* Special constant */
}

double drand(void) {
	register int mj;
	if (++inext == 56) inext = 1;
	if (++inextp == 56) inextp = 1;
	mj = ((ma[inext] - ma[inextp]) * 84589 + 45989) & MMASK;
	ma[inext] = mj;
	return (double)(mj * FAC);
}

double drand(double v) { return v*2*drand()-v; }
double drand(double v0, double v1) { return (v1-v0)*drand()+v0; }

double dgauss(void) {
	/*
	 * Now a quick and dirty way to build
	 * a quasi-normal random number.
	 */
	register int i;
	register int mj, sum;
	mj = 0;
	sum = 0;
	for (i = 12; i; i--) {
		if (++inext == 56)
			inext = 1;
		if (++inextp == 56)
			inextp = 1;
		mj = (ma[inext] - ma[inextp]) & MMASK;
		ma[inext] = mj;
		if (mj & 0x00800000L)
			mj |= 0xff000000L;
		else
			mj &= 0x00ffffffL;
		sum += mj;
	}
	ma[inext] = (mj * 84589 + 45989) & MMASK;
	return (double)(sum * FAC2);
}

double dgauss(double sigma) { return sigma*dgauss(); }
double dgauss(double m, double sigma) { return sigma*dgauss() + m; }

double gaussian(double x, double m, double sigma) {
	sigma *= sigma * 2;
	x -= m;
	return exp(-x*x / sigma) / sqrt(PI * sigma);
}

#undef MMASK
#undef MSEED
#undef FAC
#undef FAC2

////////////////////////////////////////////////////////////////

int choose(int n, int k) {
	if (k > n) {
          eblwarn( "n choose k with n = " << n << " and k = " << k << std::endl);
		eblerror("n choose k: k must be <= n");
	}
	int nk = n - k;
	int nf = 1, kf = 1, nkf = 1;
	int f = 1, i = 1;
	while ((n > 0) || (k > 0) || (nk > 0)) {
		f *= i++;
		if (--n == 0)
			nf = f;
		if (--k == 0)
			kf = f;
		if (--nk == 0)
			nkf = f;
	}
	return nf / (kf * nkf);
}

////////////////////////////////////////////////////////////////

#ifdef __WINDOWS__

double rint(double x) {
	return floor(x + .5);
}

#endif

} // end namespace ebl
