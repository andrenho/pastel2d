#ifndef GRAPHICS_HH
#define GRAPHICS_HH

#include <chrono>
#include <map>
#include <string>

#include "SDL2/SDL.h"

#include "util/noncopyable.hh"
#include "resourcemanager.hh"
#include "scene.hh"
#include "textcache.hh"
#include "util/time.hh"

struct ImGuiIO;

namespace ps {

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
    void render_text(Scene::Text const& text) const;
    void render_texture(SDL_Texture* texture, SDL_Rect const& origin, Pen const& pen, int x, int y) const;

    std::string     window_title_;
    TextCache       text_cache_;

    decltype(hr::now()) last_frame_ = hr::now();
    Duration            frame_time_ = 0ms;
    size_t              frame_count_ = 0;
    size_t              total_frames_ = 0;
};

} // ps

#endif //GRAPHICS_HH
