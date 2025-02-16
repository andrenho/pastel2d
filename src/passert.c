#include "passert.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL_messagebox.h>

#include "error.h"
#include "graphics.h"

int ps_assert(int f, const char* function, const char* filename, int line, const char* request)
{
    if (f != 0) {
        char buf[4096];
        snprintf(buf, sizeof buf, "%s\n\n%s\n\nat %s:%d\n\nFunction: %s",
            request, ps_last_error(), filename, line, function);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Assertion failed!", buf, ps_graphics_window());
        abort();
    }
    return f;
}

ptrdiff_t ps_assert_not_null(ptrdiff_t f, const char* function, const char* filename, int line, const char* request)
{
    if (f == 0) {
        char buf[4096];
        snprintf(buf, sizeof buf, "%s\n\n%s\n\nat %s:%d\n\nFunction: %s",
            request, ps_last_error(), filename, line, function);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Assertion failed!", buf, ps_graphics_window());
        abort();
    }
    return f;
}
