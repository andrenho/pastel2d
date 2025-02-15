#ifndef CONTEXT_H
#define CONTEXT_H

#include <float.h>
#include <SDL3/SDL_rect.h>

#define DEFAULT_ROT_CENTER FLT_MIN

typedef struct {
    SDL_Rect   position;
    float      rotation;
    float      zoom;
    float      opacity;
    SDL_FPoint rotation_center;
} Context;

typedef enum {
    CTX_END = 0, CTX_POSITION = 1, CTX_ROTATION, CTX_ZOOM, CTX_OPACITY, CTX_ROT_CENTER,
} ContextProperty;

Context ps_create_context();
Context ps_create_context_with(ContextProperty props, ...);
Context ps_context_sum(Context const* current, Context const* sum);

#endif //CONTEXT_H
