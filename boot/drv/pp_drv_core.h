#pragma once
#include "stdint.h"

typedef void(*pp_drv_core_cb_t)(void);

void pp_core_jump(pp_drv_core_cb_t cb);
