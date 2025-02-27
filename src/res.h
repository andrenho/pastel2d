#ifndef RES_H
#define RES_H

#include <stdint.h>

#include <SDL3/SDL.h>
#include <sys/types.h>

#define RES_ERROR 0

typedef ssize_t ps_res_idx_t;

typedef struct {
    SDL_Texture* texture;
    SDL_FRect    rect;
} ps_Tile;

typedef struct {
    ps_res_idx_t* idx;
    char*         name;
    SDL_Texture*  texture;
    SDL_FRect     rect;
} ps_TileDef;

typedef int (*ps_Manipulator)(uint8_t* pixels, int w, int h, int pitch, void* data);

ps_res_idx_t ps_res_add_image(uint8_t const* data, size_t sz);
ps_res_idx_t ps_res_add_image_manip(uint8_t const* data, size_t sz, ps_Manipulator manupulator, void* manip_data);

ps_res_idx_t ps_res_add_tile(ps_res_idx_t parent, SDL_FRect rect, size_t tile_sz);
int          ps_res_add_tiles(ps_res_idx_t parent, ps_TileDef* tiles, size_t n_tiles, size_t tile_sz);
int          ps_res_add_tiles_from_lua(ps_res_idx_t parent, uint8_t const* data, size_t sz);

int          ps_res_image_size(ps_res_idx_t idx, int* w, int *h);

ps_res_idx_t ps_res_add_ttf(uint8_t const* data, size_t sz);

ps_res_idx_t ps_res_add_cursor(SDL_Cursor* cursor);

ps_res_idx_t ps_res_add_music(uint8_t const* data, size_t sz);

ps_res_idx_t ps_red_add_sound(uint8_t const* data, size_t sz);

int          ps_res_set_name(const char* name, ps_res_idx_t idx);
ps_res_idx_t ps_res_idx(const char* name);

#define PS_NAME ps_res_set_name
#define PS_IDX ps_res_idx

#endif //RES_H
