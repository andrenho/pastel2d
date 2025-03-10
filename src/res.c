#include "res.h"

#include <SDL3/SDL.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stb_ds.h>

#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"   // prevent warning in library
#define POCKETMOD_IMPLEMENTATION
#include <pocketmod.h>
#pragma GCC diagnostic pop

#include <pl_log.h>

#include "private/res_priv.h"

#include "graphics.h"

static ps_Resource* resources;

struct {
    char*          key;
    ps_res_idx_t value;
} *resource_names = NULL;

int ps_res_init()
{
    arrpush(resources, (ps_Resource) {});  // index 0 is always an error
    return 0;
}

ps_res_idx_t ps_res_add_image(uint8_t const* data, size_t sz)
{
    return ps_res_add_image_manip(data, sz, NULL, NULL);
}

ps_res_idx_t ps_res_add_image_manip(uint8_t const* data, size_t sz, ps_Manipulator manupulator, void* manip_data)
{
    int w, h, channels;
    stbi_uc *img = stbi_load_from_memory(data, sz, &w, &h, &channels, 0);
    if (img == NULL)
        PL_ERROR_RET(RES_ERROR, "Could not load image.");

    if (channels != 3 && channels != 4)
        PL_ERROR_RET(RES_ERROR, "Only images with 3 or 4 channels are supported by now (sorry).");

    if (manupulator && manupulator(img, w, h, w * channels, manip_data) != 0) {
        return RES_ERROR;
    }

    SDL_Surface* sf = SDL_CreateSurfaceFrom(w, h, channels == 3 ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_RGBA32, img, w * channels);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(ps_graphics_renderer(), sf);
    SDL_DestroySurface(sf);
    stbi_image_free(img);

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    ps_Resource res = {
        .type = RT_TEXTURE,
        .texture = texture,
    };
    arrpush(resources, res);
    return arrlen(resources) - 1;
}

ps_res_idx_t ps_res_add_tile(ps_res_idx_t parent, SDL_FRect rect, size_t tile_sz)
{
    SDL_Texture* tx = ps_res_get(parent, RT_TEXTURE)->texture;
    if (tx == NULL)
        return RES_ERROR;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    if (rect.w == 0.f)
        rect.w = rect.h = 1;
#pragma GCC diagnostic pop

    ps_Resource res = {
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

int ps_res_add_tiles(ps_res_idx_t parent, ps_TileDef* tiles, size_t n_tiles, size_t tile_sz)
{
    for (size_t i = 0; i < n_tiles; ++i) {
        ps_res_idx_t idx = ps_res_add_tile(parent, tiles[i].rect, tile_sz);
        if (idx == RES_ERROR)
            return RES_ERROR;
        if (tiles[i].idx)
            *tiles[i].idx = idx;
        if (tiles[i].name)
            ps_res_set_name(tiles[i].name, idx);
    }
    return 0;
}

int ps_res_add_tiles_from_lua(ps_res_idx_t parent, uint8_t const* data, size_t sz)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    static const char* ERR_MSG = "Error loading Lua tileset:";
#define CHECK(check, msg) \
    if (!(check)) { \
        PL_ERROR_RET(RES_ERROR, "%s %s", ERR_MSG, msg ? msg : lua_tostring(L, -1)); \
        lua_close(L); \
        return RES_ERROR; \
    }

    CHECK(luaL_loadbuffer(L, (const char *) data, sz, "tile_loader") == LUA_OK, NULL);
    CHECK(lua_pcall(L, 0, LUA_MULTRET, 0) == LUA_OK, NULL);
    CHECK(lua_istable(L, -1), "Script should return a table.");

    lua_getfield(L, -1, "tile_size");
    CHECK(lua_isnumber(L, -1), "Expected field `tile_size` with a number.");
    int tile_size = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "tiles");
    CHECK(lua_istable(L, -1), "Expected table `tiles`.");

    ps_TileDef* tiles = NULL;

    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        ps_TileDef tile = {
            .name = strdup(lua_tostring(L, -2)),
        };

        CHECK(lua_istable(L, -1), "Expected a table for key");
        size_t len = lua_objlen(L, -1);

        CHECK((len == 2) || (len == 4), "Expected a table with 2 or 4 items for key");
        lua_rawgeti(L, -1, 1); tile.rect.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, -1, 2); tile.rect.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        if (len == 4) {
            lua_rawgeti(L, -1, 3); tile.rect.w = luaL_checkinteger(L, -1); lua_pop(L, 1);
            lua_rawgeti(L, -1, 4); tile.rect.h = luaL_checkinteger(L, -1); lua_pop(L, 1);
        }

        lua_pop(L, 1);
        arrpush(tiles, tile);
    }

    lua_close(L);

    ps_res_add_tiles(parent, tiles, arrlen(tiles), tile_size);

    for (size_t j = 0; j < (size_t) arrlen(tiles); ++j)
        free(tiles[j].name);
    arrfree(tiles);

    return 0;
}

