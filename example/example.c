#include <stdlib.h>

#include "pastel2d.h"
#include "manip.h"
#include "scene.h"

// embedded files
#include "example.png.h"
#include "example.tileset.lua.h"
#include "example-shadow.tileset.lua.h"
#include "OpenSans-Medium.ttf.h"

static void init_resources()
{
    resource_idx_t example = ps_res_add_png(example_example_png, example_example_png_sz);

    SDL_Color shadow = { 0, 0, 0, 255 };
    resource_idx_t example_shadow = ps_res_add_png_manip(example_example_png, example_example_png_sz, manip_shadow, &shadow);

    ps_res_add_tiles_from_lua(example, example_example_tileset_lua, example_example_tileset_lua_sz);
    ps_res_add_tiles_from_lua(example_shadow, example_example_shadow_tileset_lua, example_example_shadow_tileset_lua_sz);

    ps_res_name_idx("font",
        ps_res_add_ttf(example_OpenSans_Medium_ttf, example_OpenSans_Medium_ttf_sz));
}

static void event_manager(SDL_Event* e, bool* running)
{
    if (e->type == SDL_EVENT_QUIT || (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_Q))
        *running = false;
}

static Scene* scene_creator(void*)
{
    Scene* scenes = ps_create_scenes(1);

    ps_scene_push_context(&scenes[0], &(Context) { .zoom = { true, 2.f } });
    ps_scene_add_image_name_rect(&scenes[0], "happy", (SDL_Rect) { 100, 100, 58, 78 },
        &(Context) { .rotation = { true, 90 }, .opacity = { true, 20 } });

    return scenes;
}

static void post_scene()
{
    SDL_Renderer* r = ps_graphics_renderer();
    SDL_SetRenderDrawColorFloat(r, 0, 0, 0, 255);
    SDL_SetRenderScale(r, 2.f, 2.f);
    SDL_RenderRect(r, &(SDL_FRect) { 100, 100, 58, 78 });
}

static void update(size_t timestep_us)
{
}

static SDL_AssertState assertion_handler(const SDL_AssertData *data, void *userdata)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", ps_last_error(), ps_graphics_window());
    return SDL_ASSERTION_BREAK;
}

int main()
{
    SDL_SetAssertionHandler(assertion_handler, NULL);

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
