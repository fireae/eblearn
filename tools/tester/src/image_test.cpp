#include "image_test.h"

#define __SHOW__

#include "ebl_preprocessing.h"
#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace ebl;

extern string *gl_data_dir;
extern string *gl_data_errmsg;

void image_test::setUp() {
}

void image_test::tearDown() {
}

void image_test::test_resize() {
	try {
		CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
		idx<ubyte> m(6, 8);
		idx<ubyte> res;

		int i = 0;
		{ idx_aloop1(m0, m, ubyte) {
				*m0 = i++;
			}}
		//m.printElems();
		res = image_resize(m, 3, 4, 1);
		//res.printElems();
		CPPUNIT_ASSERT(res.get(0, 0) == 4);
		CPPUNIT_ASSERT(res.get(0, 1) == 6);
		CPPUNIT_ASSERT(res.get(0, 2) == 8);
		CPPUNIT_ASSERT(res.get(0, 3) == 10);
		CPPUNIT_ASSERT(res.get(1, 0) == 20);
		CPPUNIT_ASSERT(res.get(1, 1) == 22);
		CPPUNIT_ASSERT(res.get(1, 2) == 24);
		CPPUNIT_ASSERT(res.get(1, 3) == 26);
		CPPUNIT_ASSERT(res.get(2, 0) == 36);
		CPPUNIT_ASSERT(res.get(2, 1) == 38);
		CPPUNIT_ASSERT(res.get(2, 2) == 40);
		CPPUNIT_ASSERT(res.get(2, 3) == 42);

		string imgfile = *gl_data_dir;
		imgfile += "/barn.png";
		idx<ubyte> im = load_image<ubyte>(imgfile);
#ifdef __GUI__
#ifdef __SHOW__
		new_window("Image Tests");
		draw_matrix(im, "Testing images operations...");
		gui << at(15, 0) << im;
		int h = 0, w = im.dim(1) + 2;
#endif
#endif

#ifdef __GUI__
#ifdef __SHOW__
		idx<ubyte> im2 = image_resize(im, 50, 200, 0);
		idx<ubyte> im3 = image_resize(im, 50, 200, 1);
		draw_matrix(im2, h, w);
		gui << at(h, w) << im2;
		w += im2.dim(1) + 2;
		draw_matrix(im3, h, w);
		gui << at(h, w) << "ignore ratio " << im3;
		w += im3.dim(1) + 2;
		h = im.dim(0) + 2;
#endif
#endif
		// float version
		idx<float> im4(im.get_idxdim());
		idx_copy(im, im4);
		// test downsizing
		idx<float> im5 = image_resize(im4, 96, 96, 0);
		idx<float> im6 = image_gaussian_resize(im4, 96, 96, 0);
		idx<float> im7 = image_mean_resize(im4, 96, 96, 0);
		idx<float> im8 = image_resize(im4, 64, 64, 0);
		idx<float> im9 = image_gaussian_resize(im4, 64, 64, 0);
		idx<float> im10 = image_mean_resize(im4, 64, 64, 0);
		idx<float> im11 = image_resize(im4, 32, 32, 0);
		idx<float> im12 = image_gaussian_resize(im4, 32, 32, 0);
		idx<float> im13 = image_mean_resize(im4, 32, 32, 0);
		// test upsizing starting from mean downsampled image
		idx<float> im14 = image_resize(im13, 96, 96, 0);
		idx<float> im15 = image_gaussian_resize(im13, 96, 96, 0);
		idx<float> im16 = image_mean_resize(im13, 96, 96, 0);
		idx<float> im17 = image_resize(im10, 96, 96, 0);
		idx<float> im18 = image_gaussian_resize(im10, 96, 96, 0);
		idx<float> im19 = image_mean_resize(im10, 96, 96, 0);
		// test resizing of region of the original image
		rect<int> iregion(50, 50, 50, 150);
		rect<int> oregion1, oregion2, oregion3;
		idx<float> im20 = image_resize(im4, 64, 64, 0, &iregion, &oregion1);
		idx<float> im21 = image_gaussian_resize(im4, 64, 64, 0, &iregion,
																						&oregion2);
		idx<float> im22 = image_mean_resize(im4, 64, 64, 0, &iregion, &oregion3);
#ifdef __GUI__
#ifdef __SHOW__
		w = 0;
		gui << black_on_white();
		gui << at(h, w) << "downsizing tests:";
		h += 15;
		gui << at(h, w)<< "bilinear, gaussian and mean from ";
		gui << im4 << " to " << im5 << ", " << im8 << " and " << im11;
		h += 30;
		draw_matrix(im5, h, w); gui << at(h - 15, w) << im5; w += im5.dim(1) + 2;
		draw_matrix(im6, h, w); gui << at(h - 15, w) << im6; w += im6.dim(1) + 2;
		draw_matrix(im7, h, w); gui << at(h - 15, w) << im7; w += im7.dim(1) + 2;
		draw_matrix(im8, h, w); gui << at(h - 15, w) << im8; w += im8.dim(1) + 2;
		draw_matrix(im9, h, w); gui << at(h - 15, w) << im9; w += im9.dim(1) + 2;
		draw_matrix(im10, h, w); gui << at(h - 15, w) << im10; w += im10.dim(1) + 2;
		draw_matrix(im11, h, w); gui << at(h - 15, w) << im11; w += im11.dim(1) + 2;
		draw_matrix(im12, h, w); gui << at(h - 15, w) << im12; w += im12.dim(1) + 2;
		draw_matrix(im13, h, w); gui << at(h - 15, w) << im13; w += im13.dim(1) + 2;
		w = 0; h += im6.dim(0) + 2;
		gui << at(h, w) << "upsizing tests:";
		h += 15;
		gui << at(h, w)<< "bilinear, gaussian and mean from ";
		gui << im13 << " and " << im10 << " (mean downsampled) to " << im14;
		h += 30;
		draw_matrix(im14, h, w); gui << at(h - 15, w) << im14; w += im14.dim(1) + 2;
		draw_matrix(im15, h, w); gui << at(h - 15, w) << im15; w += im15.dim(1) + 2;
		draw_matrix(im16, h, w); gui << at(h - 15, w) << im16; w += im16.dim(1) + 2;
		draw_matrix(im17, h, w); gui << at(h - 15, w) << im17; w += im17.dim(1) + 2;
		draw_matrix(im18, h, w); gui << at(h - 15, w) << im18; w += im18.dim(1) + 2;
		draw_matrix(im19, h, w); gui << at(h - 15, w) << im19; w += im19.dim(1) + 2;
		w = 0; h += im14.dim(0) + 2;
		gui << at(h, w) << "region resizing tests:";
		h += 15;
		gui << at(h, w)<< "bilinear, gaussian and mean from ";
		gui << im << " and region " << iregion << " to 64x64";
		h += 45;
		draw_box(iregion.h0, iregion.w0, iregion.height, iregion.width); // original
		draw_box(h + oregion1.h0, w + oregion1.w0, oregion1.height, oregion1.width);
		draw_matrix(im20, h, w);
		gui << at(h - 30, w) << im20 << at(h - 15, w) << oregion1;
		w += im20.dim(1) + 2;
		draw_box(h + oregion2.h0, w + oregion2.w0, oregion2.height, oregion2.width);
		draw_matrix(im21, h, w);
		gui << at(h - 30, w) << im21 << at(h - 15, w) << oregion2;
		w += im21.dim(1) + 2;
		draw_box(h + oregion3.h0, w + oregion3.w0, oregion3.height, oregion3.width);
		draw_matrix(im22, h, w);
		gui << at(h - 30, w) << im22 << at(h - 15, w) << oregion3;
		w += im22.dim(1) + 2;
#endif
#endif
	} catch(string &err) {
		cerr << err << endl;
	}
}

