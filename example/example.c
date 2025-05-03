#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define PL_LOG_IMPLEMENTATION
#include <pl_log.h>

#include "pastel2d.h"

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
    ps_res_idx_t example = ps_res_add_image(example_example_png, example_example_png_sz);

    SDL_Color shadow = { 0, 0, 0, 255 };
    ps_res_idx_t example_shadow = ps_res_add_image_manip(example_example_png, example_example_png_sz, ps_manip_shadow, &shadow);

    ps_res_add_tiles_from_lua(example, example_example_tileset_lua, example_example_tileset_lua_sz);
    ps_res_add_tiles_from_lua(example_shadow, example_example_shadow_tileset_lua, example_example_shadow_tileset_lua_sz);

    PS_NAME("font1", ps_res_add_ttf(example_OpenSans_Medium_ttf, example_OpenSans_Medium_ttf_sz));
    PS_NAME("font2", ps_res_add_ttf(example_Born2bSportyFS_otf, example_Born2bSportyFS_otf_sz));

    PS_NAME("music", ps_res_add_music(example_nemesis_mod, example_nemesis_mod_sz));

    PS_NAME("sound", ps_red_add_sound(example_shotgun_wav, example_shotgun_wav_sz));
}

static void update(size_t timestep_us)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT || (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_Q))
            ps_graphics_quit();
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_SPACE)
            ps_audio_play_sound(PS_IDX("sound"));
    }
}

static void create_scene(ps_Scene* scene)
{
    ps_scene_init(scene);

    ps_scene_push_context(scene, ps_create_context_with(CTX_ZOOM, 2.f, NULL));

    ps_scene_add_image_with(scene, PS_IDX("happy"), (SDL_Point) { 100, 100 }, PS_TOP_LEFT,
        CTX_ROTATION, 90.f, CTX_OPACITY, .5f, NULL);

    ps_scene_add_text_with(scene, PS_IDX("font1"), "Hello world to ALL!", (SDL_Point) { 10, 10 }, PS_TOP_LEFT, 32, (SDL_Color) { 0, 0, 0, 255 },
        PS_TOP_LEFT, CTX_ZOOM, .5f, CTX_DRAW_BORDER, true, CTX_ROTATION, 90.f, NULL);

    ps_scene_add_text(scene, PS_IDX("font2"), "Press SPACE to fire shotgun", POS(10, 40), PS_TOP_LEFT, 18, (SDL_Color) { 0, 0, 0, 255 }, NULL);

    ps_scene_add_text_with(scene, PS_IDX("font1"), "TEST", (SDL_Point) { 200, 100 }, PS_TOP_LEFT, 32, (SDL_Color) { 0, 0, 0, 255 },
        CTX_ROTATION, 0.f, NULL);
    ps_scene_add_text_with(scene, PS_IDX("font1"), "RIGHT", (SDL_Point) { 200, 100 }, PS_TOP_RIGHT, 32, (SDL_Color) { 0, 0, 0, 255 },
        CTX_ROTATION, 0.f, NULL);
}

static void post_scene()
{
    SDL_Renderer* r = ps_graphics_renderer();
    SDL_SetRenderDrawColorFloat(r, 0, 0, 0, 255);
    SDL_SetRenderScale(r, 2.f, 2.f);
    SDL_RenderRect(r, &(SDL_FRect) { 100, 100, 58, 78 });

    SDL_RenderRect(r, &(SDL_FRect) { 200, 100, 58, 78 });
}

static void error_callback(void* data)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Assertion failed!", pl_last_error(), ps_graphics_window());
    abort();
}

int main()
{
    pl_init();
    pl_set_error_callback(error_callback, NULL);

    SDL_Log("pastel2d version %s", ps_version());

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

    ps_audio_choose_music(PS_IDX("music"));
    ps_audio_play_music(true);

    while (ps_graphics_running()) {
        update(ps_graphics_timestep_us());
        ps_audio_step();

        ps_Scene scenes[1];
        create_scene(&scenes[0]);
        ps_graphics_render_scenes(scenes, 1);

        post_scene();
        ps_graphics_present();
    }

    ps_finalize();

    return EXIT_SUCCESS;
}
