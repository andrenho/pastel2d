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
    }

    if (sum->alignment.has_value) {
        context.alignment.has_value = true;
        context.alignment.rect.x += sum->alignment.rect.x;
        context.alignment.rect.y += sum->alignment.rect.y;
    }

    if (sum->rotation.has_value) {
        context.rotation.has_value = true;
        context.rotation.value += sum->rotation.value;
    }

    if (sum->zoom.has_value) {
        context.zoom.has_value = true;
        context.zoom.value += sum->zoom.value;
    }

    if (sum->zoom.has_value) {
        context.opacity.has_value = true;
        context.opacity.value = sum->opacity.value;
    }

    return context;
}
