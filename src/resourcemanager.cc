#include "resourcemanager.hh"

#include <algorithm>
#include <optional>
#include <string>
using namespace std::string_literals;

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <lua.hpp>

namespace ps {

ResourceManager::~ResourceManager()
{
    cleanup();
}

SDL_Texture* ResourceManager::create_texture(std::vector<uint8_t> const& data)
{
    SDL_Surface* sf = IMG_Load_RW(SDL_RWFromMem((void *) data.data(), (int) data.size()), 1);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren_, sf);
    SDL_FreeSurface(sf);
    return texture;
}

resource_idx_t ResourceManager::add_texture(std::vector<uint8_t> const& data)
{
    resources_idx_.emplace_back(create_texture(data));
    return resources_idx_.size() - 1;
}

void ResourceManager::check_overwrite(std::string const& name)
{
    if (!allow_overwrites_) {
        auto it = resources_str_.find(name);
        if (it != resources_str_.end())
            throw std::runtime_error("There's already a resource called `" + name + "`");
    }
}

void ResourceManager::add_texture(std::string const& name, std::vector<uint8_t> const& data)
{
    check_overwrite(name);
    resources_str_.emplace(name, create_texture(data));
}

resource_idx_t ResourceManager::add_tile(ResourceId const& parent, int tile_size, int x, int y, int w, int h)
{
    resources_idx_.emplace_back(Tile { this->texture(parent), x * tile_size, y * tile_size, w * tile_size, h * tile_size });
    return resources_idx_.size() - 1;
}

void ResourceManager::add_tile(ResourceId const& parent, std::string const& name, int x, int y, int w, int h, int tile_size)
{
    check_overwrite(name);

    if (h == 0)
        h = w;

    resources_str_.emplace(name, Tile {
        .texture = this->texture(parent), .x = x * tile_size, .y = y * tile_size, .w = w * tile_size, .h = h * tile_size,
    });
}


void ResourceManager::add_tiles(ResourceId const& parent_resource, std::vector<TileDefName> const& tiles, int tile_size)
{
    for (auto const& tile: tiles)
        add_tile(parent_resource, tile.name, tile.x, tile.y, tile.w, tile.h, tile_size);
}

void ResourceManager::add_tiles(ResourceId const& parent_resource, std::vector<TileDefIdx> const& tiles, int tile_size)
{
    for (auto const& tile: tiles)
        *tile.idx = add_tile(parent_resource, tile_size, tile.x, tile.y, tile.w, tile.h);
}

void ResourceManager::add_tiles(ResourceId const& parent, std::string const& lua_script)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    auto CHECK = [&L](bool check, std::optional<std::string> const& msg={}) {
        if (!check) {
            std::string error = msg.value_or(lua_tostring(L, -1));
            lua_close(L);
            throw std::runtime_error("Error loading lua tileset: "s + error);
        }
    };

    CHECK(luaL_dostring(L, lua_script.c_str()) == LUA_OK);
    CHECK(lua_istable(L, -1), "Script should return a table.");

    lua_getfield(L, -1, "tile_size");
    CHECK(lua_isnumber(L, -1), "Expected field `tile_size` with a number.");
    int tile_size = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "tiles");
    CHECK(lua_istable(L, -1), "Expected table `tiles`.");
    lua_pushnil(L);

    std::vector<TileDefName> tiles;
    while (lua_next(L, -2) != 0) {
        TileDefName& tile = tiles.emplace_back();
        tile.name = lua_tostring(L, -2);

        CHECK(lua_istable(L, -1), "Expected a table for key `"s + tile.name + "`");
        size_t len = lua_objlen(L, -1);

        CHECK(len == 2 || len == 4, "Expected a table with 2 or 4 items for key `"s + tile.name + "`");
        lua_rawgeti(L, -1, 1); tile.x = luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_rawgeti(L, -1, 2); tile.y = luaL_checkinteger(L, -1); lua_pop(L, 1);
        if (len == 4) {
            lua_rawgeti(L, -1, 3); tile.w = luaL_checkinteger(L, -1); lua_pop(L, 1);
            lua_rawgeti(L, -1, 4); tile.h = luaL_checkinteger(L, -1); lua_pop(L, 1);
        }

        lua_pop(L, 1);
    }

    lua_close(L);

    add_tiles(parent, tiles, tile_size);
}

resource_idx_t ResourceManager::add_cursor(SDL_Cursor* cursor)
{
    resources_idx_.emplace_back(cursor);
    return resources_idx_.size() - 1;
}

void ResourceManager::add_cursor(std::string const& name, SDL_Cursor* cursor)
{
    check_overwrite(name);
    resources_str_.emplace(name, cursor);
}

void ResourceManager::add_font(std::string const& name, std::vector<uint8_t> const& data, int font_size)
{
    check_overwrite(name);
    resources_str_.emplace(name, TTF_OpenFontRW(SDL_RWFromMem((void *) data.data(), data.size()), 1, font_size));
}

resource_idx_t ResourceManager::add_font(std::vector<uint8_t> const& data, int font_size)
{
    resources_idx_.emplace_back(TTF_OpenFontRW(SDL_RWFromMem((void *) data.data(), data.size()), 1, font_size));
    return resources_idx_.size() - 1;
}

void ResourceManager::cleanup()
{
    std::vector<Resource> resources { resources_idx_.begin(), resources_idx_.end() };
    resources.reserve(resources.size() + resources_str_.size());
    std::transform(resources_str_.begin(), resources_str_.end(), std::back_inserter(resources),
        [](const auto& r) { return r.second; });

    for (auto const& res: resources) {
        if (auto t = std::get_if<SDL_Texture*>(&res); t)
            SDL_DestroyTexture(*t);
        else if (auto c = std::get_if<SDL_Cursor*>(&res); c)
            SDL_FreeCursor(*c);
        else if (auto f = std::get_if<TTF_Font*>(&res); f)
            TTF_CloseFont(*f);
    }
    resources_str_.clear();
}

Resource ResourceManager::get(ResourceId const& resource) const
{
    if (auto r = std::get_if<resource_idx_t>(&resource))
        return resources_idx_.at(*r);

    if (auto s = std::get_if<std::string>(&resource)) {
        auto it = resources_str_.find(*s);
        if (it == resources_str_.end())
            throw std::runtime_error("Resource " + *s + " not found");
        return it->second;
    }

    throw std::runtime_error("Invalid resource type.");
}

resource_idx_t ResourceManager::create_idx(std::string const& name, bool remove_name)
{
    auto it = resources_str_.find(name);
    if (it == resources_str_.end())
        throw std::runtime_error("Resource " + name + " not found");

    resources_idx_.push_back(it->second);
    if (remove_name)
        resources_str_.erase(name);
    return resources_idx_.size() - 1;
}

void ResourceManager::create_idx(std::vector<NameIdx> const& name_idx, bool remove_names)
{
    for (auto const& n: name_idx)
        *n.idx = create_idx(n.name, remove_names);
}

ResourceManager& res()
{
    static ResourceManager res;
    return res;
}

}