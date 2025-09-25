#pragma once
#include "stdint.h"

void pp_drv_gpio_init(uint8_t id);
void pp_drv_gpio_deinit(uint8_t id);
void pp_drv_gpio_write(uint8_t id, uint8_t level);
