#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stddef.h>
#include <stdbool.h>

void ps_graphics_init(const char* window_title, size_t w, size_t h);
void ps_graphics_finalize();

bool   ps_graphics_running();
void   ps_graphics_do_events(void (*event_manager)(/* TODO */));
size_t ps_graphics_timestep_us();
void   ps_graphics_render_scene(void (*scene_creator)());
void   ps_graphics_present();

#endif //GRAPHICS_H
