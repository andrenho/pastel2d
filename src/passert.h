#ifndef PASSERT_H
#define PASSERT_H

#include <stddef.h>
#include <sys/types.h>

#include "res.h"

int       ps_assert(int f, const char* function, const char* filename, int line, const char* request);
ptrdiff_t ps_assert_not_null(ptrdiff_t f, const char* function, const char* filename, int line, const char* request);

#define PS_ASRT(f) (ps_assert((f), __PRETTY_FUNCTION__, __FILE__, __LINE__, #f))
#define PS_ASRT_RES(f) ((resource_idx_t) ps_assert_not_null((ptrdiff_t) (f), __PRETTY_FUNCTION__, __FILE__, __LINE__, #f))
#define PS_ASRT_NOT_NULL(f) (ps_assert_not_null((ptrdiff_t) (f), __PRETTY_FUNCTION__, __FILE__, __LINE__, #f))

#endif //PASSERT_H
