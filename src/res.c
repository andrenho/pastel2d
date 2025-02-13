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

typedef enum { RT_TEXTURE } ResourceType;

typedef struct {
    ResourceType type;
    union {
        SDL_Texture* texture;
    };
} Resource;

static Resource* resources;

resource_idx_t ps_res_add_png(uint8_t const* data, size_t sz)
{
    int w, h;
    stbi_uc *img = stbi_load_from_memory(data, sz, &w, &h, NULL, 0);
    if (img == NULL) {
        snprintf(last_error, sizeof last_error, "Could not load image.");
        return RES_ERROR;
    }

    // TODO - process image
    SDL_Texture* texture = SDL_CreateTexture(ps_graphics_renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, w, h);

    Resource res = {
        .type = RT_TEXTURE,
        .texture = texture,
    };
    arrpush(resources, res);
    return arrlen(resources) - 1;
}

void ps_res_finalize()
{
    for (size_t i = 0; i < arrlen(resources); ++i) {
        switch (resources[i].type) {
            case RT_TEXTURE:
                SDL_DestroyTexture(resources[i].texture);
                break;
        }
    }

    arrfree(resources);
}

SDL_Texture* ps_res_get_texture(resource_idx_t res_id)
{
    // TODO - check for type
    return resources[res_id].texture;
}