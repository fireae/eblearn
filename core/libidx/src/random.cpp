/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include "random.h"
#include "utils.h"
#include "numerics.h"

namespace ebl {

// random number generator /////////////////////////////////////////////////////

#define MMASK  0x7fffffffL
#define MSEED  161803398L
#define FAC    ((float)(1.0/(1.0+MMASK)))
#define FAC2   ((float)(1.0/0x01000000L))

random::random() {
	// no seeding, assumes already seeded
}

random::random(int x) {
	init(x);
	eblprint( "Random seed initialized to " << x << std::endl);
}

random::~random() {
}

void random::time_seed() {
	uint seed = (uint) time(NULL);
	init((int) seed);
}

void random::time_string_seed(std::string &s) {
	// time seed
	uint seed = (uint) time(NULL);
	// initial seed
	init((int) seed);
	// use string to introduce more randomization
	// note: we don't care about overflow here.
	for (uint j = 0; j < s.size(); ++j) {
		seed += (uint) drand(limits<uint32>::min(),
												 limits<uint32>::max()) * (uint32) s[j];
	}
	// use pid to introduce more randomization
	seed += (uint) drand(limits<uint32>::min(),
					 limits<uint32>::max()) * (uint32) pid();
	init((int) seed);
}

void random::time_args_seed(int argc, char **argv) {
	// time seed
	uint seed = (uint) time(NULL);
	// initial seed
	init((int) seed);
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
	init((int) seed);
}

double random::drand(void) {
	register int mj;
	if (++inext == 56) inext = 1;
	if (++inextp == 56) inextp = 1;
	mj = ((ma[inext] - ma[inextp]) * 84589 + 45989) & MMASK;
	ma[inext] = mj;
	return (double)(mj * FAC);
}

double random::drand(double v) {
	return v*2*drand()-v;
}

double random::drand(double v0, double v1) {
	return (v1-v0)*drand()+v0;
}

// internal methods //////////////////////////////////////////////////////////

void random::init(int x) {
	dseed(x);
	srand(x);
}

void random::dseed(int x) {
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

} // end namespace ebl
