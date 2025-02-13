#ifndef SCENE_H
#define SCENE_H

#define MAX_SCENES 16
#include "res.h"

typedef enum { A_IMAGE } ArtifactType;

typedef struct {
    resource_idx_t res_id;
    int            x, y;
} Image;

typedef struct {
    ArtifactType type;
    union {
        Image image;
    };
} Artifact;

typedef struct {
    Artifact* artifacts;
} Scene;

void ps_scene_add_image(Scene* scene, resource_idx_t resource_id, int x, int y);

void ps_scene_free(Scene* scene);

#endif //SCENE_H
