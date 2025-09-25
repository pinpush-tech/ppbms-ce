#pragma once

#define SOC_N32G43X

#include "pp_pcb_app_cfg.h"
#include "pp_pcb_rt_cfg.h"

#define PP_PCB_MCU_FLASH_SIZE                       (128 * 1024)
#define PP_PCB_MCU_SRAM_SIZE                        (32)

#define PP_PCB_AFE_MAX_CH                       14
#define PP_PCB_AFE_I2C_NAME                     "i2c2"
#define PP_PCB_AFE_I2C_SCL_PIN                  GET_PIN(B, 10)
#define PP_PCB_AFE_I2C_SDA_PIN                  GET_PIN(B, 11)
#undef PP_PCB_USING_AFE_INT_WK
#undef PP_PCB_USING_AFE_FUSE
#undef PP_PCB_USING_SYS_SHUTDOWN

#define PP_PCB_CUSTSET_META  "[[\\\"bootcomm\\\",\\\"setitem0\\\",\\\"setitem1\\\"],[1,1,1],[\\\"Uint8\\\",\\\"Uint8\\\",\\\"Uint8\\\"],[\\\"Boot通信\\\"],[],[[[1,\\\"usart1\\\"]]],[],[],[]]"
#define PP_PCB_LED_PIN                          GET_PIN(A, 8)