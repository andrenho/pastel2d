#include "graphics.h"

#include <stb_ds.h>
#include <SDL3/SDL_timer.h>
#include <stdio.h>

#include "error.h"
#include "scene.h"

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

static void render_texture(SDL_Texture* tx, SDL_FRect const* origin, Context const* ctx)
{
    float tw, th;
    SDL_GetTextureSize(tx, &tw, &th);

    SDL_FRect dest = {
        .x = ctx->position.rect.x,
        .y = ctx->position.rect.y,
        .w = ctx->position.rect.w != 0 ? ctx->position.rect.w : tw,
        .h = ctx->position.rect.h != 0 ? ctx->position.rect.h : th,
    };

    if (ctx->zoom.has_value)
        SDL_SetRenderScale(ren, ctx->zoom.value, ctx->zoom.value);
    SDL_RenderTexture(ren, tx, origin, &dest);
    SDL_SetRenderScale(ren, 1.f, 1.f);
}

void ps_graphics_render_scene(Scene* (*scene_creator)(void* data), void* data)
{
    Scene* scenes = scene_creator(data);

    SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    for (size_t i = 0; i < (size_t) arrlen(scenes); ++i) {
        for (size_t j = 0; j < (size_t) arrlen(scenes[i].artifacts); ++j) {
            Artifact const* a = &scenes[i].artifacts[j];
            switch (scenes[i].artifacts[j].type) {
                case A_IMAGE: {
                    SDL_Texture* tx = ps_res_get_texture(a->image.res_id);
                    render_texture(tx, NULL, &a->image.context);
                    break;
                }
            }
        }

        ps_scene_finalize(&scenes[i]);
    }
}

int ps_graphics_present()
{
    return SDL_RenderPresent(ren) ? 0 : -1;
}

SDL_Window* ps_graphics_window() { return window; }
SDL_Renderer* ps_graphics_renderer() { return ren; }
