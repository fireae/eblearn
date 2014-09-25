#include "eblearn/libidx.h"
#include "eblearn/libidxgui.h"

using namespace std;
using namespace ebl;

MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
  try {
    // paths
    string ebl = "/home/pierre/eblearn/"; // set your eblearn root
    string image = ebl;
    image << "tools/data/barn.png";

    // load and display image
    idx<float> m = load_image<float>(image);
    draw_matrix(m);
    gui << "original image";

    // reduce blue in image
    idx<float> blue = m.select(2, 2); // select blue channel
    idx_dotc(blue, .5, blue); // multiply blue channel by .5
    draw_matrix(m, (uint) 0, m.dim(1) + 5);
    gui << at(0, m.dim(1) + 5) << "50% blue";
    
  } eblcatch();
  return 0;
}
