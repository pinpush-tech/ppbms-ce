#pragma once
#include "stdint.h"

#define     PP_BL_HOST_STATE_WAITING          0
#define     PP_BL_HOST_STATE_FOTAING          1
#define     PP_BL_HOST_STATE_FOTADONE         2
#define     PP_BL_HOST_STATE_TIMEOUT          3

extern volatile uint8_t pp_bl_host_fota_state;

void pp_bl_host_process(void);
void pp_bl_host_init(void);
void pp_bl_host_deinit(void);
