#include "graphics.h"

#include <stdio.h>
#include <time.h>

#include <stb_ds.h>
#include <SDL3/SDL_timer.h>

#include "error.h"
#include "scene.h"
#include "textcache.h"

static bool          running_ = true;
static SDL_Window*   window = NULL;
static SDL_Renderer* ren = NULL;
static uint64_t      last_frame = 0;
static uint8_t       bg_r = 0, bg_g = 0, bg_b = 0;

extern char last_error[LAST_ERROR_SZ];

int ps_graphics_init(GraphicsInit const* init)
{
    srand(time(NULL));

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
    text_cache_finalize();
    if (ren)
        SDL_DestroyRenderer(ren);
    if (window)
        SDL_DestroyWindow(window);
    if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    if (SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
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
    text_cache_cleanup();

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
    SDL_FRect dest = {};

    // calculate destination size
    if (origin != NULL) {  // partial texture (tile)
        dest.w = origin->w;
        dest.h = origin->h;
    } else {  // full texture
        float tw, th;
        SDL_GetTextureSize(tx, &tw, &th);
        dest.w = ctx->position.rect.w != 0 ? ctx->position.rect.w : tw;
        dest.h = ctx->position.rect.h != 0 ? ctx->position.rect.h : th;
    }

    // calculate position
    if (ctx->position.rect.w == 0) {
        dest.x = ctx->position.rect.x;
        dest.y = ctx->position.rect.y;
    } else {
        dest.x = ctx->position.rect.x + ctx->position.rect.w / 2 - dest.w / 2;
        dest.y = ctx->position.rect.y + ctx->position.rect.h / 2 - dest.h / 2;
    }

    // check if within bounds
    int scr_w, scr_h;
    SDL_GetWindowSizeInPixels(window, &scr_w, &scr_h);
    SDL_FRect scr = { 0, 0, scr_w / (ctx->zoom.has_value ? ctx->zoom.value : 1), scr_h / (ctx->zoom.has_value ? ctx->zoom.value : 1) };
    if (!SDL_HasRectIntersectionFloat(&scr, &dest))
        return;

    // opacity
    if (ctx->opacity.has_value)
        SDL_SetTextureAlphaModFloat(tx, ctx->opacity.value / 100.f);

    // zoom
    if (ctx->zoom.has_value)
        SDL_SetRenderScale(ren, ctx->zoom.value, ctx->zoom.value);

    // render with rotation/center
    SDL_RenderTextureRotated(ren, tx,
        origin, &dest,
        ctx->rotation.value,
        ctx->rotation_center.has_value ? &ctx->rotation_center.point : NULL,
        SDL_FLIP_NONE);

    // restore
    SDL_SetRenderScale(ren, 1.f, 1.f);
    SDL_SetTextureAlphaModFloat(tx, 1.0f);
}

int render_scene(Scene* scene)
{
    for (size_t j = 0; j < (size_t) arrlen(scene->artifacts); ++j) {
        Artifact const* a = &scene->artifacts[j];
        switch (a->type) {
            case A_IMAGE:
                switch (ps_res_get_type(a->image.res_id)) {
                    case RT_TEXTURE:
                        SDL_Texture* tx = ps_res_get_texture(a->image.res_id);
                        render_texture(tx, NULL, &a->image.context);
                        break;
                    case RT_TILE:
                        Tile const* tile = ps_res_get_tile(a->image.res_id);
                        render_texture(tile->texture, &tile->rect, &a->image.context);
                        break;
                    case RT_FONT:
                    default:
                        snprintf(last_error, sizeof last_error, "Invalid type for text resource %zu", a->image.res_id);
                        return -1;
                }
                break;
            case A_TEXT: {
                stbtt_fontinfo const* font = ps_res_get_font(a->text.font_idx);
                if (font == NULL)
                    return -1;
                SDL_Texture* tx = text_cache_get_texture(font, a->text.text, a->text.font_size, a->text.color);
                render_texture(tx, NULL, &a->text.context);
                break;
            }
        }
    }

    return 0;
}

int ps_graphics_render_scene(Scene* (*scene_creator)(void* data), void* data)
{
    Scene* scenes = scene_creator(data);

    SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    for (size_t i = 0; i < (size_t) arrlen(scenes); ++i) {
        if (render_scene(&scenes[i]) != 0)
            return -1;
        ps_scene_finalize(&scenes[i]);
    }

    arrfree(scenes);

    return 0;
}

int ps_graphics_present()
{
    return SDL_RenderPresent(ren) ? 0 : -1;
}

SDL_Window* ps_graphics_window() { return window; }
SDL_Renderer* ps_graphics_renderer() { return ren; }