void image_test::test_pnm_P3() {
	try {
		CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
		string imgfile = *gl_data_dir;
		imgfile += "/pnm/rgb_P3.ppm";
		idx<ubyte> im = load_image<ubyte>(imgfile);

		CPPUNIT_ASSERT(im.get(0, 0, 0) == 255);
		CPPUNIT_ASSERT(im.get(0, 0, 1) == 0);
		CPPUNIT_ASSERT(im.get(0, 0, 2) == 0);
		CPPUNIT_ASSERT(im.get(0, 1, 0) == 0);
		CPPUNIT_ASSERT(im.get(0, 1, 1) == 255);
		CPPUNIT_ASSERT(im.get(0, 1, 2) == 0);
		CPPUNIT_ASSERT(im.get(0, 2, 0) == 0);
		CPPUNIT_ASSERT(im.get(0, 2, 1) == 0);
		CPPUNIT_ASSERT(im.get(0, 2, 2) == 255);
	} catch(string &err) {
		cerr << err << endl;
	}
}

void image_test::test_pnm_P6() {
	try {
		CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
		string imgfile = *gl_data_dir;
		imgfile += "/pnm/rgb_P6.ppm";
		idx<ubyte> im = load_image<ubyte>(imgfile);

		CPPUNIT_ASSERT(im.get(0, 0, 0) == 255);
		CPPUNIT_ASSERT(im.get(0, 0, 1) == 0);
		CPPUNIT_ASSERT(im.get(0, 0, 2) == 0);
		CPPUNIT_ASSERT(im.get(0, 1, 0) == 0);
		CPPUNIT_ASSERT(im.get(0, 1, 1) == 255);
		CPPUNIT_ASSERT(im.get(0, 1, 2) == 0);
		CPPUNIT_ASSERT(im.get(0, 2, 0) == 0);
		CPPUNIT_ASSERT(im.get(0, 2, 1) == 0);
		CPPUNIT_ASSERT(im.get(0, 2, 2) == 255);

		load_image(imgfile, im);
	} catch(string &err) {
		cerr << err << endl;
	}
}

