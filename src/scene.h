#ifndef SCENE_H
#define SCENE_H

#define MAX_SCENES 16
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

int     ps_scene_init(Scene* scene);

Context const* ps_scene_current_context(Scene const* scene);
int            ps_scene_push_context(Scene* scene, Context const* context);
int            ps_scene_pop_context(Scene* scene);

int ps_scene_add_image(Scene* scene, resource_idx_t resource_id, int x, int y);

int ps_scene_finalize(Scene* scene);

#endif //SCENE_H
