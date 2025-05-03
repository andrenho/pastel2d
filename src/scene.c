#include "scene.h"

#include <pl_log.h>

#include "private/scene_priv.h"

#include <stb_ds.h>
#include <stdio.h>

int ps_scene_init(ps_Scene* scene)
{
    scene->artifacts = NULL;
    scene->context_stack = NULL;
    scene->initialized = true;
    scene->z_order = 100;

    arrpush(scene->context_stack, ps_create_context());

    return 0;
}

static ps_Context const* ps_scene_current_context(ps_Scene const* scene)
{
    return &scene->context_stack[arrlen(scene->context_stack) - 1];
}

#define CHECK_INIT(scene) if (!scene->initialized) PL_ERROR_RET(-1, "Scene not initialized");

int ps_scene_add_image(ps_Scene* scene, ps_res_idx_t idx, SDL_Point p, ps_Alignment align, ps_Context const* ctx)
{
    CHECK_INIT(scene)

    if (idx == RES_ERROR)
        return -1;

    ps_scene_push_context(scene, ps_create_context_with(CTX_POSITION, p, CTX_ALIGNMENT, align, NULL));
    if (ctx)
        ps_scene_push_context(scene, *ctx);

    ps_Artifact artifact = {
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

int ps_scene_add_image_with(ps_Scene* scene, ps_res_idx_t idx, SDL_Point p, ps_Alignment align, ps_ContextProperty props, ...)
{
    CHECK_INIT(scene)

    va_list ap;

    va_start(ap, props);
    ps_Context ctx = ps_create_context_with_v(props, ap);
    va_end(ap);

    return ps_scene_add_image(scene, idx, p, align, &ctx);
}

int ps_scene_add_text(ps_Scene* scene, ps_res_idx_t idx, const char* text, SDL_Point p, ps_Alignment align, int font_size, SDL_Color color, ps_Context const* ctx)
{
    CHECK_INIT(scene)

    if (idx == RES_ERROR)
        return -1;

    ps_scene_push_context(scene, ps_create_context_with(CTX_POSITION, p, CTX_ALIGNMENT, align, NULL));
    if (ctx)
        ps_scene_push_context(scene, *ctx);

    ps_Artifact artifact = {
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
}

int ps_scene_add_text_with(ps_Scene* scene, ps_res_idx_t idx, const char* text, SDL_Point p, ps_Alignment align, int font_size, SDL_Color color, ps_ContextProperty props, ...)
{
    CHECK_INIT(scene)

    va_list ap;

    va_start(ap, props);
    ps_Context ctx = ps_create_context_with_v(props, ap);
    va_end(ap);

    return ps_scene_add_text(scene, idx, text, p, align, font_size, color, &ctx);
}

int ps_scene_push_context(ps_Scene* scene, ps_Context context)
{
    CHECK_INIT(scene)

    ps_Context new_context = ps_context_sum(ps_scene_current_context(scene), &context);
    arrpush(scene->context_stack, new_context);
    return 0;
}

int ps_scene_pop_context(ps_Scene* scene)
{
    CHECK_INIT(scene)

    arrpop(scene->context_stack);
    return 0;
}

int ps_scene_finalize(ps_Scene* scene)
{
    for (int i = 0; i < arrlen(scene->artifacts); ++i)
        if (scene->artifacts[i].type == A_TEXT)
            free(scene->artifacts[i].text.text);

    arrfree(scene->artifacts);
    arrfree(scene->context_stack);
    return 0;
}
