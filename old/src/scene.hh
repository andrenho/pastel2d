#ifndef SCENE_HH
#define SCENE_HH

#include <chrono>
#include <optional>
#include <queue>
#include <SDL_pixels.h>
#include <utility>
#include <variant>

#include "resourcemanager.hh"
#include "util/time.hh"

namespace ps {

struct Pen {
    double opacity = 1.0;
    double rotation = 0.0;
    float  zoom = 1.0;
    std::optional<std::pair<int, int>> rotation_center {};

    enum class FullScreen { No, Stretch, Center };
    FullScreen full_screen = FullScreen::No;
};

class Scene {
public:
    std::array<uint8_t, 3> bg = { 0, 0, 0 };

    struct Image {
        ResourceId  resource;
        SDL_Rect    rect;
        Pen         pen;
    };

    struct Text {
        ResourceId  resource;
        std::string text;
        SDL_Rect    rect;
        SDL_Color   color;
        Pen         pen;
        Duration    cache_duration;
    };

    using Artifact = std::variant<Image, Text>;

    void add(ResourceId const& resource, int x, int y, Pen pen={}) {
        pen.zoom *= current_zoom_;
        x = x * current_zoom_ + relative_x_;
        y = y * current_zoom_ + relative_y_;
        artifacts_.emplace_back(Image { resource, { x, y, 0, 0 }, std::move(pen) });
    }

    void add(ResourceId const& resource, SDL_Rect rect, Pen pen={}) {
        pen.zoom *= current_zoom_;
        rect.x = rect.x * current_zoom_ + relative_x_;
        rect.y = rect.y * current_zoom_ + relative_y_;
        artifacts_.emplace_back(Image { resource, rect, std::move(pen) });
    }

    void add_text(ResourceId const& font, std::string const& text, int x, int y, SDL_Color const& color, Pen pen={}, Duration cache_duration=15s)
    {
        pen.zoom *= current_zoom_;
        x = x * current_zoom_ + relative_x_;
        y = y * current_zoom_ + relative_y_;
        artifacts_.emplace_back(Text { font, text, { x, y, 0, 0 }, color, pen, cache_duration });
    }

    void add_text(ResourceId const& font, std::string const& text, SDL_Rect rect, SDL_Color const& color, Pen pen={}, Duration cache_duration=15s)
    {
        pen.zoom *= current_zoom_;
        rect.x = rect.x * current_zoom_ + relative_x_;
        rect.y = rect.y * current_zoom_ + relative_y_;
        artifacts_.emplace_back(Text { font, text, rect, color, pen, cache_duration });
    }

    void set_current_zoom(float zoom) { current_zoom_ = zoom; }

    void set_relative_pos(int x, int y) {
        relative_x_ = x;
        relative_y_ = y;
    }

    [[nodiscard]] std::deque<Artifact> const& artifacts() const { return artifacts_; }

private:
    std::deque<Artifact> artifacts_;
    float current_zoom_ = 1.f;
    int relative_x_ = 0.f;
    int relative_y_ = 0.f;
};

}

#endif //SCENE_HH
