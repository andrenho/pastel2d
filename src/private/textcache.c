#include "textcache.h"

#include <math.h>
#include <stddef.h>

#include <stb_ds.h>
#include <stb_truetype.h>

#include "../graphics.h"
#include "SDL3/SDL.h"

typedef struct {
    stbtt_fontinfo const* font;
    const char*           text;
    int                   font_size;
    SDL_Color             color;
} TKey;

typedef struct {
    SDL_Texture* texture;
    uint64_t     expiration;
} TValue;

struct {
    size_t key;
    TValue value;
} *cache = NULL;

static size_t key_hash(TKey* key)
{
    char* text = (char *) key->text;
    return (size_t) key->font
        ^ (stbds_hash_string(text, 0) << 1)
        ^ (key->font_size << 2)
        ^ (key->color.r << 3)
        ^ (key->color.b << 4)
        ^ (key->color.b << 5)
        ^ (key->color.a << 6);
}

static SDL_Texture* create_text_texture(stbtt_fontinfo const* font, const char* text, int font_size, SDL_Color color)
{
    static const int MAX_W = 800;
    static const int MAX_H = 300;

    // create text bitmap
    uint8_t bitmap[MAX_W * MAX_H];
    memset(bitmap, 0, sizeof bitmap);

    // get font info
    int ascent, descent, lineGap;
    float scale = stbtt_ScaleForPixelHeight(font, font_size);
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);
    lineGap = roundf(lineGap * scale);

    // draw text
    int x = 0;
    char* p = (char *) text;
    int n_lines = 1;
    do {
        int advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(font, *p, &advance_width, &left_side_bearing);

        // character bounding box
        int bb_x1, bb_y1, bb_x2, bb_y2;
        stbtt_GetCodepointBitmapBox(font, *p, scale, scale, &bb_x1, &bb_y1, &bb_x2, &bb_y2);

        // draw character
        int y = ascent + bb_y1;
        int byte_offset = x + roundf(left_side_bearing * scale) + (y * MAX_W);
        stbtt_MakeCodepointBitmap(font, bitmap + byte_offset, bb_x2 - bb_x1, bb_y2 - bb_y1, MAX_W, scale, scale, *p);

        // advance space
        int kern = stbtt_GetCodepointKernAdvance(font, *p, *(p + 1));
        x += (advance_width + kern) * scale;
    } while (*(++p) != '\0');

    // convert to pixel data using the requested colors
    int w = MAX_W;
    int h = n_lines * (ascent - descent + lineGap);
    int sz_w = 0;
    uint8_t pixels[w * h * 4];
    memset(pixels, 255, sizeof pixels);
    for (x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            pixels[(x + y * w) * 4] = color.r;
            pixels[(x + y * w) * 4 + 1] = color.g;
            pixels[(x + y * w) * 4 + 2] = color.b;

            uint8_t a = bitmap[x + (y * MAX_W)];
            pixels[(x + y * w) * 4 + 3] = a;
            if (a)
                sz_w = x > sz_w ? x : sz_w;
        }
    }

    SDL_Surface* sf = SDL_CreateSurfaceFrom(sz_w + 1, h, SDL_PIXELFORMAT_RGBA32, pixels, MAX_W * 4);
    SDL_Texture* tx = SDL_CreateTextureFromSurface(ps_graphics_renderer(), sf);
    SDL_SetTextureScaleMode(tx, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(sf);

    return tx;
}

int text_cache_cleanup()
{
    size_t* to_remove = NULL;
    for (int i = 0; i < hmlen(cache); ++i) {
        if (SDL_GetTicks() > cache[i].value.expiration) {
            arrpush(to_remove, cache[i].key);
            SDL_DestroyTexture(cache[i].value.texture);
        }
    }
    for (int i = 0; i < arrlen(to_remove); ++i)
        hmdel(cache, to_remove[i]);
    arrfree(to_remove);
    return 0;
}

SDL_Texture* text_cache_get_texture(stbtt_fontinfo const* font, const char* text, int font_size, SDL_Color color)
{
    SDL_Texture* tx;

    TKey key = { font, text, font_size, color };
    ptrdiff_t i = hmgeti(cache, key_hash(&key));
    if (i == -1) {
        tx = create_text_texture(font, text, font_size, color);
        uint64_t expiration = SDL_GetTicks() + (rand() % 10000) + 30000;  // 30 sec
        TValue value = { tx, expiration };
        hmput(cache, key_hash(&key), value);
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
