#include "res.h"

#include <SDL3/SDL.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "error.h"
#include "graphics.h"
extern char last_error[LAST_ERROR_SZ];

typedef struct {
    ResourceType type;
    union {
        SDL_Texture* texture;
        Tile         tile;
    };
} Resource;

static Resource* resources;

struct {
    char*          key;
    resource_idx_t value;
} *resource_names = NULL;

resource_idx_t ps_res_add_png(uint8_t const* data, size_t sz)
{
    int w, h, bpp;
    stbi_uc *img = stbi_load_from_memory(data, sz, &w, &h, &bpp, STBI_rgb_alpha);
    if (img == NULL) {
        snprintf(last_error, sizeof last_error, "Could not load image.");
        return RES_ERROR;
    }
    if (bpp != 4) {
        snprintf(last_error, sizeof last_error, "Only 4-bit images supported by now (sorry).");
        stbi_image_free(img);
        return RES_ERROR;
    }

    SDL_Surface* sf = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, img, w * 4);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(ps_graphics_renderer(), sf);
    SDL_DestroySurface(sf);
    stbi_image_free(img);

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    Resource res = {
        .type = RT_TEXTURE,
        .texture = texture,
    };
    arrpush(resources, res);
    return arrlen(resources) - 1;
}

resource_idx_t ps_res_add_tile(resource_idx_t parent, SDL_FRect rect, size_t tile_sz)
{
    SDL_Texture* tx = ps_res_get_texture(parent);
    if (tx == NULL)
        return RES_ERROR;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    if (rect.w == 0.f)
        rect.w = rect.h = 1;
#pragma GCC diagnostic pop

    Resource res = {
        .type = RT_TILE,
        .tile = {
            .texture = tx,
            .rect = {
                .x = rect.x * tile_sz,
                .y = rect.y * tile_sz,
                .w = rect.w * tile_sz,
                .h = rect.h * tile_sz,
            },
        },
    };
    arrpush(resources, res);
    return arrlen(resources) - 1;
}

int ps_res_add_tiles(resource_idx_t parent, TileDef* tiles, size_t n_tiles, size_t tile_sz)
{
    for (size_t i = 0; i < n_tiles; ++i) {
        resource_idx_t idx = ps_res_add_tile(parent, tiles[i].rect, tile_sz);
        if (*tiles->idx)
            *tiles->idx = idx;
        if (tiles->name)
            ps_res_name_idx(tiles->name, idx);
    }
    return 0;
}

int ps_res_add_tiles_from_lua(resource_idx_t parent, uint8_t const* data, size_t sz)
{
    return -1;  // TODO
}

int ps_res_name_idx(const char* name, resource_idx_t idx)
{
    ptrdiff_t i = shgeti(resource_names, name);
    if (i != -1) {
        snprintf(last_error, sizeof last_error, "Name '%s' already in use.", name);
        return RES_ERROR;
    }
    shput(resource_names, name, idx);
    return 0;
}

resource_idx_t ps_res_idx(const char* name)
{
    ptrdiff_t i = shgeti(resource_names, name);
    if (i == -1) {
        snprintf(last_error, sizeof last_error, "Resource '%s' not found.", name);
        return RES_ERROR;
    }
    return resource_names[i].value;
}

ResourceType ps_res_get_type(resource_idx_t idx)
{
    return resources[idx].type;
}

SDL_Texture* ps_res_get_texture(resource_idx_t idx)
{
    if (resources[idx].type != RT_TEXTURE) {
        snprintf(last_error, sizeof last_error, "Invalid resource (not a texture)");
        return NULL;
    }
    return resources[idx].texture;
}

Tile const* ps_res_get_tile(resource_idx_t idx)
{
    if (resources[idx].type != RT_TILE) {
        snprintf(last_error, sizeof last_error, "Invalid resource (not a tile)");
        return NULL;
    }
    return &resources[idx].tile;
}

void ps_res_finalize()
{
    for (size_t i = 0; i < (size_t) arrlen(resources); ++i) {
        switch (resources[i].type) {
            case RT_TEXTURE:
                SDL_DestroyTexture(resources[i].texture);
                break;
            case RT_TILE:
                break;
        }
    }

    shfree(resource_names);
    arrfree(resources);
}

