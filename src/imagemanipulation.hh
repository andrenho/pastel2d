#ifndef IMAGEMANIPULATION_HH
#define IMAGEMANIPULATION_HH

#include "SDL2/SDL.h"

namespace ps {

struct ImageManipulation {
    virtual ~ImageManipulation() = default;
    virtual SDL_Texture* create_texture(SDL_Renderer* ren, SDL_Surface* sf) const;
};

struct ExtractShadow : public ImageManipulation {
    explicit ExtractShadow(SDL_Color const& shadow_color_)
        : shadow_color(shadow_color_) {}

    SDL_Texture* create_texture(SDL_Renderer* ren, SDL_Surface* sf) const override;

private:
    SDL_Color const& shadow_color;
};

}

#endif //IMAGEMANIPULATION_HH
