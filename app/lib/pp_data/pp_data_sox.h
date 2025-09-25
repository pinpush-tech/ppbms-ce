#pragma once
#include "stdint.h"
#include "pp_cfg.h"

typedef struct {
    uint8_t soh; // sch[r]
    uint8_t fmah_lrn_flag; // sch[r]
    uint8_t ff_100_0_cf_flag; // sch[r]
    uint8_t ff_100_0_dsg_flag; // sch[r]
    uint8_t ff_x_100_x; // sch[r]
    uint8_t ff_x_100_type; // sch[r]
    float ff_x_100_dsg_mah; // sch[r]
    float ff_x_100_chg_mah; // sch[r]
    float comp_mah; // sch[r]
    float fmah; // sch[r]
    uint8_t rmah_pct; // sch[r]
    int32_t mah_t1; // ch[r]
    int32_t mah_t2; // ch[r]
    int32_t rmah; // sch[r]
    int32_t cyc_dsg_mah; // sch[r]
    uint16_t cyc_cnt; // sch[r]
} pp_data_sox_t;

typedef struct {
    int8_t        tf_curve_tmpr[5];
    float         tf_curve_fmah[5];
#ifdef PP_BAT_OCV_SHEET
    uint16_t      ocv_sheet[100];
#endif
} pp_data_sox_cfg_t;

extern pp_data_sox_t _sox;
extern pp_data_sox_cfg_t _sox_cfg;
