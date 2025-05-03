#ifndef PASTEL2D_CC_HH
#define PASTEL2D_CC_HH

#include <cstdint>

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>
#include <tuple>
#include <variant>

extern "C" {
#include "pastel2d.h"
}

namespace ps {

// definitions

class Exception : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// initialization

namespace graphics { using Init = ps_GraphicsInit; }
void        init(graphics::Init const& graphics_init);
void        finalize();
std::string version();

// context

enum class Alignment { TopLeft = PS_TOP_LEFT, Center = PS_CENTER, TopRight = PS_TOP_RIGHT, BottomLeft = PS_BOTTOM_LEFT, BottomRight = PS_BOTTOM_RIGHT };

class Context {
public:
    SDL_Point                 position { 0, 0 };
    Alignment                 align = (Alignment) PS_ALIGN_DEFAULT;
    float                     rotation = 0.f;
    float                     zoom = 1.f;
    float                     opacity = 1.f;
    std::optional<SDL_FPoint> rotation_center {};
    bool                      draw_border = false;

    ps_Context context_c() const;
};

// res

namespace res {
    using idx_t = ps_res_idx_t;
    using TileDef = ps_TileDef;
    using Tile = ps_Tile;
    using ResourceId = std::variant<idx_t, std::string>;
    using Manipulator = ps_Manipulator;

    idx_t add_image(uint8_t const* data, size_t sz, Manipulator manipulator=nullptr, void* manip_data=nullptr);
    idx_t add_image(std::string const& name, uint8_t const* data, size_t sz, Manipulator manipulator=nullptr, void* manip_data=nullptr);

    idx_t add_tile(ResourceId const& parent, SDL_FRect const& rect, size_t tile_sz);
    idx_t add_tile(std::string const& name, ResourceId const& parent, SDL_FRect const& rect, size_t tile_sz);
    void  add_tiles(ResourceId const& parent, std::vector<TileDef>& tiles, size_t tile_sz);
    void  add_tiles_from_lua(ResourceId const& id, uint8_t const* data, size_t sz);

    idx_t add_ttf(uint8_t const* data, size_t sz);
    idx_t add_ttf(std::string const& name, uint8_t const* data, size_t sz);
    idx_t add_cursor(SDL_Cursor* cursor);
    idx_t add_cursor(std::string const& name, SDL_Cursor* cursor);
    idx_t add_music(uint8_t const* data, size_t sz);
    idx_t add_music(std::string const& name, uint8_t const* data, size_t sz);
    idx_t add_sound(uint8_t const* data, size_t sz);
    idx_t add_sound(std::string const& name, uint8_t const* data, size_t sz);

    void  set_name(std::string const& name, idx_t idx);
    idx_t idx(std::string const& name);

    std::pair<int, int> image_size(ResourceId res_id);
    Tile                tile(ResourceId res_id);
}

// scene

class Scene {
public:
    Scene();
    ps_Scene const& scene() const { return scene_; }

    void push_context(Context const& ctx);
    void pop_context();

    void add_image(res::ResourceId const& id, SDL_Point const& p, Alignment align = Alignment::TopLeft);
    void add_image(res::ResourceId const& id, SDL_Point const& p, Alignment align, Context const& ctx);

    void add_text(res::ResourceId const& id, std::string const& text, SDL_Point const& p, Alignment align, int font_size, SDL_Color const& color);
    void add_text(res::ResourceId const& id, std::string const& text, SDL_Point const& p, Alignment align, int font_size, SDL_Color const& color, Context const& ctx);

    void set_z_order(int z);

private:
    ps_Scene scene_;
};

// graphics

namespace graphics {
    using namespace std::chrono;

    void          set_bg(uint8_t r, uint8_t g, uint8_t b);
    bool          running();
    microseconds  timestep();
    void          present();
    void          quit();

    void          render_scenes(std::vector<Scene> const& scene);

    SDL_Window*   window();
    SDL_Renderer* renderer();

    void          set_window_title(std::string const& title);
    int           fps();
}

// manip

namespace manip {
    int shadow(uint8_t* pixels, int w, int h, int pitch, void* data);
}

// audio

namespace audio {
    void step();

    void choose_music(res::ResourceId const& id);
    void play_music(bool play);
    void play_sound(res::ResourceId const& id);
}

}

#endif //PASTEL2D_CC_HH
