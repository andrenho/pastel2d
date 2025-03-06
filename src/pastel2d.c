#include "pastel2d.h"

#include "private/graphics_priv.h"
#include "private/audio_priv.h"
#include "private/res_priv.h"

int ps_init(ps_GraphicsInit const* graphics)
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

const char* ps_version()
{
    return PROJECT_VERSION;
}
