#ifndef RES_H
#define RES_H

#include <stdint.h>
#include <sys/types.h>

#include <SDL3/SDL.h>

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

typedef enum { RT_TEXTURE, RT_TILE, RT_FONT, RT_CURSOR, RT_MUSIC, RT_SOUND } ps_ResourceType;

typedef int (*ps_Manipulator)(uint8_t* pixels, int w, int h, int pitch, void* data);

typedef struct stbtt_fontinfo stbtt_fontinfo;
typedef struct pocketmod_context pocketmod_context;

int ps_res_init();
int ps_res_finalize();

ps_res_idx_t ps_res_add_png(uint8_t const* data, size_t sz);
ps_res_idx_t ps_res_add_png_manip(uint8_t const* data, size_t sz, ps_Manipulator manupulator, void* manip_data);

ps_res_idx_t ps_res_add_tile(ps_res_idx_t parent, SDL_FRect rect, size_t tile_sz);
int            ps_res_add_tiles(ps_res_idx_t parent, ps_TileDef* tiles, size_t n_tiles, size_t tile_sz);
int            ps_res_add_tiles_from_lua(ps_res_idx_t parent, uint8_t const* data, size_t sz);

ps_res_idx_t ps_res_add_ttf(uint8_t const* data, size_t sz);

ps_res_idx_t ps_res_add_cursor(SDL_Cursor* cursor);

ps_res_idx_t ps_res_add_music(uint8_t const* data, size_t sz, int rate);

ps_res_idx_t ps_red_add_sound(uint8_t const* data, size_t sz);

int            ps_res_set_name(const char* name, ps_res_idx_t idx);
ps_res_idx_t ps_res_idx(const char* name);

typedef struct {
    SDL_AudioSpec    spec;
    uint8_t*         data;
    uint32_t         sz;
    SDL_AudioStream* stream;
} ps_SoundEffect;

typedef struct {
    ps_ResourceType type;
    union {
        SDL_Texture*       texture;
        ps_Tile               tile;
        stbtt_fontinfo*    font;
        SDL_Cursor*        cursor;
        pocketmod_context* music;
        ps_SoundEffect        sound;
    };
} ps_Resource;

ps_Resource const* ps_res_get(ps_res_idx_t idx, ps_ResourceType validate_resource_type);
ps_ResourceType    ps_res_get_type(ps_res_idx_t idx);

#define PS_NAME ps_res_set_name
#define PS_IDX ps_res_idx

#endif //RES_H
