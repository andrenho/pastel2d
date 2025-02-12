#include <stdlib.h>

#include "pastel2d.hh"

static void init_resources()
{
}

static void event_manager(SDL_Event* e, bool* running)
{
    if (e->type == SDL_EVENT_QUIT)
        *running = false;
}

static void /* TODO - return type? */ scene_creator()
{
}

static void post_scene()
{
}

static void update(size_t timestep_us)
{
}

int main()
{
    ps_graphics_init(&(GraphicsInit) {
        .appname = "pastel2d-example",
        .appidentifier = "com.github.pastel2d",
        .appversion = "1.0.0",
        .window_w = 1400,
        .window_h = 900,
        .flags = SDL_WINDOW_RESIZABLE,
    });
    ps_graphics_set_bg(160, 180, 200);

    init_resources();

    while (ps_graphics_running()) {
        ps_graphics_do_events(event_manager);
        update(ps_graphics_timestep_us());
        ps_graphics_render_scene(scene_creator);
        post_scene();
        ps_graphics_present();
    }

    ps_graphics_finalize();

    return EXIT_SUCCESS;
}