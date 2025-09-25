#pragma once
#include "stdint.h"

uint16_t pp_tools_crc16(uint8_t *pdata, uint16_t n);
uint8_t pp_tools_crc8(uint8_t *pdata, uint16_t n);

uint8_t pp_check_sum(uint8_t *data, uint8_t length);
void pp_TEA_code(volatile long *data, volatile long *key);
void pp_TEA_decode(volatile long *data, volatile long *key);
