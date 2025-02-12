#include "imagemanipulation.hh"

#include <iostream>
#include <stdexcept>

SDL_Texture* ps::ImageManipulation::create_texture(SDL_Renderer* ren, SDL_Surface* sf) const
{
    return SDL_CreateTextureFromSurface(ren, sf);
}

SDL_Texture* ps::ExtractShadow::create_texture(SDL_Renderer* ren, SDL_Surface* sf) const
{
    SDL_Surface* new_sf = SDL_ConvertSurfaceFormat(sf, SDL_PIXELFORMAT_RGBA32, 0);

    SDL_LockSurface(new_sf);
    uint8_t* pixels = (uint8_t *) new_sf->pixels;
    for (size_t i = 0; i < (size_t) new_sf->w * (size_t) new_sf->h * 4; i +=4 ) {
        if (pixels[i+3] == 0) { // transparent color
            pixels[i] = pixels[i+1] = pixels[i+2] = pixels[i+3] = 0;
        } else {
            pixels[i] = shadow_color.r;
            pixels[i+1] = shadow_color.g;
            pixels[i+2] = shadow_color.b;
            pixels[i+3] = 255;
        }
    }
    SDL_UnlockSurface(new_sf);

    SDL_Texture* tx = SDL_CreateTextureFromSurface(ren, new_sf);
    SDL_FreeSurface(new_sf);
    return tx;
}

/*
SDL_Texture* ps::ExtractShadow::manipulate(SDL_Texture* origin, SDL_Rect const& rect, SDL_Renderer* ren) const
{
    // read texture data
    uint8_t pixels[rect.w * rect.h * 4];
    if (SDL_RenderReadPixels(ren, &rect, SDL_PIXELFORMAT_RGBA32, pixels, rect.w * 4) != 0)
        throw std::runtime_error("Could not read pixels from surface.");
    SDL_SetRenderTarget(ren, nullptr);

    // manipulate image
    uint8_t sf_pixels[rect.w * rect.h * 4];
    for (size_t i = 0; i < (rect.w * rect.h * 4); i += 4) {
        if (pixels[i] == 0) { // transparent color
            sf_pixels[i] = sf_pixels[i+1] = sf_pixels[i+2] = sf_pixels[i+3] = 0;
        } else {
            sf_pixels[i] = shadow_color.r;
            sf_pixels[i+1] = shadow_color.g;
            sf_pixels[i+2] = shadow_color.b;
            sf_pixels[i+3] = 255;
        }
    }

    // create new texture based on manipulated image
    SDL_Texture* new_tx = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, rect.w, rect.h);
    SDL_UpdateTexture(new_tx, nullptr, sf_pixels, rect.w * 4);
    return new_tx;
}
*/