#include "context.h"

#include <stdarg.h>

Context ps_create_context()
{
    return (Context) {
        .position = { 0, 0, 0, 0 },
        .rotation = 0.f,
        .zoom = 1.f,
        .opacity = 1.f,
        .rotation_center = { DEFAULT_ROT_CENTER, DEFAULT_ROT_CENTER },
    };
}

Context ps_create_context_with_v(ContextProperty props, va_list ap)
{
    Context ctx = ps_create_context();

    ContextProperty prop = props;
    while (1) {
        if (prop == CTX_END)
            break;
        switch (prop) {
            case CTX_POSITION:   ctx.position = va_arg(ap, SDL_Rect); break;
            case CTX_ROTATION:   ctx.rotation = va_arg(ap, double); break;
            case CTX_ZOOM:       ctx.zoom = va_arg(ap, double); break;
            case CTX_OPACITY:    ctx.opacity = va_arg(ap, double); break;
            case CTX_ROT_CENTER: ctx.rotation_center = va_arg(ap, SDL_FPoint); break;
            case CTX_END: break;
        }

        prop = va_arg(ap, ContextProperty);
    }

    return ctx;
}

Context ps_create_context_with(ContextProperty props, ...)
{
    va_list ap;

    va_start(ap, props);
    Context ctx = ps_create_context_with_v(props, ap);
    va_end(ap);

    return ctx;
}

Context ps_context_sum(Context const* current, Context const* sum)
{
    Context context = *current;

    context.position.x += sum->position.x;
    context.position.y += sum->position.y;
    if (sum->position.w != 0) {
        context.position.w = sum->position.w;
        context.position.h = sum->position.h;
    }

    context.rotation += sum->rotation;

    context.zoom *= sum->zoom;
    context.opacity *= sum->opacity;

    if (sum->rotation_center.x != DEFAULT_ROT_CENTER)
        context.rotation_center = sum->rotation_center;

    return context;
}
