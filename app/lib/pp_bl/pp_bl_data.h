#pragma once
#include "pp_cfg.h"
#include "stdint.h"

#define PP_BL_DATA_FOTA_WAIT_TIME   3000

typedef struct {
    uint8_t         sota_flag;
    uint32_t        dl_offset;
    uint32_t        app_size;
    uint8_t         comm_cfg_flag;
    uint16_t        fota_wait_time;
    uint8_t         gpio_ids[4];
    uint8_t         gpio_level[4];
    uint8_t         dev_type; // 0 uart; 1 can
    uint8_t         uart_is_single;
    uint8_t         rs_485_mode_gpio_id;
    uint32_t        user_data[64];
} pp_bl_data_t;

extern pp_bl_data_t pp_bl_data;

#ifndef PP_PCB_CTX_APP
void pp_bl_data_init(void);
void pp_bl_data_deinit(void);
#endif