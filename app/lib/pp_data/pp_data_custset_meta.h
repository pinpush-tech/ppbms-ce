#pragma once
#include "pp_cfg.h"
typedef struct {
    char meta[sizeof(PP_PCB_CUSTSET_META)];
} pp_data_custset_meta_t;

extern pp_data_custset_meta_t _custset_meta;


