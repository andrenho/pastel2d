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
} ps_Context;

typedef enum {
    CTX_END = 0, CTX_POSITION = 1, CTX_ROTATION, CTX_ZOOM, CTX_OPACITY, CTX_ROT_CENTER,
} ps_ContextProperty;

ps_Context ps_create_context();
ps_Context ps_create_context_with_v(ps_ContextProperty props, va_list arg);
ps_Context ps_create_context_with(ps_ContextProperty props, ...);
ps_Context ps_context_sum(ps_Context const* current, ps_Context const* sum);

#endif //CONTEXT_H
