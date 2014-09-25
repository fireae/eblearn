#include "ebl_preprocessing_test.h"

#include <ostream>

#define __SHOW__

#include "netconf.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace ebl;

extern string *gl_data_dir;
extern string *gl_data_errmsg;

void ebl_preprocessing_test::setUp() {
}

void ebl_preprocessing_test::tearDown() {
}

void ebl_preprocessing_test::test_resizing() {
  string path, s;
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  path << *gl_data_dir << "/barn.png";
  //path << *gl_data_dir << "/lena.png";
  //path << *gl_data_dir << "/reagan.jpg";
  idx<float> im = load_image<float>(path);
  path = ""; path << *gl_data_dir << "/2008_007714.jpg";
  idx<float> imbird = load_image<float>(path);
  bool glob_norm = false;
  idxdim kd(9, 9), ind(100, 300), kd5(5, 5);
  rgb_to_yn_module<float> pp(kd, false, WSTD_NORM, glob_norm);
  rgb_to_yn_module<float> ppl(kd, true, LAPLACIAN_NORM, glob_norm);
  rgb_to_yn_module<float> pp5(kd5, false, WSTD_NORM, glob_norm);
  rgb_to_ynuv_module<float> ppuv(kd, false, WSTD_NORM, glob_norm);
  rgb_to_ynuv_module<float> ppuv5(kd5, false, WSTD_NORM, glob_norm);
  rgb_to_y_module<float> ppy;
  rgb_to_yuv_module<float> ppyuv;
  rgb_to_hp_module<float> pph(kd);
  resizepp_module<float> mean_rspp(ind, MEAN_RESIZE, &pp);
  resizepp_module<float> bil_rspp(ind, BILINEAR_RESIZE, &pp);
  resizepp_module<float> gaus_rspp(ind, GAUSSIAN_RESIZE, &pp);
  resizepp_module<float> mean(ind, MEAN_RESIZE, NULL);
  resizepp_module<float> bil(ind, BILINEAR_RESIZE, NULL);
  resizepp_module<float> gaus(ind, GAUSSIAN_RESIZE, NULL);
  idxdim lpd(192, 192);
  midxdim mlpd(lpd);
  laplacian_pyramid_module<float> lp5(3, mlpd, MEAN_RESIZE, &ppyuv);
  // laplacian_pyramid_module<float>
  //   lp5l(3, lpd, MEAN_RESIZE, &ppy, false, NULL, false, true);
  // pyramid_module<float> lp5(3, .5, lpd, MEAN_RESIZE, &pplw);
  // pyramid_module<float> lp5l(3, .5, lpd, MEAN_RESIZE, &ppl);
  pyramid_module<float> pyr(3, .5, lpd, MEAN_RESIZE, &ppuv);
  pyramid_module<float> pyr5(3, .5, lpd, MEAN_RESIZE, &ppuv5);
  // barn
  im = im.shift_dim(2, 0);
  state<float> in(im.get_idxdim()), out(1, 1, 1);
  idx_copy(im, in);
  im = im.shift_dim(0, 2);
  pp.fprop(in, out);
  idx<float> im2 = out.shift_dim(0, 2);
  im2 = idx_copy(im2);
  ppuv.fprop(in, out);
  idx<float> im3 = out.shift_dim(0, 2);
  im3 = idx_copy(im3);
  bil_rspp.fprop(in, out);
  idx<float> im4 = out.shift_dim(0, 2);
  im4 = idx_copy(im4);
  gaus_rspp.fprop(in, out);
  idx<float> im5 = out.shift_dim(0, 2);
  im5 = idx_copy(im5);
  mean_rspp.fprop(in, out);
  idx<float> im6 = out.shift_dim(0, 2);
  im6 = idx_copy(im6);
  // barn laplacian pyramid
  // midx<float> outs(1);
  // lp5l.fprop(in, outs);
  // outs.shift_dim_internal(0, 2);
  midx<float> outs1(1);
  lp5.fprop(in, outs1);
  outs1.shift_dim_internal(0, 2);
  midx<float> outs2(1);
  pyr5.fprop(in, outs2);
  outs2.shift_dim_internal(0, 2);
  midx<float> outs3(1);
  pyr.fprop(in, outs3);
  outs3.shift_dim_internal(0, 2);
  // resize an input region without keeping aspect ratio
  rect<int> inr(159, 45, 105, 282), outr;
  idx<float> barn_no_ar = image_resize(im, 100, 100, 1, &inr, &outr);
  barn_no_ar = image_region_to_rect(barn_no_ar, outr, 100, 100);
  // bird
  imbird = imbird.shift_dim(2, 0);
  state<float> inb(imbird.get_idxdim());
  idx_copy(imbird, inb);
  imbird = imbird.shift_dim(0, 2);
  pp.fprop(inb, out);
  idx<float> im7 = out.shift_dim(0, 2);
  im7 = idx_copy(im7);
  ppuv.fprop(inb, out);
  idx<float> im8 = out.shift_dim(0, 2);
  im8 = idx_copy(im8);
  bil_rspp.fprop(inb, out);
  idx<float> im9 = out.shift_dim(0, 2);
  im9 = idx_copy(im9);
  gaus_rspp.fprop(inb, out);
  idx<float> im10 = out.shift_dim(0, 2);
  im10 = idx_copy(im10);
  mean_rspp.fprop(inb, out);
  idx<float> im11 = out.shift_dim(0, 2);
  im11 = idx_copy(im11);
  pph.fprop(inb, out);
  idx<float> im12 = out.shift_dim(0, 2);
  im12 = idx_copy(im12);
  bil.fprop(inb, out);
  idx<float> im13 = out.shift_dim(0, 2);
  im13 = idx_copy(im13);
  gaus.fprop(inb, out);
  idx<float> im14 = out.shift_dim(0, 2);
  im14 = idx_copy(im14);
  mean.fprop(inb, out);
  idx<float> im15 = out.shift_dim(0, 2);
  im15 = idx_copy(im15);
#ifdef __GUI__
#ifdef __SHOW__
  uint h = 0, w = 0;
  new_window("ebl_preprocessing_test");

  rect<int> r(200, 100, 50, 25);
  idx<float> tmp = im.narrow(0, r.height, r.h0);
  tmp = tmp.narrow(1, r.width, r.w0);
  idx_clear(tmp);
  //draw_box(r);
  double angle = 30;
  draw_cross(r.hcenter(), r.wcenter(), 10, 255, 0, 0);
  im = image_rotate(im, angle, r.hcenter(), r.wcenter());
  r.rotate(angle);
  draw_box(r, 0, 255, 0);

  // barn
  s = ""; s << "RGB " << im;
  draw_matrix(im, s.c_str(), h, w);
  w += im.dim(1) + 2;
  s = ""; s << "Yp " << im2;
  draw_matrix(im2, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  // draw barn_no_ar input region
  s = "no asp.ratio input";
  rect<int> inr2 = inr;
  inr2.h0 += h; inr2.w0 += w;
  draw_box(inr2, 0, 0, 255, 255, &s);
  w += im2.dim(1) + 2;
  s = ""; s << "YpUV " << im3;
  draw_matrix(im3, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  h += im3.dim(0) + 2;
  w = 0;
  s = ""; s << "bilinear " << im4;
  draw_matrix(im4, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im4.dim(1) + 2;
  s = ""; s << "gaussian " << im5;
  draw_matrix(im5, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im5.dim(1) + 2;
  s = ""; s << "mean " << im6;
  draw_matrix(im6, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im6.dim(1) + 2;
  s = ""; s << "bilinear no asp.ratio " << barn_no_ar;
  draw_matrix(barn_no_ar, s.c_str(), h, w, 1, 1, (float)0, (float)255);
  // rect<int> outr2 = outr;
  // outr2.h0 += h; outr2.w0 += w;
  // draw_box(outr2, 0, 0, 255, 255);
  h += barn_no_ar.dim(0) + 2;
  w = 0;
  // laplacian pyramid 5x5 global norm
  h += 15;
  uint htmp;
  // s = "laplacian (loc norm) ";
  // for (uint i = 0; i < outs.dim(0); ++i) {
  //   if (outs.exists(i)) {
  //     idx<float> layer = outs.get(i);
  //     s << layer;
  //     gui << at(h - 15, w) << s;
  //     draw_matrix(layer, NULL, h, w, 1, 1, (float)-1, (float)1);
  //     w += layer.dim(1) + 2;
  //     s = "";
  //     if (i == 0)
  // 	htmp = layer.dim(0) + 2;
  //   }
  // }
  // w += 10;
  // laplacian pyramid 5x5 global norm
  s = "laplacian (glob norm) ";
  for (uint i = 0; i < outs1.dim(0); ++i) {
    if (outs1.exists(i)) {
      idx<float> layer = outs1.mget(i);
      // idx<float> layer2 = outs2.get(i);
      // cout << "************** idx_sqrdist outs1 outs2 " << idx_sqrdist(layer, layer2) << endl;
      s << layer;
      gui << at(h - 15, w) << s;
      draw_matrix(layer, NULL, h, w, 1, 1, (float)-1, (float)1);
      // cout << "layer " << layer << endl;
      // idx_eloop1(laye, layer, float) {
      // 	cout << "min " << idx_min(laye) << " max " << idx_max(laye) << endl;
      // }
      w += layer.dim(1) + 2;
      s = "";
      if (i == 0)
	htmp = layer.dim(0) + 2;
    }
  }
  w += 10;
  // regular pyramid 5x5
  s = "";
  s << "pyramid (" << kd5 << ") ";
  for (uint i = 0; i < outs2.dim(0); ++i) {
    if (outs2.exists(i)) {
      idx<float> layer = outs2.mget(i);
      s << layer;
      gui << at(h - 15, w) << s;
      draw_matrix(layer, NULL, h, w, 1, 1, (float)-1, (float)1);
      w += layer.dim(1) + 2;
      s = "";
      if (i == 0)
	htmp = layer.dim(0) + 2;
    }
  }
  w += 10;
  // regular pyramid 9x9
  s = "";
  s << "pyramid (" << kd << ") ";
  for (uint i = 0; i < outs3.dim(0); ++i) {
    if (outs3.exists(i)) {
      idx<float> layer = outs3.mget(i);
      s << layer;
      gui << at(h - 15, w) << s;
      draw_matrix(layer, NULL, h, w, 1, 1, (float)-1, (float)1);
      w += layer.dim(1) + 2;
      s = "";
      if (i == 0)
	htmp = layer.dim(0) + 2;
    }
  }
    h += htmp;
    w = 0;
  // // bird
  // h += 15;
  // gui << at(h - 15, w) << "RGB " << im;
  // draw_matrix(imbird, h, w);
  // w += imbird.dim(1) + 152;
  // gui << at(h - 15, w) << "Yp " << im7;
  // draw_matrix(im7, h, w, 1, 1, (float)-1, (float)1);
  // w += im7.dim(1) + 152;
  // gui << at(h - 15, w) << "YpUV " << im8;
  // draw_matrix(im8, h, w, 1, 1, (float)-1, (float)1);
  // w += im8.dim(1) + 152;
  // gui << at(h - 15, w) << "Hp " << im12;
  // draw_matrix(im12, h, w, 1, 1, (float)-1, (float)1);
  // h += im12.dim(0) + 2;
  // w = 0;
  // s = ""; s << "bilinear " << im9;
  // draw_matrix(im9, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  // w += im9.dim(1) + 2;
  // s = ""; s << "gaussian " << im10;
  // draw_matrix(im10, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  // w += im10.dim(1) + 2;
  // s = ""; s << "mean " << im11;
  // draw_matrix(im11, s.c_str(), h, w, 1, 1, (float)-1, (float)1);
  // h += im11.dim(0) + 2;
  // w = 0;
  // s = ""; s << "bilinear " << im13;
  // draw_matrix(im13, s.c_str(), h, w, 1, 1, (float)0, (float)255);
  // w += im13.dim(1) + 2;
  // s = ""; s << "gaussian " << im14;
  // draw_matrix(im14, s.c_str(), h, w, 1, 1, (float)0, (float)255);
  // w += im14.dim(1) + 2;
  // s = ""; s << "mean " << im15;
  // draw_matrix(im15, s.c_str(), h, w, 1, 1, (float)0, (float)255);
  // h += im15.dim(0) + 2;

#endif
#endif
}

void ebl_preprocessing_test::test_preprocessing_modules() {
  typedef float T;
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  // create preprocessing modules
  vector<resizepp_module<T>*> mods;
  idxdim ker3(3, 3), ker5(5, 5), ker7(7,7), ker9(9, 9);
  midxdim kers953, kers73, kers999, kers555, kers55;
  midxdim zpads;
  kers953.push_back(ker9);
  kers953.push_back(ker5);
  kers953.push_back(ker3);
  kers73.push_back(ker7);
  kers73.push_back(ker3);
  kers999.push_back(ker9);
  kers999.push_back(ker9);
  kers999.push_back(ker9);
  kers555.push_back(ker5);
  kers555.push_back(ker5);
  kers555.push_back(ker5);
  kers55.push_back(ker5);
  kers55.push_back(ker5);
  midxdim in16(idxdim(16, 16)), in32(idxdim(32, 32)), in48(idxdim(48, 48)),
    in64(idxdim(64, 64));
  bool globnorm = true;
  bool keep_ar = true;
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker5, "bilinear", keep_ar, 3, NULL,globnorm));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker5, "bilinear", keep_ar, 3, NULL,globnorm,
  // 		  true,true,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker3, "bilinear", keep_ar, 3, NULL,globnorm,
  // 		  true,false,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker5, "bilinear", keep_ar, 3, NULL,globnorm,
  // 		  true,false,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker7, "bilinear", keep_ar, 3, NULL,globnorm,
  // 		  true,false,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker9, "bilinear", keep_ar, 3, NULL,globnorm,
  // 		  true,false,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker5, "bilinear", keep_ar, 3, NULL,globnorm,
  // 		  false,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "YUV", ker5, "bilinear", keep_ar, 3, NULL,false,true));
  // mods.push_back(create_preprocessing<T>
  // 		 (64, 64, "RGB", ker5, "bilinear", keep_ar, 3, NULL, globnorm));
  // const char *chans[] = { "YnUV", "YnUVn", "YnUnVn", "YUVn", "RGBn"};
  // for (uint i = 0; i < sizeof (chans) / sizeof (chans[0]); ++i) {
  //   mods.push_back(create_preprocessing<T>
  // 		   (64, 64, chans[i], ker5, "mean", keep_ar, 0, NULL, globnorm));
  //   mods.push_back(create_preprocessing<T>
  // 		   (32, 32, chans[i], ker5, "mean", keep_ar, 0, NULL, globnorm));
  //   mods.push_back(create_preprocessing<T>
  // 		   (16, 16, chans[i], ker5, "mean", keep_ar, 0, NULL, globnorm));
  // }

  mods.push_back(create_preprocessing<T>
  		 (in64, "YUV", kers953, zpads, "bilinear", keep_ar, 3, NULL,
		  NULL, globnorm, false, false, false, false));
  mods.push_back(create_preprocessing<T>
  		 (in64, "YUV", kers953, zpads, "bilinear", keep_ar, 3, NULL,
		  NULL, globnorm, true, false, false, false));
  mods.push_back(create_preprocessing<T>
  		 (in64, "YUV", kers953, zpads, "bilinear", keep_ar, 3, NULL,
		  NULL, globnorm, true, false, true, true));
  mods.push_back(create_preprocessing<T>
  		 (in64, "YUV", kers953, zpads, "bilinear", keep_ar, 3, NULL,
		  NULL, globnorm, true, false, true, false));
  mods.push_back(create_preprocessing<T>
  		 (in64, "YUV", kers953, zpads, "bilinear", keep_ar, 3, NULL,
		  NULL, globnorm, true, true, true, true));
  mods.push_back(create_preprocessing<T>
  		 (in64, "YUV", kers555, zpads, "bilinear", keep_ar, 3, NULL,
		  NULL, globnorm, true, false, true, true));

  mods.push_back(create_preprocessing<T>
  		 (in48, "YUV", kers73, zpads, "bilinear", keep_ar, 2, NULL,
		  NULL, globnorm, false, false, false, false));
  mods.push_back(create_preprocessing<T>
  		 (in48, "YUV", kers73, zpads, "bilinear", keep_ar, 2, NULL, NULL, globnorm,
  		  true, false, false, false));
  mods.push_back(create_preprocessing<T>
  		 (in48, "YUV", kers73, zpads, "bilinear", keep_ar, 2, NULL, NULL, globnorm,
  		  true, false, true, true));
  mods.push_back(create_preprocessing<T>
  		 (in48, "YUV", kers73, zpads, "bilinear", keep_ar, 2, NULL, NULL, globnorm,
  		  true, false, true, false));
  mods.push_back(create_preprocessing<T>
  		 (in48, "YUV", kers73, zpads, "bilinear", keep_ar, 2, NULL, NULL, globnorm,
  		  true, true, true, true));
  mods.push_back(create_preprocessing<T>
  		 (in48, "YUV", kers55, zpads, "bilinear", keep_ar, 2, NULL, NULL, globnorm,
  		  true, false, true, true));

  const char *chans[] = { "YnUV", "YnUVn" };
  for (uint i = 0; i < sizeof (chans) / sizeof (chans[0]); ++i) {
    mods.push_back(create_preprocessing<T>
		   (in64, chans[i], kers55, zpads, "mean", keep_ar, 0, NULL, NULL, globnorm));
    mods.push_back(create_preprocessing<T>
		   (in32, chans[i], kers55, zpads, "mean", keep_ar, 0, NULL, NULL, globnorm));
    mods.push_back(create_preprocessing<T>
		   (in16, chans[i], kers55, zpads, "mean", keep_ar, 0, NULL, NULL, globnorm));
  }

  uint h = 0, hmax = 0;
#ifdef __GUI__
  uint w = 0, wmax = 0, htmp = 0;
  T minval, maxval;
  int wid = -1;
#endif
  const char *files[] = { "dark.ppm", "contrast.ppm", "blurry.ppm" };
  // loop on images
  for (uint i = 0; i < sizeof (files) / sizeof (files[0]); ++i) {
    string path;
    path << *gl_data_dir << "/signs/" << files[i];
    idx<T> im = load_image<T>(path);
    hmax = std::max(hmax, (uint) (h + im.dim(0) + 5));
  }
  // loop on images
  for (uint i = 0; i < sizeof (files) / sizeof (files[0]); ++i) {
#ifdef __GUI__
#ifdef __SHOW__
    if (wid < 0) wid = new_window("Preprocessing modules test");
    gui << black_on_white();
    uint w2 = w;
    string path;
    path << *gl_data_dir << "/signs/" << files[i];
    idx<T> im = load_image<T>(path);
    // draw original

    // im = image_resize(im, (double) 64, (double) 64, 0);
    // //			keep_aspect_ratio ? 0 : 1, &inr, &outr);
    // im = im.select(2, 0);
    // image_global_normalization(im);
    // draw_matrix(im, h, w, 1, 1, idx_min(im), idx_max(im)); h += im.dim(0) + 5;
    htmp = h;
    gui << at(h, w) << im; h += 16;
    gui << at(h, w) << "min: " << idx_min(im); h += 16;
    gui << at(h, w) << "max: " << idx_max(im); h += 16;
    h = htmp;
    w += 100;
    draw_matrix(im, h, w); h += im.dim(0) + 5;
    wmax = std::max(wmax, (uint) (w + im.dim(1) + 5));
    im = im.shift_dim(2, 0);
    w = w2;
    h = hmax;
    state<T> in;
    midx<T> out(1);
    in = im;
    // loop on preprocessing modules
    for (uint j = 0; j < mods.size(); ++j) {
      mods[j]->fprop(in, out);
      mods[j]->get_display_range(minval, maxval);
      gui << at(h, w) << mods[j]->name(); h += 16;
      // loop on layers
      for (uint k = 0; k < out.dim(0); ++k) {
	if (!out.exists(k)) continue ;
	idx<T> o = out.mget(k);
	o = o.shift_dim(0, 2);
	gui << at(h, w) << o;
	gui << at(h + 15, w) << "sum:" << idx_sum(o);
	w += 100;
	draw_matrix(o, h, w, 1, 1, minval, maxval);
	wmax = std::max(wmax, (uint) (w + o.dim(1) + 5));
	// loop on channels
	o = o.shift_dim(2, 0);
	idx_bloop1(chan, o, T) {
	  w += chan.dim(1) + 5;
	  draw_matrix(chan, h, w, 1, 1, minval, maxval);
	  wmax = std::max(wmax, (uint) (w + chan.dim(1) + 5));
	}
	w = w2;
	h += o.dim(1) + 5;
      }
      out.clear();
    }
    w = wmax;
    h = 0;
#endif
#endif
  }
  // delete modules
  for (uint j = 0; j < mods.size(); ++j)
    delete mods[j];
}
