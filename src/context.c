#include "context.h"

Context ps_create_context()
{
    return (Context) {};
}

Context ps_context_sum(Context const* current, Context const* sum)
{
    Context context = *current;

    if (sum->position.has_value) {
        context.position.has_value = true;
        context.position.rect.x += sum->position.rect.x;
        context.position.rect.y += sum->position.rect.y;
        context.position.rect.w = sum->position.rect.w;
        context.position.rect.h = sum->position.rect.h;
    }

    if (sum->rotation.has_value) {
        context.rotation.has_value = true;
        context.rotation.value += sum->rotation.value;
    }

    if (sum->zoom.has_value) {
        context.zoom.has_value = true;
        context.zoom.value += sum->zoom.value;
    }

    if (sum->opacity.has_value) {
        context.opacity.has_value = true;
        context.opacity.value = sum->opacity.value;
    }

    if (sum->rotation_center.has_value) {
        context.rotation_center.has_value = true;
        context.rotation_center.point = sum->rotation_center.point;
    }

    return context;
}
