#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stddef.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

typedef struct {
    const char*     appname;
    size_t          window_w;
    size_t          window_h;
    const char*     appversion;
    const char*     appidentifier;
    SDL_WindowFlags flags;
} GraphicsInit;

int  ps_graphics_init(GraphicsInit const* init);
int  ps_graphics_finalize();

bool   ps_graphics_running();
int    ps_graphics_do_events(void (*event_manager)(SDL_Event* e, bool* running));
size_t ps_graphics_timestep_us();
void   ps_graphics_render_scene(void (*scene_creator)());
int    ps_graphics_present();

#endif //GRAPHICS_H
