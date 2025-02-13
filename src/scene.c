#include "scene.h"

#include <stb_ds.h>

void ps_scene_add_image(Scene* scene, resource_idx_t resource_id, int x, int y)
{
    Artifact artifact = {
        .type = A_IMAGE,
        .image = { .res_id = resource_id, .x = x, .y = y },
    };
    arrpush(scene->artifacts, artifact);
}

void ps_scene_free(Scene* scene)
{
    arrfree(scene->artifacts);
}