#include "imagemanipulation.hh"

#include <stdexcept>

SDL_Texture* ps::ExtractShadow::manipulate(SDL_Texture* origin, SDL_Rect const& rect, SDL_Renderer* ren) const
{
    uint8_t pixels[rect.w * rect.h * 4];
    if (SDL_RenderReadPixels(ren, &rect, SDL_PIXELFORMAT_RGBA32, pixels, rect.w * 4) != 0)
        throw std::runtime_error("Could not read pixels from surface.");

    SDL_Surface* sf = SDL_CreateRGBSurfaceWithFormat()

    return origin;
}
