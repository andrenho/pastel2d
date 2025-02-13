#include "scene.h"

#include <stb_ds.h>

int ps_scene_init(Scene* scene)
{
    scene->artifacts = NULL;
    scene->context_stack = NULL;

    Context context = ps_create_context();
    arrpush(scene->context_stack, context);
}

int ps_scene_add_image(Scene* scene, resource_idx_t resource_id, int x, int y)
{
    ps_scene_push_context(scene, &(Context) {
        .position = { x, y, 0, 0 },
    });

    Artifact artifact = {
        .type = A_IMAGE,
        .image = {
            .res_id = resource_id,
            .context = ps_scene_current_context(scene),
        },
    };
    arrpush(scene->artifacts, artifact);

    ps_scene_pop_context(scene);

    return 0;
}

Context const* ps_scene_current_context(Scene const* scene)
{
    return &scene->context_stack[arrlen(scene->context_stack) - 1];
}

int ps_scene_push_context(Scene* scene, Context const* context)
{
    Context new_context = ps_context_sum(ps_scene_current_context(scene), context);
    arrpush(scene->context_stack, new_context);
}

int ps_scene_pop_context(Scene* scene)
{
    arrpop(scene->context_stack);
}

int ps_scene_finalize(Scene* scene)
{
    arrfree(scene->artifacts);
    arrfree(scene->context_stack);
    return 0;
}
