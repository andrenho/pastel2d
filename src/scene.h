#ifndef SCENE_H
#define SCENE_H

#include "res.h"
#include "context.h"

typedef struct ps_Artifact ps_Artifact;

typedef struct ps_Scene {
    ps_Artifact* artifacts;
    ps_Context*  context_stack;
    int          z_order;
    bool         initialized;
} ps_Scene;

int ps_scene_init(ps_Scene* scene);

int ps_scene_push_context(ps_Scene* scene, ps_Context context);
int ps_scene_pop_context(ps_Scene* scene);

int ps_scene_add_image(ps_Scene* scene, ps_res_idx_t idx, SDL_Rect r, ps_Context const* ctx);
int ps_scene_add_image_with(ps_Scene* scene, ps_res_idx_t idx, SDL_Rect r, ps_ContextProperty props, ...);

int ps_scene_add_text(ps_Scene* scene, ps_res_idx_t idx, const char* text, SDL_Rect rect, int font_size, SDL_Color color, ps_Context const* ctx);
int ps_scene_add_text_with(ps_Scene* scene, ps_res_idx_t idx, const char* text, SDL_Rect rect, int font_size, SDL_Color color, ps_ContextProperty props, ...);

#define POS(x, y) ((SDL_Rect) { x, y, 0, 0 })

#endif //SCENE_H
