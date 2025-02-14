#include "textcache.h"

#include <stddef.h>

#include <stb_ds.h>
#include <stb_truetype.h>

typedef struct {
    stbtt_fontinfo const* font;
    const char* text;
    SDL_Color color;
} TKey;

typedef struct {
    SDL_Texture* texture;
    uint64_t     expiration;
} TValue;

struct {
    TKey   key;
    TValue value;
} *cache = NULL;

int text_cache_cleanup()
{
    // TODO
    return 0;
}

static SDL_Texture* create_text_texture(stbtt_fontinfo const* font, const char* text, SDL_Color color)
{
    return NULL;  // TODO
}

SDL_Texture* text_cache_get_texture(stbtt_fontinfo const* font, const char* text, SDL_Color color)
{
    SDL_Texture* tx;

    TKey key = { font, text, color };
    ptrdiff_t i = hmgeti(cache, key);
    if (i == -1) {
        tx = create_text_texture(font, text, color);
        uint64_t expiration = 0;  // TODO
        TValue value = { tx, expiration };
        hmput(cache, key, value);
    } else {
        tx = cache[i].value.texture;
    }
    return tx;
}

int text_cache_finalize()
{
    // TODO - cleanup textures
    hmfree(cache);
    return 0;
}
