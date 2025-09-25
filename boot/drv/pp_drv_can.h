#pragma once
#include "stdint.h"

typedef void (*pp_drv_can_rx_cb_t)(uint8_t data);

void pp_drv_can_init(uint32_t * user_data, pp_drv_can_rx_cb_t can_cb);
void pp_drv_can_deinit(void);
void pp_drv_can_tx(uint8_t * data, uint16_t len);
