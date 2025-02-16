#include "pastel2d.h"

int ps_init(GraphicsInit const* graphics)
{
    if (ps_res_init() != 0)
        return -1;
    if (ps_graphics_init(graphics) != 0)
        return -1;
    if (ps_audio_init() != 0)
        return -1;
    return 0;
}

int ps_finalize()
{
    if (ps_res_finalize() != 0)
        return -1;
    if (ps_audio_finalize() != 0)
        return -1;
    if (ps_graphics_finalize() != 0)
        return -1;
    return 0;
}
