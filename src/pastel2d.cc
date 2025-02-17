#include "pastel2d.hh"

namespace ps {

static int CHECK(int f)
{
    if (f != 0)
        throw std::runtime_error(ps_last_error());
    return f;
}

static ps_res_idx_t CHECK_RES(ps_res_idx_t f)
{
    if (f == RES_ERROR)
        throw std::runtime_error(ps_last_error());
    return f;
}

//
// init
//

void init(graphics::Init const& graphics_init) { CHECK(ps_init(&graphics_init)); }
void finalize() { ps_finalize(); }

std::string version() { return ps_version(NULL, NULL, NULL); }

std::tuple<uint8_t, uint8_t, uint8_t> version_number()
{
    uint8_t major, minor, patch;
    ps_version(&major, &minor, &patch);
    return { major, minor, patch };
}

//
// scene
//

class Scene {
};

//
// graphics
//

namespace graphics {

void          set_bg(uint8_t r, uint8_t g, uint8_t b) { CHECK(ps_graphics_set_bg(r, g, b)); }
bool          running() { return ps_graphics_running(); }
void          present() { CHECK(ps_graphics_present()); }
microseconds  timestep() { return microseconds(ps_graphics_timestep_us()); }
void          quit() { ps_graphics_quit(); }
SDL_Window*   window() { return ps_graphics_window(); }
SDL_Renderer* renderer() { return ps_graphics_renderer(); }
// TODO - render scene

}

//
// res
//

namespace res {

static idx_t get_res(ResourceId const& id)
{
    if (auto idx = std::get_if<ps_res_idx_t>(&id))
        return *idx;
    else
        return ps_res_idx(std::get<std::string>(id).c_str());
}

idx_t add_png(uint8_t const* data, size_t sz, Manipulator manipulator, void* manip_data)
{
    if (manipulator)
        return CHECK_RES(ps_res_add_png_manip(data, sz, manipulator, manip_data));
    else
        return CHECK_RES(ps_res_add_png(data, sz));
}

idx_t add_png(std::string const& name, uint8_t const* data, size_t sz, Manipulator manipulator, void* manip_data)
{
    return ps_res_set_name(name.c_str(), add_png(data, sz, manipulator, manip_data));
}

idx_t add_tile(ResourceId const& parent, SDL_FRect const& rect, size_t tile_sz)
{
    return CHECK_RES(ps_res_add_tile(get_res(parent), rect, tile_sz));
}

idx_t add_tile(std::string const& name, ResourceId const& parent, SDL_FRect const& rect, size_t tile_sz)
{
    return ps_res_set_name(name.c_str(), add_tile(parent, rect, tile_sz));
}

void add_tiles(ResourceId const& parent, std::vector<TileDef>& tiles, size_t tile_sz)
{
    CHECK(ps_res_add_tiles(get_res(parent), tiles.data(), tiles.size(), tile_sz));
}

void add_tiles_from_lua(ResourceId const& id, uint8_t const* data, size_t sz)
{
    CHECK(ps_res_add_tiles_from_lua(get_res(id), data, sz));
}

idx_t add_ttf(uint8_t const* data, size_t sz) { return CHECK_RES(ps_res_add_ttf(data, sz)); }
idx_t add_cursor(SDL_Cursor* cursor) { return CHECK_RES(ps_res_add_cursor(cursor)); }
idx_t add_music(uint8_t const* data, size_t sz) { return CHECK_RES(ps_res_add_music(data, sz)); }
idx_t add_sound(uint8_t const* data, size_t sz) { return CHECK_RES(ps_red_add_sound(data, sz)); }

idx_t add_ttf(std::string const& name, uint8_t const* data, size_t sz) { return ps_res_set_name(name.c_str(), add_ttf(data, sz)); }
idx_t add_cursor(std::string const& name, SDL_Cursor* cursor) { return ps_res_set_name(name.c_str(), add_cursor(cursor)); }
idx_t add_music(std::string const& name, uint8_t const* data, size_t sz) { return ps_res_set_name(name.c_str(), add_music(data, sz)); }
idx_t add_sound(std::string const& name, uint8_t const* data, size_t sz) { return ps_res_set_name(name.c_str(), add_sound(data, sz)); }

void  set_name(std::string const& name, idx_t idx) { CHECK(ps_res_set_name(name.c_str(), idx)); }
void  idx(std::string const& name) { CHECK(ps_res_idx(name.c_str())); }

}


namespace manip {

int shadow(uint8_t* pixels, int w, int h, int pitch, void* data)
{
    return ps_manip_shadow(pixels, w, h, pitch, data);
}

}


namespace audio {

void step() { CHECK(ps_audio_step()); }
void choose_music(res::ResourceId const& id) { CHECK(ps_audio_choose_music(res::get_res(id))); }
void play_music(bool play) { CHECK(ps_audio_play_music(play)); }
void play_sound(res::ResourceId const& id) { CHECK(ps_audio_play_sound(res::get_res(id))); }

}


}
