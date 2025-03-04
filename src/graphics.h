#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stddef.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#include "scene.h"

typedef struct {
    const char*     appname;
    const char*     appidentifier;
    const char*     appversion;
    size_t          window_w;
    size_t          window_h;
    SDL_WindowFlags flags;
} ps_GraphicsInit;

int           ps_graphics_set_bg(uint8_t r, uint8_t g, uint8_t b);

int           ps_graphics_render_scenes(ps_Scene* scenes, size_t n_scenes);
int           ps_graphics_present();

int           ps_graphics_set_window_title(const char* fmt, ...);

bool          ps_graphics_running();
void          ps_graphics_quit();

size_t        ps_graphics_timestep_us();   // call only once per frame

SDL_Window*   ps_graphics_window();
SDL_Renderer* ps_graphics_renderer();
int           ps_graphics_fps();

#endif //GRAPHICS_H
