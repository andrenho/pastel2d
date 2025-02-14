#include "textcache.h"

#include <stddef.h>

#include <stb_ds.h>
#include <stb_truetype.h>

#include "graphics.h"
#include "SDL3/SDL.h"

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
    TKey* to_remove = NULL;
    for (int i = 0; i < hmlen(cache); ++i) {
        if (cache[i].value.expiration > SDL_GetTicks()) {
            arrpush(to_remove, cache[i].key);
            SDL_DestroyTexture(cache[i].value.texture);
        }
    }
    for (int i = 0; i < arrlen(to_remove); ++i)
        hmdel(cache, to_remove[i]);
    arrfree(to_remove);
    return 0;
}

static SDL_Texture* create_text_texture(stbtt_fontinfo const* font, const char* text, SDL_Color color)
{
    // TODO
    SDL_Renderer* ren = ps_graphics_renderer();
    SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 50);
    SDL_SetRenderTarget(ren, texture);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderFillRect(ren, NULL);
    SDL_SetRenderTarget(ren, NULL);
    SDL_Log("Texture created: %s", text);
    return texture;
}

SDL_Texture* text_cache_get_texture(stbtt_fontinfo const* font, const char* text, SDL_Color color)
{
    SDL_Texture* tx;

    TKey key = { font, text, color };
    ptrdiff_t i = hmgeti(cache, key);
    if (i == -1) {
        tx = create_text_texture(font, text, color);
        uint64_t expiration = SDL_GetTicks() + (rand() % 10000) + 30000;  // 30 sec
        TValue value = { tx, expiration };
        hmput(cache, key, value);
    } else {
        tx = cache[i].value.texture;
    }
    return tx;
}

int text_cache_finalize()
{
    for (int i = 0; i < hmlen(cache); ++i)
        SDL_DestroyTexture(cache[i].value.texture);
    hmfree(cache);
    return 0;
}
