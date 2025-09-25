#pragma once
#include "stdint.h"
#include "pp_cfg.h"

typedef struct {
    uint32_t tmp_bat_id;
	char hard_version[20]; // pcb.prd.0.ver
} pp_data_burn_t;

extern pp_data_burn_t _burn;
