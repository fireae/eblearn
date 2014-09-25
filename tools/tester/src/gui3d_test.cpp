#include "gui3d_test.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace ebl;

void gui3d_test::test_all() {
#ifndef __GUI3D__
  eblerror("cannot test gui3d if not compiled with __GUI3D__");
#else
  new_window3d("3D gui test");
  draw_sphere(0, 0, 0, 1, "sphere", 0, 0, 255, 100);
  draw_cylinder(0, 0, 0, 2, .5, 0, 0, 0, "0,0", 255, 0, 0, 255, false);
  draw_cylinder(0, 0, 0, 2, .5, 0, 90, 0, "90,0", 0, 255, 0, 255, false);
  draw_cylinder(0, 0, 0, 2, .5, 0, 180, 0, "180,0", 0, 255, 0, 255, false);
  // draw_cylinder(0, 0, 0, 2, .5, 0, 0, 90, "0,90", 0, 0, 255, 255, false);
  // draw_cylinder(0, 0, 0, 2, .5, 0, 0, 180, "0,180", 0, 0, 255, 255, false);
  // draw_cylinder(0, 0, 0, 2, .5, 0, 180, 180, "180,180", 0, 255, 255, 255,false);
  secsleep(5);
#endif
}
