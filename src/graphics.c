#include "graphics.h"

#include <SDL3/SDL_timer.h>

#include "error.h"

static bool          running_ = true;
static SDL_Window*   window = NULL;
static SDL_Renderer* ren = NULL;
static uint64_t      last_frame = 0;
static uint8_t       bg_r = 0, bg_g = 0, bg_b = 0;

extern char last_error[LAST_ERROR_SZ];

int ps_graphics_init(GraphicsInit const* init)
{
    const int linked = SDL_GetVersion();
    SDL_Log("SDL version: %d.%d.%d\n",
        SDL_VERSIONNUM_MAJOR(linked), SDL_VERSIONNUM_MINOR(linked), SDL_VERSIONNUM_MICRO(linked));

    SDL_SetAppMetadata(init->appname, init->appversion, init->appidentifier);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        snprintf(last_error, sizeof last_error, "Could not initialize SDL: %s", SDL_GetError());
        return -1;
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    char window_title[256];
    snprintf(window_title, sizeof window_title, "%s %s", init->appname, init->appversion);
    if (!SDL_CreateWindowAndRenderer(init->appname, init->window_w, init->window_h, init->flags, &window, &ren)) {
        snprintf(last_error, sizeof last_error, "Could not create window: %s", SDL_GetError());
        return -1;
    }

    SDL_Log("Current SDL_Renderer: %s", SDL_GetRendererName(ren));

    last_frame = SDL_GetTicksNS();

    return 0;
}

int ps_graphics_finalize()
{
    if (ren)
        SDL_DestroyRenderer(ren);
    if (window)
        SDL_DestroyWindow(window);
    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_Quit();
    return 0;
}

int ps_graphics_set_bg(uint8_t r, uint8_t g, uint8_t b)
{
    bg_r = r;
    bg_g = g;
    bg_b = b;
    return 0;
}

bool ps_graphics_running()
{
    return running_;
}

int ps_graphics_do_events(void(* event_manager)(SDL_Event* e, bool* running))
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
        event_manager(&e, &running_);
    return 0;
}

size_t ps_graphics_timestep_us()
{
    uint64_t now = SDL_GetTicksNS();
    uint64_t diff = now - last_frame;
    last_frame = now;
    return SDL_NS_TO_US(diff);
}

void ps_graphics_render_scene(void(* scene_creator)())
{
    SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);
}

int ps_graphics_present()
{
    return SDL_RenderPresent(ren) ? 0 : -1;
}