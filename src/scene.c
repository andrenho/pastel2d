#include "scene.h"

#include <stb_ds.h>

static int ps_scene_init(Scene* scene)
{
    scene->artifacts = NULL;
    scene->context_stack = NULL;

    Context context = ps_create_context();
    arrpush(scene->context_stack, context);

    return 0;
}

Scene* ps_create_scenes(size_t n_scenes)
{
    Scene* scenes = NULL;
    for (size_t i = 0; i < n_scenes; ++i) {
        Scene scene;
        ps_scene_init(&scene);
        arrpush(scenes, scene);
    }
    return scenes;
}

int ps_scene_add_image(Scene* scene, resource_idx_t resource_id, int x, int y, Context const* ctx)
{
    ps_scene_push_context(scene, &(Context) {
        .position = { true, { x, y, 0, 0 } },
    });
    if (ctx)
        ps_scene_push_context(scene, ctx);

    Artifact artifact = {
        .type = A_IMAGE,
        .image = {
            .res_id = resource_id,
            .context = *ps_scene_current_context(scene),
        },
    };
    arrpush(scene->artifacts, artifact);

    ps_scene_pop_context(scene);
    if (ctx)
        ps_scene_pop_context(scene);

    return 0;
}

int ps_scene_add_image_name(Scene* scene, const char* resource_name, int x, int y, Context const* ctx)
{
    resource_idx_t idx = ps_res_idx(resource_name);
    if (idx == RES_ERROR)
        return -1;
    return ps_scene_add_image(scene, idx, x, y, ctx);
}

Context const* ps_scene_current_context(Scene const* scene)
{
    return &scene->context_stack[arrlen(scene->context_stack) - 1];
}

int ps_scene_push_context(Scene* scene, Context const* context)
{
    Context new_context = ps_context_sum(ps_scene_current_context(scene), context);
    arrpush(scene->context_stack, new_context);
    return 0;
}

int ps_scene_pop_context(Scene* scene)
{
    arrpop(scene->context_stack);
    return 0;
}

int ps_scene_finalize(Scene* scene)
{
    arrfree(scene->artifacts);
    arrfree(scene->context_stack);
    return 0;
}
