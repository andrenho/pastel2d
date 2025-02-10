#include "textcache.hh"

#include <iostream>
#include <cstdlib>

namespace ps {

SDL_Texture* TextCache::get(Scene::Text const& text, ResourceManager const& res, SDL_Renderer* ren) const
{
    TTF_Font* font = res.font(text.resource);

    auto it = cache_.find({ font, text.text, text.color });

    if (it == cache_.end()) {
        SDL_Surface* sf = TTF_RenderUTF8_Blended(font, text.text.c_str(), text.color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, sf);
        SDL_FreeSurface(sf);
        Time time = hr::now() + text.cache_duration + (1ms * (rand() % 1000));
        std::cout << text.text << "\n";
        cache_[{ font, text.text, text.color }] = Cache { texture, time };
        return texture;
    }

    return it->second.texture;
}

void TextCache::expire_cache()
{
    std::erase_if(cache_, [](auto const& item) {
        return hr::now() > item.second.expiration;
    });
}

bool TextCache::CacheKey::operator==(CacheKey const& key) const
{
    return font == key.font && text == key.text && memcmp(&color, &key.color, sizeof color) == 0;
}

std::size_t TextCache::CacheKey::Hasher::operator()(CacheKey const& k) const
{
    return std::hash<TTF_Font *>()(k.font)
        ^ (std::hash<std::string>()(k.text) << 1)
        ^ k.color.r << 2
        ^ k.color.g << 3
        ^ k.color.b << 4
        ^ k.color.a << 5;
}

} // ps
