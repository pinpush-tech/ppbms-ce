#pragma once

#include "stdint.h"
#include "rtthread.h"

void pp_afe_mos_ctl(int8_t dsg_enable, int8_t chg_enable, int8_t pdsg_enable, int8_t type);
void pp_afe_shut_down(void);
