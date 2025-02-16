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
    ps_res_idx_t example = PS_ASRT_RES(ps_res_add_png(example_example_png, example_example_png_sz));

    SDL_Color shadow = { 0, 0, 0, 255 };
    ps_res_idx_t example_shadow = PS_ASRT_RES(ps_res_add_png_manip(example_example_png, example_example_png_sz, ps_manip_shadow, &shadow));

    PS_ASRT(ps_res_add_tiles_from_lua(example, example_example_tileset_lua, example_example_tileset_lua_sz));
    PS_ASRT(ps_res_add_tiles_from_lua(example_shadow, example_example_shadow_tileset_lua, example_example_shadow_tileset_lua_sz));

    PS_NAME("font1", PS_ASRT_RES(ps_res_add_ttf(example_OpenSans_Medium_ttf, example_OpenSans_Medium_ttf_sz)));
    PS_NAME("font2", PS_ASRT_RES(ps_res_add_ttf(example_Born2bSportyFS_otf, example_Born2bSportyFS_otf_sz)));

    PS_NAME("music", PS_ASRT_RES(ps_res_add_music(example_nemesis_mod, example_nemesis_mod_sz, 44100)));

    PS_NAME("sound", PS_ASRT_RES(ps_red_add_sound(example_shotgun_wav, example_shotgun_wav_sz)));
}

static void event_manager(SDL_Event* e, bool* running)
{
    if (e->type == SDL_EVENT_QUIT || (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_Q))
        *running = false;
    if (e->type == SDL_EVENT_KEY_DOWN && e->key.key == SDLK_SPACE)
        PS_ASRT(ps_audio_play_sound(PS_IDX("sound")));
}

static ps_Scene* scene_creator(void*)
{
    ps_Scene* scenes = ps_create_scenes(1);

    PS_ASRT(ps_scene_push_context(&scenes[0], ps_create_context_with(CTX_ZOOM, 2.f, NULL)));

    PS_ASRT(ps_scene_add_image_with(&scenes[0], PS_IDX("happy"), (SDL_Rect) { 100, 100, 58, 78 },
        CTX_ROTATION, 90.f, CTX_OPACITY, .5f, NULL));

    ps_scene_add_text_with(&scenes[0], PS_IDX("font1"), "Hello world to ALL!", POS(10, 10), 32, (SDL_Color) { 0, 0, 0, 255 },
        CTX_ZOOM, .5f, NULL);

    ps_scene_add_text(&scenes[0], PS_IDX("font2"), "Press SPACE to fire shotgun", POS(10, 30), 18, (SDL_Color) { 0, 0, 0, 255 }, NULL);

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

int main()
{
    SDL_Log("pastel2d version %s", ps_version(NULL, NULL, NULL));

    ps_init(&(ps_GraphicsInit) {
        .appname = "pastel2d-example",
        .appidentifier = "com.github.pastel2d",
        .appversion = "1.0.0",
        .window_w = 1400,
        .window_h = 900,
        .flags = SDL_WINDOW_RESIZABLE,
    });
    ps_graphics_set_bg(160, 180, 200);

    init_resources();

    PS_ASRT(ps_audio_choose_music(PS_IDX("music")));
    PS_ASRT(ps_audio_play_music(true));

    while (ps_graphics_running()) {
        PS_ASRT(ps_graphics_do_events(event_manager));
        PS_ASRT(ps_audio_step());
        update(ps_graphics_timestep_us());
        PS_ASRT(ps_graphics_render_scene(scene_creator, NULL));
        post_scene();
        PS_ASRT(ps_graphics_present());
    }

    ps_finalize();

    return EXIT_SUCCESS;
}
