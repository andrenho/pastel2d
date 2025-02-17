#ifndef SCENE_H
#define SCENE_H

#include "res.h"
#include "context.h"

typedef enum { A_IMAGE, A_TEXT } ps_ArtifactType;

typedef struct {
    ps_res_idx_t res_id;
    ps_Context   context;
} ps_Image;

typedef struct {
    ps_res_idx_t font_idx;
    ps_Context   context;
    char*        text;
    int          font_size;
    SDL_Color    color;
} ps_Text;

typedef struct {
    ps_ArtifactType type;
    union {
        ps_Image image;
        ps_Text  text;
    };
} ps_Artifact;

typedef struct {
    ps_Artifact* artifacts;
    ps_Context*  context_stack;
    bool         initialized;
} ps_Scene;

int               ps_scene_init(ps_Scene* scene);
int               ps_scene_finalize(ps_Scene* scene);

int               ps_scene_push_context(ps_Scene* scene, ps_Context context);
int               ps_scene_pop_context(ps_Scene* scene);

int ps_scene_add_image(ps_Scene* scene, ps_res_idx_t idx, SDL_Rect r, ps_Context const* ctx);
int ps_scene_add_image_with(ps_Scene* scene, ps_res_idx_t idx, SDL_Rect r, ps_ContextProperty props, ...);

int ps_scene_add_text(ps_Scene* scene, ps_res_idx_t idx, const char* text, SDL_Rect rect, int font_size, SDL_Color color, ps_Context const* ctx);
int ps_scene_add_text_with(ps_Scene* scene, ps_res_idx_t idx, const char* text, SDL_Rect rect, int font_size, SDL_Color color, ps_ContextProperty props, ...);

#define POS(x, y) ((SDL_Rect) { x, y, 0, 0 })

#endif //SCENE_H
