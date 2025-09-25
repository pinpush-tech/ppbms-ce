#pragma once
#include "stdint.h"

typedef void(*pp_drv_uart_cb_t)(uint8_t data);

void pp_drv_uart_init(uint32_t * user_data, pp_drv_uart_cb_t uart_cb);
void pp_drv_uart_deinit(void);
void pp_drv_uart_tx(uint8_t* array, uint16_t num);
