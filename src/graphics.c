#include "graphics.h"

#include "error.h"

static bool          running_ = true;
static SDL_Window*   window = NULL;
static SDL_Renderer* ren = NULL;

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

    char window_title[256];
    snprintf(window_title, sizeof window_title, "%s %s", init->appname, init->appversion);
    if (!SDL_CreateWindowAndRenderer(init->appname, init->window_w, init->window_h, init->flags, &window, &ren)) {
        snprintf(last_error, sizeof last_error, "Could not create window: %s", SDL_GetError());
        return -1;
    }

    return 0;
}

int ps_graphics_finalize()
{
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
    return 0;  // TODO
}

void ps_graphics_render_scene(void(* scene_creator)())
{

}

int ps_graphics_present()
{
    return SDL_RenderPresent(ren) ? 0 : -1;
}