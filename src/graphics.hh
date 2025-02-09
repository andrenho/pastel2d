#ifndef GRAPHICS_HH
#define GRAPHICS_HH

#include <chrono>
#include <string>

#include "util/noncopyable.hh"
#include "resourcemanager.hh"
#include "scene.hh"
#include "SDL2/SDL.h"

struct ImGuiIO;

namespace ps {

using hr = std::chrono::high_resolution_clock;
using Duration = decltype(hr::now() - hr::now());

class Graphics : public NonCopyable {
public:
    virtual ~Graphics();

    void step();

    [[nodiscard]] bool                   running() const { return running_; }
    [[nodiscard]] SDL_Window*            window() const { return window_; }
    [[nodiscard]] SDL_Renderer*          ren() const { return ren_; }
    [[nodiscard]] ResourceManager const& res() const { return res_; }
    [[nodiscard]] ResourceManager&       res()       { return res_; }

protected:
    Graphics(std::string const& window_title, int window_width, int window_height, SDL_WindowFlags flags=(SDL_WindowFlags) 0);

    virtual void update(Duration timestep) = 0;
    virtual void event(SDL_Event const& event) = 0;

    virtual Scene create_scene() const = 0;
    virtual void draw_gui() const {}

    bool            running_ = true;
    ResourceManager res_ {};
    SDL_Window*     window_ = nullptr;
    SDL_Renderer*   ren_ = nullptr;
    ImGuiIO*        io = nullptr;

private:
    void init_imgui();
    void finalize_imgui();

    void render() const;
    void render_scene(Scene const& scene) const;
    void render_image(Scene::Image const& image) const;

    std::string     window_title_;

    decltype(hr::now()) last_frame_ = hr::now();
    Duration            frame_time_ = std::chrono::milliseconds(0);
    size_t              frame_count_ = 0;
    size_t              total_frames_ = 0;
};

} // ps

#endif //GRAPHICS_HH
