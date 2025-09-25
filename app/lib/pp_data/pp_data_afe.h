#pragma once
#include "stdint.h"
#include "pp_cfg.h"
#include "pp_data/pp_data_afe_status.h"

#define PP_DATA_AFE_STATUS_LENTH    60

typedef struct {
    uint8_t por_flag;
    uint8_t chgrin_flag;
    uint8_t ldon_flag;
    uint8_t chgroff_flag;
    uint8_t ldoff_flag;
    uint8_t pdsg_mos_off_flag;

    uint8_t __status_head__;
    _AFE_STATES_FILEDS
    uint8_t __status_tail__;

    uint8_t maxv_cell_ch; // ch[r]
    uint8_t minv_cell_ch; // ch[r]

    int32_t crt; // ch[r]
    int32_t crt_original; // ch[r]
    int16_t cell0_temp;
    int16_t cell1_temp;
    int16_t cell_min_temp; // ch[r]
    int16_t cell_max_temp; // ch[r]
    int16_t cell_avg_temp; // ch[r]
    int16_t mos_temp; // ch[r]
    int16_t chip_temp; // ch[r]

    int32_t bat_vol; // ch[r]
    int16_t cell_vol_1; // ch[r]
    int16_t cell_vol_2; // ch[r]
    int16_t cell_vol_3; // ch[r]
    int16_t cell_vol_4; // ch[r]
    int16_t cell_vol_5; // ch[r]
    int16_t cell_vol_6; // ch[r]
    int16_t cell_vol_7; // ch[r]
    int16_t cell_vol_8; // ch[r]
    int16_t cell_vol_9; // ch[r]
    int16_t cell_vol_10; // ch[r]
    int16_t cell_vol_11; // ch[r]
    int16_t cell_vol_12; // ch[r]
    int16_t cell_vol_13; // ch[r]
    int16_t cell_vol_14; // ch[r]
    int16_t cell_vol_15; // ch[r]
    int16_t cell_vol_16; // ch[r]
    int16_t cell_vol_17; // ch[r]
    int16_t cell_vol_18; // ch[r]
    int16_t cell_vol_19; // ch[r]
    int16_t cell_vol_20; // ch[r]
    int16_t cell_vol_21; // ch[r]
    int16_t cell_vol_22; // ch[r]
    int16_t cell_vol_23; // ch[r]
    int16_t cell_vol_24; // ch[r]
    int16_t cell_max_vol; // ch[r]
    int16_t cell_min_vol; // ch[r]
    uint32_t load_vol;
    uint8_t general_adc_req_ch;

    uint8_t afe_comm_err;
    uint8_t afe_has_data;
    uint8_t afe_has_flag;
    uint8_t afe_times;
    int16_t afe_cyc_time;

    char afe_status[PP_DATA_AFE_STATUS_LENTH]; // ch[r]

    uint8_t bat_status; // ch[r]
    rt_uint32_t cb_cell_en;

    uint8_t sys_mos_lock[3];
    uint8_t dsg_mos_ctl;
    uint8_t chg_mos_ctl;
    uint8_t pdsg_mos_ctl;
} pp_data_afe_t;

extern pp_data_afe_t _afe;
