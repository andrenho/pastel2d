#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stddef.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#include "scene.h"

typedef struct {
    const char*     appname;
    size_t          window_w;
    size_t          window_h;
    const char*     appversion;
    const char*     appidentifier;
    SDL_WindowFlags flags;
} GraphicsInit;

int    ps_graphics_init(GraphicsInit const* init);
int    ps_graphics_finalize();

int    ps_graphics_set_bg(uint8_t r, uint8_t g, uint8_t b);

int    ps_graphics_do_events(void (*event_manager)(SDL_Event* e, bool* running));

void   ps_graphics_render_scene(void (*scene_creator)(Scene scenes[MAX_SCENES]));
int    ps_graphics_present();

bool          ps_graphics_running();
size_t        ps_graphics_timestep_us();
SDL_Window*   ps_graphics_window();
SDL_Renderer* ps_graphics_renderer();

#endif //GRAPHICS_H
