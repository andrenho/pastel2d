#include "graphics.h"

static bool running = true;

void ps_graphics_init(const char* window_title, size_t w, size_t h)
{

}

void ps_graphics_finalize()
{

}

bool ps_graphics_running()
{
    return running;
}

void ps_graphics_do_events(void(* event_manager)())
{

}

size_t ps_graphics_timestep_us()
{
    return 0;  // TODO
}

void ps_graphics_render_scene(void(* scene_creator)())
{

}

void ps_graphics_present()
{

}