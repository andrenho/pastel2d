#ifndef RESOURCEMANAGER_HH
#define RESOURCEMANAGER_HH

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "imagemanipulation.hh"
#include "battery/embed.hpp"
#include "util/noncopyable.hh"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

namespace ps {

struct Tile {
    SDL_Texture* texture;
    int x, y, w, h;
};

using resource_idx_t = size_t;
using Resource = std::variant<SDL_Texture*, Tile, SDL_Cursor*, TTF_Font*>;
using ResourceId = std::variant<std::string, resource_idx_t>;

class ResourceManager : public NonCopyable {
public:
    struct TileDefName {
        std::string name;
        int x, y, w = 1, h = 1;
    };

    struct TileDefIdx {
        resource_idx_t* idx;
        int x, y, w = 1, h = 1;
    };

    struct NameIdx {
        resource_idx_t* idx;
        std::string     name;
    };

    ~ResourceManager();

    resource_idx_t add_texture(std::vector<uint8_t> const& data, ImageManipulation const& manipulation={});
    void           add_texture(std::string const& name, std::vector<uint8_t> const& data, ImageManipulation const& manipulation={});

    template <b::embed_string_literal img_file>
    void           add_texture(std::string const& name, ImageManipulation const& manipulation={}) { add_texture(name, b::embed<img_file>().vec(), manipulation); }
    template <b::embed_string_literal img_file>
    resource_idx_t add_texture(ImageManipulation const& manipulation={}) { return add_texture(b::embed<img_file>().vec(), manipulation); }

    resource_idx_t add_tile(ResourceId const& parent, int tile_size, int x, int y, int w=1, int h=1);
    void           add_tile(std::string const& name, ResourceId const& parent, int x, int y, int w, int h, int tile_size);

    void           add_tiles(ResourceId const& parent, std::vector<TileDefName> const& tiles, int tile_size);
    void           add_tiles(ResourceId const& parent, std::vector<TileDefIdx> const& tiles, int tile_size);
    void           add_tiles(ResourceId const& parent, std::string const& lua_script);

    template <b::embed_string_literal lua_tileset>
    void           add_tiles(ResourceId const& parent) { add_tiles(parent, b::embed<lua_tileset>().str()); }
    template <b::embed_string_literal img_file, b::embed_string_literal lua_tileset>
    void           add_texture_and_tiles() { add_tiles(add_texture<img_file>(), b::embed<lua_tileset>().str()); }

    resource_idx_t add_cursor(SDL_Cursor* cursor);
    void           add_cursor(std::string const& name, SDL_Cursor* cursor);

    void           add_font(std::string const& name, void* data, size_t sz, int font_size);
    resource_idx_t add_font(void* data, size_t sz, int font_size);

    template <b::embed_string_literal font_file>
    void           add_font(std::string const& name, int font_size) { add_font(name, (void *) b::embed<font_file>().data(), b::embed<font_file>().size(), font_size); }
    template <b::embed_string_literal font_file>
    resource_idx_t add_font(int font_size) { return add_font((void *) b::embed<font_file>().data(), b::embed<font_file>().size(), font_size); }

    // create a resource_idx_t out of a name
    resource_idx_t create_idx(std::string const& name, bool remove_name=false);
    void           create_idx(std::vector<NameIdx> const& name_idx, bool remove_names=false);

    void set_renderer(SDL_Renderer* ren) { ren_ = ren; }
    void set_allow_overwrites(bool allow_overwrites) { allow_overwrites_ = allow_overwrites; }

    void cleanup();

    [[nodiscard]] Resource     get(ResourceId const& r) const;
    [[nodiscard]] SDL_Texture* texture(ResourceId const& r) const { return convert_resource<SDL_Texture*>(get(r)); }
    [[nodiscard]] Tile         tile(ResourceId const& r) const    { return convert_resource<Tile>(get(r)); }
    [[nodiscard]] SDL_Cursor*  cursor(ResourceId const& r) const  { return convert_resource<SDL_Cursor*>(get(r)); }
    [[nodiscard]] TTF_Font*    font(ResourceId const& r) const     { return convert_resource<TTF_Font*>(get(r)); }

private:
    SDL_Texture* create_texture(std::vector<uint8_t> const& data, ImageManipulation const& manipulation);

    void check_overwrite(std::string const& name);

    template <typename T>
    T convert_resource(Resource const& res) const {
        if (!std::holds_alternative<T>(res))
            throw std::runtime_error("Resource not of the correct type");
        return std::get<T>(res);
    }

    bool allow_overwrites_ = false;
    SDL_Renderer* ren_ = nullptr;
    std::unordered_map<std::string, Resource> resources_str_;
    std::vector<Resource> resources_idx_;
};

}

#endif //RESOURCEMANAGER_HH
