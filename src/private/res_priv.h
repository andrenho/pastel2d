#ifndef RES_PRIV_H
#define RES_PRIV_H

#include "res.h"

#include <SDL3/SDL.h>

typedef enum { RT_TEXTURE, RT_TILE, RT_FONT, RT_CURSOR, RT_MUSIC, RT_SOUND } ps_ResourceType;

typedef struct stbtt_fontinfo stbtt_fontinfo;
typedef struct pocketmod_context pocketmod_context;

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
        ps_Tile            tile;
        stbtt_fontinfo*    font;
        SDL_Cursor*        cursor;
        pocketmod_context* music;
        ps_SoundEffect     sound;
    };
} ps_Resource;

int ps_res_init();
int ps_res_finalize();

ps_Resource const* ps_res_get(ps_res_idx_t idx, ps_ResourceType validate_resource_type);
ps_ResourceType    ps_res_get_type(ps_res_idx_t idx);

#endif //RES_PRIV_H
