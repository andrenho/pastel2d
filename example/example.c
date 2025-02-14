#include <stdlib.h>

#include "pastel2d.h"
#include "example.png.h"
#include "scene.h"

static void init_resources()
{
    resource_idx_t example = ps_res_name_idx("example", ps_res_add_png(example_example_png, example_example_png_sz));
    ps_res_name_idx("happy", ps_res_add_tile(example, (SDL_Rect) { 0, 0, 1, 1 }, 32));
}

static void event_manager(SDL_Event* e, bool* running)
{
    if (e->type == SDL_EVENT_QUIT)
        *running = false;
}

static Scene* scene_creator(void*)
{
    Scene* scenes = ps_create_scenes(1);

    ps_scene_push_context(&scenes[0], &(Context) { .zoom = { true, 2 } });
    ps_scene_add_image_name(&scenes[0], "example", 100, 100, NULL);

    return scenes;
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
        ps_graphics_render_scene(scene_creator, NULL);
        post_scene();
        ps_graphics_present();
    }

    ps_finalize();

    return EXIT_SUCCESS;
}