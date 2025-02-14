#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include <SDL3/SDL_rect.h>

typedef struct {
    bool       has_value;
    SDL_FPoint point;
} ContextPointValue;

typedef struct {
    bool     has_value;
    SDL_Rect rect;
} ContextRectValue;

typedef struct {
    bool  has_value;
    float value;
} ContextFloatValue;

typedef struct {
    ContextRectValue  position;
    ContextFloatValue rotation;
    ContextFloatValue zoom;
    ContextFloatValue opacity;
    ContextPointValue rotation_center;
} Context;

Context ps_create_context();
Context ps_context_sum(Context const* current, Context const* sum);

#endif //CONTEXT_H
