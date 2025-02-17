#ifndef PASTEL2D_HH
#define PASTEL2D_HH

#include "graphics.h"
#include "res.h"
#include "error.h"
#include "audio.h"
#include "passert.h"
#include "manip.h"

int ps_init(ps_GraphicsInit const* graphics);
int ps_finalize();

const char* ps_version(uint8_t* major, uint8_t* minor, uint8_t* patch);

#endif //PASTEL2D_HH
