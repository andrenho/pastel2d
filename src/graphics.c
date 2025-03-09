#include "graphics.h"

#include <pl_log.h>
#include <stdio.h>
#include <time.h>

#include <stb_ds.h>
#include <SDL3/SDL_timer.h>

#include "audio.h"
#include "scene.h"

#include "private/textcache.h"
#include "private/res_priv.h"
#include "private/scene_priv.h"

static bool          running_ = true;
static SDL_Window*   window = NULL;
static SDL_Renderer* ren = NULL;
static uint64_t      last_frame = 0;
static uint8_t       bg_r = 0, bg_g = 0, bg_b = 0;
static char          window_title[1024];

// FPS calculation
static uint64_t      frame_count = 0;
static double        fps = 0.0;
static bool          show_fps = false;

static uint32_t update_fps(void* userdata, SDL_TimerID timer_id, uint32_t interval)
{
    fps = (double) frame_count / (double) interval * 1000.0;
    frame_count = 0;
    return 1000;
}

int ps_graphics_init(ps_GraphicsInit const* init)
{
    srand(time(NULL));

    const int linked = SDL_GetVersion();
    PL_INFO("SDL version: %d.%d.%d",
        SDL_VERSIONNUM_MAJOR(linked), SDL_VERSIONNUM_MINOR(linked), SDL_VERSIONNUM_MICRO(linked));

    SDL_SetAppMetadata(init->appname, init->appversion, init->appidentifier);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        PL_ERROR_RET(-1, "Could not initialize SDL: %s", SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    snprintf(window_title, sizeof window_title, "%s %s", init->appname, init->appversion);
    if (!SDL_CreateWindowAndRenderer(init->appname, init->window_w, init->window_h, init->flags, &window, &ren))
        PL_ERROR_RET(-1, "Could not create window: %s", SDL_GetError());

    PL_INFO("Current SDL_Renderer: %s", SDL_GetRendererName(ren));

    last_frame = SDL_GetTicksNS();
    SDL_AddTimer(1000, update_fps, NULL);

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

void ps_graphics_quit()
{
    running_ = false;
}

size_t ps_graphics_timestep_us()
{
    uint64_t now = SDL_GetTicksNS();
    uint64_t diff = now - last_frame;
    last_frame = now;
    ++frame_count;

    return SDL_NS_TO_US(diff);
}

static void render_texture(SDL_Texture* tx, SDL_FRect const* origin, ps_Context const* ctx)
{
    SDL_FRect dest = {};

    // calculate destination size
    if (origin != NULL) {  // partial texture (tile)
        dest.w = origin->w;
        dest.h = origin->h;
    } else {  // full texture
        float tw, th;
        SDL_GetTextureSize(tx, &tw, &th);
        dest.w = ctx->position.w != 0 ? ctx->position.w : tw;
        dest.h = ctx->position.h != 0 ? ctx->position.h : th;
    }

    // calculate position
    if (ctx->position.w == 0) {
        dest.x = ctx->position.x;
        dest.y = ctx->position.y;
    } else {
        dest.x = ctx->position.x + ctx->position.w / 2 - dest.w / 2;
        dest.y = ctx->position.y + ctx->position.h / 2 - dest.h / 2;
    }

    // check if within bounds
    int scr_w, scr_h;
    SDL_GetWindowSizeInPixels(window, &scr_w, &scr_h);
    SDL_FRect scr = { 0, 0, scr_w / ctx->zoom, scr_h / ctx->zoom };
    if (!SDL_HasRectIntersectionFloat(&scr, &dest))
        return;

    // opacity
    SDL_SetTextureAlphaModFloat(tx, ctx->opacity);

    // zoom
    SDL_SetRenderScale(ren, ctx->zoom, ctx->zoom);

    // render with rotation/center
    SDL_RenderTextureRotated(ren, tx,
        origin, &dest,
        ctx->rotation,
        ctx->rotation_center.x != DEFAULT_ROT_CENTER ? &ctx->rotation_center : NULL,
        SDL_FLIP_NONE);

    // restore
    SDL_SetRenderScale(ren, 1.f, 1.f);
    SDL_SetTextureAlphaModFloat(tx, 1.0f);
}

int render_scene(ps_Scene* scene)
{
    for (size_t j = 0; j < (size_t) arrlen(scene->artifacts); ++j) {
        ps_Artifact const* a = &scene->artifacts[j];
        switch (a->type) {
            case A_IMAGE:
                switch (ps_res_get_type(a->image.res_id)) {
                    case RT_TEXTURE: {
                        SDL_Texture* tx = ps_res_get(a->image.res_id, RT_TEXTURE)->texture;
                        render_texture(tx, NULL, &a->image.context);
                        break;
                    }
                    case RT_TILE: {
                        ps_Tile const* tile = &ps_res_get(a->image.res_id, RT_TILE)->tile;
                        render_texture(tile->texture, &tile->rect, &a->image.context);
                        break;
                    }
                    case RT_FONT:
                    case RT_CURSOR:
                    case RT_MUSIC:
                    case RT_SOUND:
                        PL_ERROR_RET(-1, "Invalid type for image resource %zu", a->image.res_id);
                }
                break;
            case A_TEXT: {
                stbtt_fontinfo const* font = ps_res_get(a->text.font_idx, RT_FONT)->font;
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

static int comp_scene(void const* scene_a, void const* scene_b)
{
    ps_Scene const* scene1 = scene_a;
    ps_Scene const* scene2 = scene_b;
    return scene2->z_order - scene1->z_order;
}

int ps_graphics_render_scenes(ps_Scene* scenes, size_t n_scenes)
{
    SDL_SetRenderDrawColor(ren, bg_r, bg_g, bg_b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    // reorder scenes
    qsort(scenes, n_scenes, sizeof(ps_Scene), comp_scene);

    // render scene
    for (size_t i = 0; i < n_scenes; ++i) {
        if (render_scene(&scenes[i]) != 0)
            return -1;
        ps_scene_finalize(&scenes[i]);
    }

    return 0;
}

int ps_graphics_present()
{
    text_cache_cleanup();
    return SDL_RenderPresent(ren) ? 0 : -1;
}

int  ps_graphics_set_window_title(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(window_title, sizeof window_title, fmt, ap);
    bool success = SDL_SetWindowTitle(window, buf);
    va_end(ap);
    return success ? 0 : -1;
}

SDL_Window*   ps_graphics_window() { return window; }
SDL_Renderer* ps_graphics_renderer() { return ren; }
int           ps_graphics_fps() { return (int) fps; }
