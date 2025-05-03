#ifndef PRIV_SCENE_H
#define PRIV_SCENE_H

typedef enum { A_IMAGE, A_TEXT } ps_ArtifactType;

typedef struct {
    ps_res_idx_t res_id;
    ps_Context   context;
} ps_Image;

typedef struct {
    ps_res_idx_t     font_idx;
    ps_Context       context;
    char*            text;
    int              font_size;
    SDL_Color        color;
} ps_Text;

typedef struct ps_Artifact {
    ps_ArtifactType type;
    union {
        ps_Image image;
        ps_Text  text;
    };
} ps_Artifact;

int ps_scene_finalize(ps_Scene* scene);

#endif //PRIV_SCENE_H