typedef double t_gdata;
void image_test::test_gaussian_pyramid() {
	// TODO: fix test
	//   CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
	//   string imgfile = *gl_data_dir;
	//   imgfile += "/lena.png";
	//   idx<ubyte> im_rgb(1, 1, 3);
	//   image_read_rgbx(imgfile.c_str(), im_rgb);
	//   //  im_rgb = im_rgb.narrow(1, im_rgb.dim(1) / 2, im_rgb.dim(1) / 4);

	//   rect r(im_rgb.dim(0) / 10, 0,
	// 	 im_rgb.dim(0) / 5, im_rgb.dim(1));
	// //   rect r(im_rgb.dim(0) / 10, im_rgb.dim(1) / 4,
	// // 	 im_rgb.dim(0) / 5, im_rgb.dim(1) / 2);
	//   //im_rgb = image_region_to_square(im_rgb, r);
	//     im_rgb = im_rgb.shift_chan(0); // put channel dimension first
	//   idx<t_gdata> im(im_rgb.get_idxdim());
	//   idx_copy(im_rgb, im);
	//   gaussian_pyramid<t_gdata> gp;

	//   idx<t_gdata> in = im;
	//   idx<t_gdata> tin;
	//   idx<t_gdata> reduced;
	// #ifdef __GUI__
	//   uint h = 0, w = 0;
	//   new_window("Testing gaussian pyramid scaling");

	//   //  for (int i = 0; i < 5; ++i) {
	//     // input
	//     draw_matrix(in, "in", h, w, 1.0, 1.0, 0.0, 255.0);
	//     h += in.dim(1) + 5;
	//     // reduced
	//     reduced = gp.reduce(in, 3);
	//     // reduced/expanded
	//     idx<t_gdata> re = gp.expand(reduced);
	//     draw_matrix(re, "R-Expanded", h, w, 1.0, 1.0, 0.0, 255.0);
	//     save_matrix(re, "re.mat");
	//     h += re.dim(1) + 5;
	//     // laplacian
	//     idx<t_gdata> laplacian(re.get_idxdim());
	//     cout << "in " << in << endl;
	//     tin = gp.cut_pad(gp.red_tin, 2);
	//     idx_sub(tin, re, laplacian);
	//     draw_matrix(laplacian, "Laplacian", h, w);
	//     h = 0;
	//     w += laplacian.dim(2) + 5;
	//     in = reduced;
	//     //  }

	//   h = 0, w = 0;
	//   ostringstream oss;
	//   new_window("Gaussian square resize");
	//   // draw original image
	//   oss << "original " << im;
	//   draw_matrix(im, oss.str().c_str(), h);
	//   h += im.dim(1);
	//   // draw region cropped image
	//   idx<t_gdata> cropped = im.narrow(1, r.height, r.h0);
	//   cropped = cropped.narrow(2, r.width, r.w0);
	//   oss.str("");
	//   oss << "bbox cropped original " << cropped << " h0xw0xhxw:";
	//   oss << r.h0 << "x" << r.w0 << "x" << r.height << "x" << r.width;
	//   draw_matrix(cropped, oss.str().c_str(), h);
	//   h += cropped.dim(1);
	//   // draw gaussian resized
	//   in = image_gaussian_square_resize(im, r, 96);
	//   oss.str("");
	//   oss << in;
	//   draw_matrix(in, oss.str().c_str(), h, w);
	//   w += in.dim(2) + 10;
	//   // draw gaussian resized
	//   in = image_gaussian_square_resize(im, r, 45);
	//   oss.str("");
	//   oss << in;
	//   draw_matrix(in, oss.str().c_str(), h, w);
	//   w += in.dim(2) + 10;
	//   // draw gaussian resized
	//   uint reductions = std::max(0, (int)
	// 			gp.count_reductions(std::max(r.height, r.width), 96) - 1);
	//   rect rrr = gp.reduce_rect(r, reductions);
	//   rect rr(0, 0, in.dim(1), in.dim(2));
	//   in = image_gaussian_square_resize(in, rr, 96);
	//   oss.str("");
	//   oss << in;
	//   draw_matrix(in, oss.str().c_str(), h, w);
	//   w += in.dim(2) + 10;
	//#endif
}

