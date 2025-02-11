#ifndef IMAGEMANIPULATION_HH
#define IMAGEMANIPULATION_HH

#include "SDL2/SDL.h"

namespace ps {

struct ImageManipulation {
    virtual ~ImageManipulation() = default;
    virtual SDL_Texture* manipulate(SDL_Texture* origin, SDL_Rect const& rect, SDL_Renderer* ren) const = 0;
};

struct ExtractShadow : ImageManipulation {
    explicit ExtractShadow(SDL_Color const& shadow_color_)
        : shadow_color(shadow_color_) {}

    SDL_Texture* manipulate(SDL_Texture* origin, SDL_Rect const& rect, SDL_Renderer* ren) const override;

private:
    SDL_Color const& shadow_color;
};

}

#endif //IMAGEMANIPULATION_HH
