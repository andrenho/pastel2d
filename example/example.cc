#include <SDL3/SDL.h>

#include "pastel2d.hh"

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
    auto example = ps::res::add_png(example_example_png, example_example_png_sz);

    SDL_Color shadow_color = { 0, 0, 0, 255 };
    auto example_shadow = ps::res::add_png(example_example_png, example_example_png_sz, ps::manip::shadow, &shadow_color);

    ps::res::add_tiles_from_lua(example, example_example_tileset_lua, example_example_tileset_lua_sz);

    ps::res::add_ttf("font1", example_OpenSans_Medium_ttf, example_OpenSans_Medium_ttf_sz);
    ps::res::add_ttf("font2", example_Born2bSportyFS_otf, example_Born2bSportyFS_otf_sz);

    ps::res::add_music("music", example_nemesis_mod, example_nemesis_mod_sz);

    ps::res::add_sound("sound", example_shotgun_wav, example_shotgun_wav_sz);
}

static void update(std::chrono::microseconds duration)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT || (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_Q))
            ps::graphics::quit();
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_SPACE)
            ps::audio::play_sound("sound");
    }
}

int main()
{
    SDL_Log("pastel2d version %s", ps::version().c_str());

    ps::init({
        .appname = "pastel2d-example",
        .appidentifier = "com.github.pastel2d",
        .appversion = "1.0.0",
        .window_w = 1400,
        .window_h = 900,
        .flags = SDL_WINDOW_RESIZABLE,
    });
    ps::graphics::set_bg(160, 180, 200);

    init_resources();

    ps::audio::choose_music("music");
    ps::audio::play_music(true);

    while (ps::graphics::running()) {
        ps::audio::step();
        update(ps::graphics::timestep());
        ps::graphics::present();
    }

    ps::finalize();
}
