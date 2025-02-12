#include <stdlib.h>

#include "pastel2d.hh"

static void init_resources()
{
}

static void event_manager(/* TODO - parameters? */)
{
}

static void /* TODO - return type? */ scene_creator()
{
}

static void post_scene()
{
}

static void update(size_t timestep_us)
{
}

int main()
{
    ps_graphics_init("pastel2d-example", 1200, 800);
    init_resources();

    while (ps_graphics_running()) {
        ps_graphics_do_events(event_manager);
        update(ps_graphics_timestep_us());
        post_scene();
        ps_graphics_render_scene(scene_creator);
    }

    ps_graphics_finalize();

    return EXIT_SUCCESS;
}