int ps_res_image_size(ps_res_idx_t idx, int* w, int* h)
{
    switch (resources[idx].type) {
        case RT_TEXTURE: {
            float fw, fh;
            SDL_GetTextureSize(resources[idx].texture, &fw, &fh);
            *w = (int) fw;
            *h = (int) fh;
            break;
        }
        case RT_TILE:
            *w = (int) resources[idx].tile.rect.w;
            *h = (int) resources[idx].tile.rect.h;
            break;
        case RT_FONT:
        case RT_CURSOR:
        case RT_MUSIC:
        case RT_SOUND:
        default:
            PL_ERROR_RET(-1, "Not a valid image.");
    }

    return 0;
}

ps_res_idx_t ps_res_add_ttf(uint8_t const* data, size_t sz)
{
    ps_Resource resource = {
        .type = RT_FONT,
        .font = malloc(sizeof(stbtt_fontinfo)),
    };
    if (!stbtt_InitFont(resource.font, data, 0))
        PL_ERROR_RET(RES_ERROR, "Could not load TTF file.");
    arrpush(resources, resource);
    return arrlen(resources) - 1;
}

ps_res_idx_t ps_res_add_cursor(SDL_Cursor* cursor)
{
    ps_Resource resource = {
        .type = RT_CURSOR,
        .cursor = cursor,
    };
    arrpush(resources, resource);
    return arrlen(resources) - 1;
}

ps_res_idx_t ps_res_add_music(uint8_t const* data, size_t sz)
{
    ps_Resource res = {
        .type = RT_MUSIC,
        .music = malloc(sizeof(pocketmod_context))
    };
    if (pocketmod_init(res.music, data, sz, 44100) == 0)
        PL_ERROR_RET(RES_ERROR, "Could not load MOD file.");
    arrpush(resources, res);
    return arrlen(resources) - 1;
}

ps_res_idx_t ps_red_add_sound(uint8_t const* data, size_t sz)
{
    ps_Resource res = {
        .type = RT_SOUND,
    };
    SDL_IOStream* io = SDL_IOFromConstMem(data, sz);
    if (!SDL_LoadWAV_IO(io, true, &res.sound.spec, &res.sound.data, &res.sound.sz))
        PL_ERROR_RET(RES_ERROR, "Could not load WAV file.");

    extern SDL_AudioStream* ps_audio_create_stream(SDL_AudioSpec* spec);
    res.sound.stream = ps_audio_create_stream(&res.sound.spec);

    arrpush(resources, res);
    return arrlen(resources) - 1;
}

ps_Resource const* ps_res_get(ps_res_idx_t idx, ps_ResourceType validate_resource_type)
{
    if (resources[idx].type != validate_resource_type)
        PL_ERROR_RET(RES_ERROR, "Resource requested is not the correct type.");
    return &resources[idx];
}

ps_ResourceType ps_res_get_type(ps_res_idx_t idx)
{
    return resources[idx].type;
}

int ps_res_set_name(const char* name, ps_res_idx_t idx)
{
    if (idx == RES_ERROR)
        return RES_ERROR;

    ptrdiff_t i = shgeti(resource_names, name);
    if (i != -1)
        PL_ERROR_RET(RES_ERROR, "Name '%s' already in use.", name);
    shput(resource_names, strdup(name), idx);
    return 0;
}

ps_res_idx_t ps_res_idx(const char* name)
{
    ptrdiff_t i = shgeti(resource_names, name);
    if (i == -1)
        PL_ERROR_RET(RES_ERROR, "Resource '%s' not found.", name);
    return resource_names[i].value;
}

int ps_res_finalize()
{
    for (int i = 0; i < arrlen(resources); ++i) {
        switch (resources[i].type) {
            case RT_TEXTURE:
                SDL_DestroyTexture(resources[i].texture);
                break;
            case RT_CURSOR:
                SDL_DestroyCursor(resources[i].cursor);
                break;
            case RT_TILE:
                break;
            case RT_FONT:
                free(resources[i].font);
                break;
            case RT_MUSIC:
                free(resources[i].music);
                break;
            case RT_SOUND:
                SDL_DestroyAudioStream(resources[i].sound.stream);
                free(resources[i].sound.data);
                break;
        }
    }
    arrfree(resources);

    for (int i = 0; i < shlen(resource_names); ++i)
        free(resource_names[i].key);

    shfree(resource_names);

    return 0;
}

SDL_Texture* ps_res_get_texture(ps_res_idx_t idx)
{
    if (resources[idx].type != RT_TEXTURE)
        PL_ERROR_RET(NULL, "Resource '%zu' not a texture.", idx);
    return resources[idx].texture;
}

ps_Tile ps_res_get_tile(ps_res_idx_t idx)
{
    if (resources[idx].type != RT_TILE) {
        ps_Tile empty = { NULL, (SDL_FRect) { 0, 0, 0, 0 } };
        PL_ERROR_RET(empty, "Resource '%zu' not a tile.", idx);
    }
    return resources[idx].tile;
}
