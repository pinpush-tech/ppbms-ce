#pragma once
#include <stdint.h>

typedef struct pp_data_custset {
    uint8_t bootcomm;
    uint8_t setitem0;
    uint8_t setitem1;
}pp_data_custset_t;

extern pp_data_custset_t _custset;

