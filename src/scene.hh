#ifndef SCENE_HH
#define SCENE_HH

#include <optional>
#include <queue>
#include <utility>

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
        int         x, y;
        Pen         pen;
    };

    void add(ResourceId const& resource, int x, int y, Pen pen={}) {
        pen.zoom *= current_zoom_;
        x = x * current_zoom_ + relative_x_;
        y = y * current_zoom_ + relative_y_;
        images_.emplace_back(resource, x, y, std::move(pen));
    }

    void set_current_zoom(float zoom) { current_zoom_ = zoom; }

    void set_relative_pos(int x, int y) {
        relative_x_ = x;
        relative_y_ = y;
    }

    [[nodiscard]] std::deque<Image> const& images() const { return images_; }

private:
    std::deque<Image> images_;
    float current_zoom_ = 1.f;
    int relative_x_ = 0.f;
    int relative_y_ = 0.f;
};

}

#endif //SCENE_HH
