#pragma once
#include "stdint.h"

int pp_mcu_flash_init(void);
int pp_mcu_flash_read(long offset, uint8_t *buf, uint32_t size);
int pp_mcu_flash_write(long offset, const uint8_t *buf, uint32_t size);
int pp_mcu_flash_erase(long offset, uint32_t size);
