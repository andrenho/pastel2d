#include <imgui.h>

#include "pastel2d.hh"

class MyGraphics : public ps::Graphics {
public:
    MyGraphics()
        : Graphics("pastel2d-example", 1024, 800)
    {
        res_.add_texture_and_tiles<"example/example.png", "example/example.tileset.lua">();
        res_.add_font<"example/OpenSans-Medium.ttf">("font", 16);
    }

protected:
    void event(SDL_Event const& event) override
    {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q))
            running_ = false;
    }

    void update(ps::Duration timestep) override
    {
        x_ += dir_x;
        y_ += dir_y;
        if (x_ < 0 || x_ > 200)
            dir_x = -dir_x;
        if (y_ < 0 || y_ > 200)
            dir_y = -dir_y;
    }

    void draw_gui() const override
    {
        ImGui::ShowDemoWindow();
    }

    ps::Scene create_scene() const override
    {
        ps::Scene scene;
        scene.set_current_zoom(2);
        scene.bg = { 230, 230, 230 };
        scene.add("happy", x_, y_);
        scene.add("sad", y_, x_);
        scene.add_text("font", "Hello world!", 400, 400, { 0, 0, 0 });
        return scene;
    }

private:
    int x_ = 30, y_ = 60;
    int dir_x = 1, dir_y = -1;
};

int main()
{
    MyGraphics graphics;

    while (graphics.running()) {
        graphics.step();
    }
}