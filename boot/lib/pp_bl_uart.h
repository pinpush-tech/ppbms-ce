#pragma once
#include "stdint.h"

void pp_bl_uart_tx(uint8_t * data, uint16_t len);
void pp_bl_uart_init(pp_drv_uart_cb_t recv_cb);
