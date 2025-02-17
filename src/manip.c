#include "manip.h"

#include <stddef.h>
#include <SDL3/SDL.h>

int ps_manip_shadow(uint8_t* pixels, int w, int h, int pitch, void* data)
{
    SDL_Color* shadow_color = (SDL_Color *) data;

    for (int i = 0; i < w * h * 4; i += 4) {
        if (pixels[i+3] == 0) { // transparent color
            pixels[i] = pixels[i+1] = pixels[i+2] = pixels[i+3] = 0;
        } else {
            pixels[i] = shadow_color->r;
            pixels[i+1] = shadow_color->g;
            pixels[i+2] = shadow_color->b;
            pixels[i+3] = 255;
        }
    }
    return 0;
}
