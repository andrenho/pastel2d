#include <stdlib.h>

#include "pastel2d.h"
#include "manip.h"
#include "scene.h"

// embedded files
#include "example.png.h"
#include "example.tileset.lua.h"
#include "example-shadow.tileset.lua.h"
#include "OpenSans-Medium.ttf.h"
#include "Born2bSportyFS.otf.h"
#include "nemesis.mod.h"
#include "shotgun.wav.h"

static void init_resources()
{
    resource_idx_t example = ps_res_add_png(example_example_png, example_example_png_sz);

    SDL_Color shadow = { 0, 0, 0, 255 };
    resource_idx_t example_shadow = ps_res_add_png_manip(example_example_png, example_example_png_sz, manip_shadow, &shadow);

    ps_res_add_tiles_from_lua(example, example_example_tileset_lua, example_example_tileset_lua_sz);
    ps_res_add_tiles_from_lua(example_shadow, example_example_shadow_tileset_lua, example_example_shadow_tileset_lua_sz);

    SDL_assert(NAME("font1", ps_res_add_ttf(example_OpenSans_Medium_ttf, example_OpenSans_Medium_ttf_sz)) != RES_ERROR);
    SDL_assert(NAME("font2", ps_res_add_ttf(example_Born2bSportyFS_otf, example_Born2bSportyFS_otf_sz)) != RES_ERROR);

    SDL_assert(NAME("music", ps_res_add_music(example_nemesis_mod, example_nemesis_mod_sz, 44100)) != RES_ERROR);

    SDL_assert(NAME("sound", ps_red_add_sound(example_shotgun_wav, example_shotgun_wav_sz)) != RES_ERROR);
}

static void event_manager(SDL_Event* e, bool* running)
{
    if (e->type == SDL_EVENT_QUIT || (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_Q))
        *running = false;
    if (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_SPACE)
        ps_audio_play_sound(IDX("sound"));
}

static Scene* scene_creator(void*)
{
    Scene* scenes = ps_create_scenes(1);

    ps_scene_push_context(&scenes[0], ps_create_context_with(CTX_ZOOM, 2.f, NULL));

    ps_scene_add_image_with(&scenes[0], IDX("happy"), (SDL_Rect) { 100, 100, 58, 78 },
        CTX_ROTATION, 90.f, CTX_OPACITY, .5f, NULL);

    ps_scene_add_text_with(&scenes[0], IDX("font1"), "Hello world to ALL!", POS(10, 10), 32, (SDL_Color) { 0, 0, 0, 255 },
        CTX_ZOOM, .5f, NULL);

    ps_scene_add_text(&scenes[0], IDX("font2"), "Press SPACE to fire shotgun", POS(10, 30), 18, (SDL_Color) { 0, 0, 0, 255 }, NULL);

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

    ps_init(&(GraphicsInit) {
        .appname = "pastel2d-example",
        .appidentifier = "com.github.pastel2d",
        .appversion = "1.0.0",
        .window_w = 1400,
        .window_h = 900,
        .flags = SDL_WINDOW_RESIZABLE,
    });
    ps_graphics_set_bg(160, 180, 200);

    init_resources();

    SDL_assert(ps_audio_choose_music(IDX("music")) == 0);
    ps_audio_play_music(true);

    while (ps_graphics_running()) {
        ps_graphics_do_events(event_manager);
        ps_audio_step();
        update(ps_graphics_timestep_us());
        ps_graphics_render_scene(scene_creator, NULL);
        post_scene();
        ps_graphics_present();
    }

    ps_finalize();

    return EXIT_SUCCESS;
}
