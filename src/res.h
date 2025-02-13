#ifndef RES_H
#define RES_H

#include <stddef.h>
#include <stdint.h>

typedef size_t resource_idx_t;

#define RES_ERROR -1

resource_idx_t ps_res_add_png(uint8_t const* data, size_t sz);

#endif //RES_H