void image_test::test_deformations() {
	try {
		CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
		string imgfile;
		imgfile << *gl_data_dir << "/barn.png";
		idx<ubyte> im_rgb = load_image<ubyte>(imgfile);
		idx<float> im(im_rgb.get_idxdim());
		idx_copy(im_rgb, im);

		// idx<float> im(3, 1, 1);
		// float n = 0;
		// idx_aloop1(i, im, float) {
		//   *i = n++;
		// }

		// deformations
		idx<float> scaled(im.get_idxdim());
		idx<float> flow;
		{
			float th = 75, tw = 0, deg = 45, sh = .5, sw = 1, shh = 0, shw = 0;
			int elasize = 50;
			float elacoeff = 50;
			flow = image_deformation_flow(im, th, tw, sh, sw, deg, shh, shw,
																		elasize, elacoeff);
			image_deformation(im, scaled, th, tw, sh, sw, deg, shh, shw,
												elasize, elacoeff);
		}

		// idxdim d(im);
		// d.remove_dim(2);
		// idx<float> grid = create_grid(d);
		// idx<float> gridx = grid.select(0, 0);
		// idx<float> gridy = grid.select(0, 1);

		idx<float> flowx = flow.select(0, 0);
		idx<float> flowy = flow.select(0, 1);

#ifdef __GUI__
		new_window("Image deformations");
		unsigned int h = 0, w = 0;
		// // input
		// draw_matrix(im, "RGB", h, w);
		// //    draw_flow(flow, h, w);
		// w += im.dim(1) + 5;
		// // scaled
		// draw_matrix(scaled, "Scaled *2", h, w);
		// w += scaled.dim(1) + 5;
		// string s;
		// s << "x flow " << flowx;
		// draw_matrix(flowx, s.c_str(), h, w);
		// w += flowx.dim(1) + 5;
		// s = ""; s << "y flow " << flowy;
		// draw_matrix(flowy, s.c_str(), h, w);
		// cout << "flowx min " << idx_min(flowx) << " max " << idx_max(flowx) << endl;
		// cout << "flowy min " << idx_min(flowy) << " max " << idx_max(flowy) << endl;
		// w = 0;
		// h += flowx.dim(0) + 5;

		int trans[] = {-2,2,-2,2};
		float rotations[] = {-20,20};
		float scalings[] = {.8,1.2,.8,1.2};
		float shears[] = {-.2,.2,-.2,.2};
		float els[] = {0,20,0,20};
		uint pads[] = {2,2,2,2};

		vector<int> tr(trans, trans + sizeof(trans) / sizeof(int));
		vector<float> rot(rotations, rotations + sizeof(rotations) / sizeof(float));
		vector<float> sc(scalings, scalings + sizeof(scalings) / sizeof(float));
		vector<float> sh(shears, shears + sizeof(shears) / sizeof(float));
		vector<float> el(els, els + sizeof(els) / sizeof(float));
		vector<uint> pad(pads, pads + sizeof(pads) / sizeof(uint));

		jitter_module<float> j;
		j.set_translations(tr);
		j.set_rotations(rot);
		j.set_scalings(sc);
		j.set_shears(sh);
		j.set_elastics(el);
		// j.set_padding(pad);
		zpad_module<float> zp(2, 2, 2, 2);

		string img2;
		img2 << *gl_data_dir << "/2.mat";
		idx<float> im2 = load_matrix<float>(img2);
		state<float> in, out, out2;
		in = im2;
		idx<float> o, i = im2.shift_dim(0, 2);
		int num_deforms = 20;
		while (num_deforms != 0) {
			num_deforms--;
			disable_window_updates();
			clear_window();
			w = 0;
			double zm = 4;
			j.fprop(in, out);
			zp.fprop(out, out2);
			o = out2.shift_dim(0, 2);
			draw_matrix(i, h, w, zm, zm, (float)-1, (float)1);
			w += (uint) (i.dim(1) * zm + 5);
			draw_matrix(o, h, w, zm, zm, (float)-1, (float)1);
			enable_window_updates();
			millisleep(200);
		}
#endif
	} catch(string &err) {
		cerr << err << endl;
	}
}

