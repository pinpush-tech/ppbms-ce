#pragma once

#include "stdint.h"

typedef struct {
    uint8_t index;
	uint8_t arg_num;
	int64_t arg1;
	int64_t arg2;
} pp_data_hostcmd_t;

extern pp_data_hostcmd_t _hostcmd;
