#pragma once
#include "stdint.h"
#include "pp_cfg.h"

/**
 * 1. 时间类的参数都不允许改变
 * 2. 运行时修改的参数：a在启动时检查；b在修改前检查
 * 3. 编译时修改的参数：在初始化时兜底
 */
typedef struct {
    uint8_t cell_count; // sch[wr]

    rt_uint32_t fmah; // sch[wr]
    uint16_t max_cyc; // c

    int8_t utc_th; // sch[wr]
    int8_t utd_th; // sch[wr]
    uint16_t ut_dly; // c

    int8_t otc_th; // sch[wr]
    int8_t otd_th; // sch[wr]
    uint16_t ot_dly; // c

    rt_uint32_t occ1_th; // sch[wr]
    uint16_t occ1_dly; // c

    rt_uint32_t occ2_th; // sch[wr]
    uint16_t occ2_dly; // c

    rt_uint32_t ocd1_th; // sch[wr]
    uint16_t ocd1_dly; // c

    rt_uint32_t ocd2_th; // sch[wr]
    uint16_t ocd2_dly; // c

    rt_uint32_t eoc_crrt; // sch[wr]
    uint16_t eoc_th; // sch[wr]
    uint16_t eocr_hys; // sch[wr]
    uint16_t eoc_dly; // c

    uint16_t cb1_th; // c
    uint16_t cb1_diff; // c

    int8_t mot_th; // c
    int8_t motr_hys; // c
    uint16_t mot_dly; // c

    rt_uint32_t scd_th; // sch[wr]
    uint16_t scd_dly; // sch[wr]

    uint16_t uv1_th; // sch[wr]
    uint16_t uvr_hys; // sch[wr]
    uint16_t uv1_dly; // c

    uint16_t ov1_th; // sch[wr]
    uint16_t ovr_hys; // sch[wr]
    uint16_t ov1_dly; // c

    uint16_t ov2_th; // c
    uint16_t ov2_dly; // c

    uint16_t uv2_th; // c
    uint16_t uv2_dly; // c

    uint16_t vdif_th; // sch[wr]
    uint16_t vdif_hys; // sch[wr]
    uint16_t vdif_dly; // c

    int32_t zero_th; // c

    uint16_t pdsg_time; // sch[wr]
    uint8_t pdsg_scd_th; // c
    uint8_t smart_mos_off_flag; // sch[rw]
    uint8_t shutdown_off_flag; // sch[rw]

    double crt_rv; // ch[r]

    double crt_calib_w; // ch[r]
    int32_t crt_calib_b; // ch[r]
    uint32_t chgm_fail_dly;
    uint32_t dsgm_fail_dly;
    uint32_t afe_fail_dly;
} pp_data_afe_cfg_t;

extern pp_data_afe_cfg_t _afe_cfg;

uint8_t _check_range_i(int64_t value, int64_t min, int64_t max);
uint8_t _check_range_d(double value, double min, double max);
int32_t _data_check_afe_cfg(pp_data_afe_cfg_t * afe_cfg);
