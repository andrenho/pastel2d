#include "context.h"

#include <stdarg.h>

ps_Context ps_create_context()
{
    return (ps_Context) {
        .position = { 0, 0, 0, 0 },
        .rotation = 0.f,
        .zoom = 1.f,
        .opacity = 1.f,
        .rotation_center = { DEFAULT_ROT_CENTER, DEFAULT_ROT_CENTER },
    };
}

ps_Context ps_create_context_with_v(ps_ContextProperty props, va_list ap)
{
    ps_Context ctx = ps_create_context();

    ps_ContextProperty prop = props;
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

        prop = va_arg(ap, ps_ContextProperty);
    }

    return ctx;
}

ps_Context ps_create_context_with(ps_ContextProperty props, ...)
{
    va_list ap;

    va_start(ap, props);
    ps_Context ctx = ps_create_context_with_v(props, ap);
    va_end(ap);

    return ctx;
}

ps_Context ps_context_sum(ps_Context const* current, ps_Context const* sum)
{
    ps_Context context = *current;

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
