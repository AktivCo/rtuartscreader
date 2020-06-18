#pragma once

#include <stdint.h>

typedef struct f_d_index {
    uint8_t f_index;
    uint8_t d_index;
} f_d_index_t;

extern const f_d_index_t f_d_index_default;