void image_test::test_colorspaces() {
	try {
		CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
		string imgfile = *gl_data_dir;
		imgfile += "/barn.png";
		idx<ubyte> im_rgb = load_image<ubyte>(imgfile);

		im_rgb = image_resize(im_rgb, 189, 252);
		idxdim d(im_rgb);
		idx<float> fim_rgb(d);
		idx_copy(im_rgb, fim_rgb);
		idx<float> fim_yuv(d);
		rgb_to_yuv(fim_rgb, fim_yuv);
		idx<float> im_hsv(d);
		rgb_to_hsv(fim_rgb, im_hsv);
		idx<float> im_hsv3(d);
		rgb_to_hsv3(fim_rgb, im_hsv3, .10, .15);

		idx<float> ftmp(d);
		idx<float> ftmp1(d);
		idx<float> ftmp3(1, 1);
		idx<float> ftmp5(d.dim(0), d.dim(1));
		idx<float> ftmp6(d.dim(0), d.dim(1));

		// TODO: wrong values, fixme
		//   CPPUNIT_ASSERT(imb.get(0, 0, 0) == 81);
		//   CPPUNIT_ASSERT(imb.get(0, 0, 1) == 239);
		//   CPPUNIT_ASSERT(imb.get(0, 0, 2) == 90);
		//   CPPUNIT_ASSERT(imb.get(0, 1, 0) == 144);
		//   CPPUNIT_ASSERT(imb.get(0, 1, 1) == 34);
		//   CPPUNIT_ASSERT(imb.get(0, 1, 2) == 53);
		//   CPPUNIT_ASSERT(imb.get(0, 2, 0) == 40);
		//   CPPUNIT_ASSERT(imb.get(0, 2, 1) == 109);
		//   CPPUNIT_ASSERT(imb.get(0, 2, 2) == 239);

		//   idx<ubyte> im0(1, 1, 1);
		//   idx<ubyte> im1(1, 1, 1);
		//   idx<ubyte> im2(1, 1, 1);
		//   idx<ubyte> im3(1, 1, 1);
		//   pnm_fread_into_rgbx(imgfile.c_str(), im0);
		//   im1 = image_resize(im0, 200, 100, 0);
		//   im2 = image_resize(im0, 200, 100, 0);
		//   im3 = image_resize(im0, 200, 100, 0);
		//   idx_clear(im2);
		//   idx_clear(im3);
		//   rgb_to_yuv(im1, im2);
		//   yuv_to_rgb(im2, im3);
		// TODO: complete test

#ifdef __GUI__
		new_window("Testing color spaces");
		idx<ubyte> tmp2;
		idx<float> ftmp2;
		unsigned int h = 0, w = 0;
		// input (RGB)
		draw_matrix(im_rgb, "RGB", h, w);
		w += im_rgb.dim(1) + 5;
		tmp2 = im_rgb.select(2, 0);
		//  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
		draw_matrix(tmp2, "R", h, w);
		w += tmp2.dim(1) + 5;
		tmp2 = im_rgb.select(2, 1);
		//  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
		draw_matrix(tmp2, "G", h, w);
		w += tmp2.dim(1) + 5;
		tmp2 = im_rgb.select(2, 2);
		//  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
		draw_matrix(tmp2, "B", h, w);
		w = 0;
		h += tmp2.dim(0) + 5;

		// yuv
		//   draw_matrix(fim_yuv, "YUV", h, w);
		//   w += fim_yuv.dim(1) + 5;
		ftmp2 = fim_yuv.select(2, 0);

		idx_copy(ftmp2, ftmp5);
		image_global_normalization(ftmp5);
		image_local_normalization(ftmp5, ftmp6, 9);
		idx<float> background(1);
		// the 0.5 thingies are necessary because warp-bilin interprets
		// integer coordinates as being at the center of each pixel.
		//   float x1 = -50.5, y1 = 0.5;
		//   float x2 = ftmp5.dim(0) - 0.5, y2 = 0.5;
		//   float x3 = ftmp5.dim(0) - 0.5, y3 = ftmp5.dim(1) - 0.5;
		//   float x4 =  0.5, y4 = ftmp5.dim(1) - 0.5;
		//   float p1 = 0.5 + 20, q1 = 0.5 + 40;
		//   float p3 = ftmp5.dim(0) - 100 - 0.5, q3 = ftmp5.dim(1) - 30 - 0.5;
		//     image_warp_quad(ftmp6, ftmp5, background, 1,
		// 				-.5, -50.5, // x1 y1
		// 				ftmp5.dim(1) -.5, -.5, // x2 y2
		// 				ftmp5.dim(1) -.5, ftmp5.dim(0) -.5, // x3 y3
		// 				-.5, ftmp.dim(0) - .5, // x4 y4
		// 				-.5, -.5, ftmp5.dim(1) - .5, ftmp5.dim(0) - .5);

		// TODO: find out why following code (warp) sometimes crashes
		//     dynamic_init_drand();
		//     image_deformation_ranperspective(ftmp6, ftmp5, 50, 0, (float) 0.0);
		//     idxdim d3(ftmp5);
		//     idx<float> ftmp7(d3);
		//     idx_copy(ftmp5, ftmp7);

		//  idx_copy(ftmp5, ftmp6);
		//  idx<float> ftmp7 = image_resize(ftmp6, p3, q3);
		//   draw_matrix(ftmp7, "warped Y", h, w, 1, 1, (float)-1.0, (float)1.0);
		//   w += tmp2.dim(1) + 5;


		//   idx<float> hat = create_mexican_hat<float>(2, 5);
		//   image_mexican_filter(ftmp2, ftmp5, 0, 0, 7, &hat, &ftmp3);
		image_mexican_filter(ftmp2, ftmp5, 5, 9);
		image_global_normalization(ftmp5);
		//   cout << "Y inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		//   cout << "Y hat inf: " << (int) idx_min(ftmp5) << " sup:" << (int) idx_max(ftmp5) << endl;
		draw_matrix(ftmp5, "Y (mexican hat = 5, 9x9)", h, w, 1, 1, (float)-1.0, (float)1.0);
		w += tmp2.dim(1) + 5;

		idx_copy(ftmp2, ftmp5);
		image_global_normalization(ftmp5);
		image_local_normalization(ftmp5, ftmp6, 9);
		draw_matrix(ftmp6, "Y (global + local norm 9x9)", h, w, 1, 1, (float)-1.0, (float)1.0);
		w += tmp2.dim(1) + 5;

		image_mexican_filter(ftmp2, ftmp5, 5, 9);
		image_global_normalization(ftmp5);
		image_local_normalization(ftmp5, ftmp6, 9);
		draw_matrix(ftmp6, "Y (mex glob loc 5,9x9)", h, w, 1, 1, (float)-1.0, (float)1.0);
		w += tmp2.dim(1) + 5;

		//   ftmp2 = fim_yuv.select(2, 1);
		//   cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		//   draw_matrix(ftmp2, "U", h, w);
		//   w += ftmp2.dim(1) + 5;
		//   ftmp2 = fim_yuv.select(2, 2);
		//   cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		//   draw_matrix(ftmp2, "V", h, w);

		w = 0;
		h += ftmp2.dim(0) + 5;

		// hsv
		draw_matrix(im_hsv, "HSV", h, w);
		w += im_hsv.dim(1) + 5;
		idx_copy(im_hsv, ftmp);
		ftmp2 = ftmp.select(2, 1); // s
		idx_fill(ftmp2, (float).5);
		ftmp2 = ftmp.select(2, 2); // v
		idx_fill(ftmp2, (float).5);
		hsv_to_rgb(ftmp, ftmp1);
		ftmp2 = im_hsv.select(2, 0);
		//  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		//  cout << "inf tmp1: " << (int) idx_min(ftmp1) << " sup:" << (int) idx_max(ftmp1) << endl;
		draw_matrix(ftmp1, "H", h, w, 1.0, 1.0, (float)63.0, (float)127.0);
		w += ftmp2.dim(1) + 5;
		ftmp2 = im_hsv.select(2, 1);
		//  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		draw_matrix(ftmp2, "S", h, w);
		w += ftmp2.dim(1) + 5;
		ftmp2 = im_hsv.select(2, 2);
		//  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		draw_matrix(ftmp2, "V", h, w);
		w = 0;
		h += ftmp2.dim(0) + 5;

		// hsv3
		draw_matrix(im_hsv3, "HSV3", h, w);
		w += im_hsv3.dim(1) + 5;
		idx_copy(im_hsv3, ftmp);
		ftmp2 = ftmp.select(2, 1); // s
		idx_fill(ftmp2, (float).5);
		ftmp2 = ftmp.select(2, 2); // v
		idx_fill(ftmp2, (float).5);
		hsv3_to_rgb(ftmp, ftmp1);
		ftmp2 = im_hsv3.select(2, 0);
		//  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		//  cout << "inf tmp1: " << (int) idx_min(ftmp1) << " sup:" << (int) idx_max(ftmp1) << endl;
		draw_matrix(ftmp1, "H3", h, w, 1.0, 1.0, (float)63.0, (float)127.0);
		w += ftmp2.dim(1) + 5;
		ftmp2 = im_hsv3.select(2, 1);
		//  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		draw_matrix(ftmp2, "S", h, w);
		w += ftmp2.dim(1) + 5;
		ftmp2 = im_hsv3.select(2, 2);
		//  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
		draw_matrix(ftmp2, "V", h, w);

		w = 0;
		h += ftmp2.dim(0) + 5;
		idx<float> spectrum(50, 420, 3);
		idx<float> ftmp4(50, 420, 3);
		for (int i = 0; i < 420; ++i) {
			ftmp2 = spectrum.select(1, i);
			idx_fill(ftmp2, (float) i);
		}
		ftmp2 = spectrum.select(2, 1);
		idx_fill(ftmp2, (float).5);
		ftmp2 = spectrum.select(2, 2);
		idx_fill(ftmp2, (float).5);
		hsv3_to_rgb(spectrum, ftmp4);
		draw_matrix(ftmp4, "H3 0 - 420", h, w, 1.0, 1.0, (float)63.0, (float)127.0);

		//  sleep(5000);
#endif
	} catch(string &err) {
		cerr << err << endl;
	}
}
