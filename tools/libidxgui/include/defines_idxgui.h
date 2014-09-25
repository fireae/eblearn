#ifndef DEFINES_IDXGUI_H_
#define DEFINES_IDXGUI_H_

#include "defines.h"

// maximum height or width size
#define WIN_MAX_SIZE 80000

#ifdef LIBIDXGUI // we are inside the library
#define IDXGUIEXPORT EXPORT
#else // we are outside
#define IDXGUIEXPORT IMPORT
#endif

#endif /* DEFINES_IDXGUI_H_*/
