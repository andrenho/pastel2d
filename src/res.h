#ifndef RES_H
#define RES_H

#include <stdint.h>
#include <sys/types.h>

#include <SDL3/SDL.h>

#define RES_ERROR -1

typedef ssize_t resource_idx_t;

typedef struct {
    SDL_Texture* texture;
    SDL_FRect    rect;
} Tile;

typedef struct {
    resource_idx_t* idx;
    char*           name;
    SDL_Texture*    texture;
    SDL_FRect       rect;
} TileDef;

typedef enum { RT_TEXTURE, RT_TILE, RT_FONT, RT_CURSOR, RT_AUDIO_MOD } ResourceType;

typedef int (*Manipulator)(uint8_t* pixels, int w, int h, int pitch, void* data);

typedef struct stbtt_fontinfo stbtt_fontinfo;
typedef struct pocketmod_context pocketmod_context;

resource_idx_t ps_res_add_png(uint8_t const* data, size_t sz);
resource_idx_t ps_res_add_png_manip(uint8_t const* data, size_t sz, Manipulator manupulator, void* manip_data);

resource_idx_t ps_res_add_tile(resource_idx_t parent, SDL_FRect rect, size_t tile_sz);
int            ps_res_add_tiles(resource_idx_t parent, TileDef* tiles, size_t n_tiles, size_t tile_sz);
int            ps_res_add_tiles_from_lua(resource_idx_t parent, uint8_t const* data, size_t sz);

resource_idx_t ps_res_add_ttf(uint8_t const* data, size_t sz);

resource_idx_t ps_res_add_cursor(SDL_Cursor* cursor);

resource_idx_t ps_res_add_audio_mod(uint8_t const* data, size_t sz, int rate);

int            ps_res_set_name(const char* name, resource_idx_t idx);
resource_idx_t ps_res_idx(const char* name);

int ps_res_finalize();

typedef struct {
    ResourceType type;
    union {
        SDL_Texture*       texture;
        Tile               tile;
        stbtt_fontinfo*    font;
        SDL_Cursor*        cursor;
        pocketmod_context* mod;
    };
} Resource;

Resource const* ps_res_get(resource_idx_t idx);

#define NAME ps_res_set_name
#define IDX ps_res_idx

#endif //RES_H
