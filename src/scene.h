#ifndef SCENE_H
#define SCENE_H

#include "res.h"
#include "context.h"

typedef enum { A_IMAGE, A_TEXT } ArtifactType;

typedef struct {
    resource_idx_t res_id;
    Context        context;
} Image;

typedef struct {
    resource_idx_t font_idx;
    Context        context;
    char*          text;
    int            font_size;
    SDL_Color      color;
} Text;

typedef struct {
    ArtifactType type;
    union {
        Image image;
        Text  text;
    };
} Artifact;

typedef struct {
    Artifact* artifacts;
    Context*  context_stack;
} Scene;

Scene*         ps_create_scenes(size_t n_scenes);

Context const* ps_scene_current_context(Scene const* scene);
int            ps_scene_push_context(Scene* scene, Context const* context);
int            ps_scene_pop_context(Scene* scene);

int ps_scene_add_image(Scene* scene, resource_idx_t idx, int x, int y, Context const* ctx);
int ps_scene_add_image_rect(Scene* scene, resource_idx_t idx, SDL_Rect r, Context const* ctx);
int ps_scene_add_image_name(Scene* scene, const char* resource_name, int x, int y, Context const* ctx);
int ps_scene_add_image_name_rect(Scene* scene, const char* resource_name, SDL_Rect r, Context const* ctx);

int ps_scene_add_text(Scene* scene, resource_idx_t idx, const char* text, int x, int y, int font_size, SDL_Color color, Context const* ctx);
int ps_scene_add_text_name(Scene* scene, const char* resource_name, const char* text, int x, int y, int font_size, SDL_Color color, Context const* ctx);
int ps_scene_add_text_rect(Scene* scene, resource_idx_t idx, const char* text, SDL_Rect rect, int font_size, SDL_Color color, Context const* ctx);
int ps_scene_add_text_name_rect(Scene* scene, const char* resource_name, const char* text, SDL_Rect rect, int font_size, SDL_Color color, Context const* ctx);

int ps_scene_finalize(Scene* scene);

#endif //SCENE_H
