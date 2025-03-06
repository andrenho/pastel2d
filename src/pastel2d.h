#ifndef PASTEL2D_HH
#define PASTEL2D_HH

#include "graphics.h"
#include "res.h"
#include "audio.h"
#include "manip.h"

int ps_init(ps_GraphicsInit const* graphics);
int ps_finalize();

const char* ps_version();

#endif //PASTEL2D_HH
