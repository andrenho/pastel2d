#ifndef PASTEL2D_CC_HH
#define PASTEL2D_CC_HH

#include <cstdint>

#include <chrono>
#include <functional>
#include <stdexcept>
#include <string>
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
void                                  init(graphics::Init const& graphics_init);
void                                  finalize();
std::string                           version();
std::tuple<uint8_t, uint8_t, uint8_t> version_number();

// graphics

namespace graphics {
    using namespace std::chrono;

    void          set_bg(uint8_t r, uint8_t g, uint8_t b);
    bool          running();
    microseconds  timestep();
    void          present();
    void          quit();

    SDL_Window*   window();
    SDL_Renderer* renderer();
}

// res

namespace res {
    using idx_t = ps_res_idx_t;
    using ResourceId = std::variant<idx_t, std::string>;
    using Manipulator = ps_Manipulator;

    idx_t add_png(uint8_t const* data, size_t sz, Manipulator manipulator=nullptr, void* manip_data=nullptr);
    idx_t add_png(std::string const& name, uint8_t const* data, size_t sz, Manipulator manipulator=nullptr, void* manip_data=nullptr);

    void  add_tiles_from_lua(ResourceId const& id, uint8_t const* data, size_t sz);
}

// manip

namespace manip {
    int shadow(uint8_t* pixels, int w, int h, int pitch, void* data);
}

}

#endif //PASTEL2D_HH
