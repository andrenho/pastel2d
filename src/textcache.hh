#ifndef TEXTCACHE_HH
#define TEXTCACHE_HH

#include "scene.hh"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <unordered_map>

#include "resourcemanager.hh"

namespace ps {

class TextCache {
public:
    SDL_Texture* get(Scene::Text const& text, ResourceManager const& res, SDL_Renderer* ren) const;
    void expire_cache();

private:
    struct CacheKey {
        TTF_Font*  font;
        std::string text;
        SDL_Color   color;

        bool operator==(CacheKey const&) const;

        struct Hasher {
            std::size_t operator()(CacheKey const&) const;
        };
    };

    struct Cache {
        SDL_Texture* texture;
        Time         expiration;
    };

    mutable std::unordered_map<CacheKey, Cache, CacheKey::Hasher> cache_;
};

} // ps

#endif //TEXTCACHE_HH
