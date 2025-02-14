#ifndef SCENE_H
#define SCENE_H

#include "res.h"
#include "context.h"

typedef enum { A_IMAGE } ArtifactType;

typedef struct {
    resource_idx_t res_id;
    Context        context;
} Image;

typedef struct {
    ArtifactType type;
    union {
        Image image;
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

int ps_scene_add_image(Scene* scene, resource_idx_t resource_id, int x, int y, Context const* ctx);
int ps_scene_add_image_name(Scene* scene, const char* resource_name, int x, int y, Context const* ctx);

int ps_scene_finalize(Scene* scene);

#endif //SCENE_H
