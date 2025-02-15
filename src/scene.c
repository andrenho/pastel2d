#include "scene.h"

#include <stb_ds.h>

static int ps_scene_init(Scene* scene)
{
    scene->artifacts = NULL;
    scene->context_stack = NULL;

    arrpush(scene->context_stack, ps_create_context());

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

int ps_scene_add_image(Scene* scene, resource_idx_t idx, SDL_Rect r, Context const* ctx)
{
    if (idx == RES_ERROR)
        return -1;

    ps_scene_push_context(scene, ps_create_context_with(CTX_POSITION, r, NULL));
    if (ctx)
        ps_scene_push_context(scene, *ctx);

    Artifact artifact = {
        .type = A_IMAGE,
        .image = {
            .res_id = idx,
            .context = *ps_scene_current_context(scene),
        },
    };
    arrpush(scene->artifacts, artifact);

    ps_scene_pop_context(scene);
    if (ctx)
        ps_scene_pop_context(scene);

    return 0;
}

int ps_scene_add_text(Scene* scene, resource_idx_t idx, const char* text, SDL_Rect rect, int font_size, SDL_Color color, Context const* ctx)
{
    if (idx == RES_ERROR)
        return -1;

    ps_scene_push_context(scene, ps_create_context_with(CTX_POSITION, rect, NULL));
    if (ctx)
        ps_scene_push_context(scene, *ctx);

    Artifact artifact = {
        .type = A_TEXT,
        .text = {
            .font_idx = idx,
            .context = *ps_scene_current_context(scene),
            .text = strdup(text),
            .font_size = font_size,
            .color = color,
        },
    };
    arrpush(scene->artifacts, artifact);

    ps_scene_pop_context(scene);
    if (ctx)
        ps_scene_pop_context(scene);

    return 0;

    // stbtt_MakeCodepointBitmap(font, )
    return 0;
}

Context const* ps_scene_current_context(Scene const* scene)
{
    return &scene->context_stack[arrlen(scene->context_stack) - 1];
}

int ps_scene_push_context(Scene* scene, Context context)
{
    Context new_context = ps_context_sum(ps_scene_current_context(scene), &context);
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
    for (int i = 0; i < arrlen(scene->artifacts); ++i)
        if (scene->artifacts[i].type == A_TEXT)
            free(scene->artifacts[i].text.text);

    arrfree(scene->artifacts);
    arrfree(scene->context_stack);
    return 0;
}
