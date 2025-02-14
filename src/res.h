#ifndef RES_H
#define RES_H

#include <stddef.h>
#include <stdint.h>

#include <SDL3/SDL.h>

#define RES_ERROR -1

typedef ssize_t resource_idx_t;

typedef struct {
    SDL_Texture* texture;
    SDL_Rect     rect;
} Tile;

typedef struct {
    resource_idx_t* idx;
    const char*     name;
    SDL_Texture* texture;
    SDL_Rect     rect;
} TileDef;

resource_idx_t ps_res_add_png(uint8_t const* data, size_t sz);

resource_idx_t ps_res_add_tile(resource_idx_t parent, SDL_Rect rect, size_t tile_sz);
int            ps_res_add_tiles(resource_idx_t parent, TileDef* tiles, size_t n_tiles, size_t tile_sz);
int            ps_res_add_tiles_from_lua(resource_idx_t parent, uint8_t const* data, size_t sz);

int            ps_res_name_idx(const char* name, resource_idx_t idx);
resource_idx_t ps_res_idx(const char* name);

SDL_Texture*   ps_res_get_texture(resource_idx_t res_id);

void ps_res_finalize();

#endif //RES_H
