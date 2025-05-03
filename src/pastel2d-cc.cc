#include "pastel2d.hh"

#include <pl_log.h>


namespace ps {

static int CHECK(int f)
{
    if (f != 0)
        throw std::runtime_error(pl_last_error());
    return f;
}

static ps_res_idx_t CHECK_RES(ps_res_idx_t f)
{
    if (f == RES_ERROR)
        throw std::runtime_error(pl_last_error());
    return f;
}

//
// init
//

void init(graphics::Init const& graphics_init) { CHECK(ps_init(&graphics_init)); }
void finalize() { ps_finalize(); }

std::string version() { return ps_version(); }

//
// context
//

ps_Context Context::context_c() const
{
    return ps_Context {
        .position = position,
        .align = (ps_Alignment) align,
        .rotation = rotation,
        .zoom = zoom,
        .opacity = opacity,
        .rotation_center = rotation_center ? *rotation_center : SDL_FPoint { DEFAULT_ROT_CENTER, DEFAULT_ROT_CENTER },
        .draw_border = draw_border,
    };
}

void Scene::push_context(Context const& ctx)  { CHECK(ps_scene_push_context(&scene_, ctx.context_c())); }
void Scene::pop_context() { ps_scene_pop_context(&scene_); }

namespace res { static idx_t get_res(ResourceId const& id); }



void Scene::add_image(res::ResourceId const& id, SDL_Point const& p, Alignment align)
{
    CHECK(ps_scene_add_image(&scene_, res::get_res(id), p, (ps_Alignment) align, nullptr));
}

void Scene::add_image(res::ResourceId const& id, SDL_Point const& p, Alignment align, Context const& ctx)
{
    ps_Context context = ctx.context_c();
    CHECK(ps_scene_add_image(&scene_, res::get_res(id), p, (ps_Alignment) align, &context));
}

void Scene::add_text(res::ResourceId const& id, std::string const& text, SDL_Point const& p, Alignment align, int font_size, SDL_Color const& color)
{
    CHECK(ps_scene_add_text(&scene_, res::get_res(id), text.c_str(), p, (ps_Alignment) align, font_size, color, nullptr));
}

void Scene::add_text(res::ResourceId const& id, std::string const& text, SDL_Point const& p, Alignment align, int font_size, SDL_Color const& color, Context const& ctx)
{
    ps_Context context = ctx.context_c();
    CHECK(ps_scene_add_text(&scene_, res::get_res(id), text.c_str(), p, (ps_Alignment) align, font_size, color, &context));
}

void Scene::set_z_order(int z)
{
    scene_.z_order = z;
}

//
// scene
//

Scene::Scene()
{
    ps_scene_init(&scene_);
}

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

void render_scenes(std::vector<Scene> const& scenes)
{
    std::vector<ps_Scene> scenes_cptr(scenes.size());
    for (size_t i = 0; i < scenes.size(); ++i)
        memcpy(&scenes_cptr[i], &scenes.at(i).scene(), sizeof(ps_Scene));
    ps_graphics_render_scenes(scenes_cptr.data(), scenes_cptr.size());
}

void set_window_title(std::string const& title)
{
    static char buf[1024];
    snprintf(buf, sizeof buf, "%s", title.c_str());
    ps_graphics_set_window_title("%s", buf);
}

int fps()
{
    return ps_graphics_fps();
}

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

idx_t add_image(uint8_t const* data, size_t sz, Manipulator manipulator, void* manip_data)
{
    if (manipulator)
        return CHECK_RES(ps_res_add_image_manip(data, sz, manipulator, manip_data));
    else
        return CHECK_RES(ps_res_add_image(data, sz));
}

idx_t add_image(std::string const& name, uint8_t const* data, size_t sz, Manipulator manipulator, void* manip_data)
{
    return ps_res_set_name(name.c_str(), add_image(data, sz, manipulator, manip_data));
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
idx_t idx(std::string const& name) { return CHECK_RES(ps_res_idx(name.c_str())); }

std::pair<int, int> image_size(ResourceId res_id) {
    int w, h;
    ps_res_image_size(get_res(res_id), &w, &h);
    return { w, h };
}

Tile tile(ResourceId res_id)
{
    ps_Tile tile = ps_res_get_tile(get_res(res_id));
    if (tile.texture == nullptr)
        throw std::runtime_error(pl_last_error());
    return tile;
}


}

//
// manip
//

namespace manip {

int shadow(uint8_t* pixels, int w, int h, int pitch, void* data)
{
    return ps_manip_shadow(pixels, w, h, pitch, data);
}

}

//
// audio
//

namespace audio {

void step() { CHECK(ps_audio_step()); }
void choose_music(res::ResourceId const& id) { CHECK(ps_audio_choose_music(res::get_res(id))); }
void play_music(bool play) { CHECK(ps_audio_play_music(play)); }
void play_sound(res::ResourceId const& id) { CHECK(ps_audio_play_sound(res::get_res(id))); }

}


}
