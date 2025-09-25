#include "pp_data/pp_data_afe_cfg.h"
#include "pp_data/pp_data_afe.h"
#include "pp_afe/pp_afe_crt_calib.h"
#include "pp_afe_chip_cfg.h"
#include "pp_host/pp_host.h"
#include "pp_cfg.h"
#include "ulog.h"
#include "pp_export/pp_export_f.h"
#include "rtthread.h"
#include "pp_assert/pp_assert.h"
#include "pp_export/pp_export_m.h"
#include "pp_data/pp_data_export.h"

pp_data_afe_cfg_t _afe_cfg = {
    .cell_count = PP_BAT_CELL_COUNT,

    .fmah = PP_BAT_FMAH,
    .max_cyc = PP_BAT_MAX_CYC,

    .utc_th = PP_BAT_UTC_TH,
    .utd_th = PP_BAT_UTD_TH,
    .ut_dly = PP_BAT_UT_DLY,

    .otc_th = PP_BAT_OTC_TH,
    .otd_th = PP_BAT_OTD_TH,
    .ot_dly = PP_BAT_OT_DLY,

    .occ1_th = PP_BAT_OCC1_TH,
    .occ1_dly = PP_BAT_OCC1_DLY,

    .occ2_th = PP_BAT_OCC2_TH,
    .occ2_dly = PP_BAT_OCC2_DLY,

    .ocd1_th = PP_BAT_OCD1_TH,
    .ocd1_dly = PP_BAT_OCD1_DLY,

    .ocd2_th = PP_BAT_OCD2_TH,
    .ocd2_dly = PP_BAT_OCD2_DLY,

    .eoc_crrt = PP_BAT_EOC_CRRT,
    .eoc_th = PP_BAT_EOC_TH,
    .eocr_hys = PP_BAT_EOCR_HYS,
    .eoc_dly = PP_BAT_EOC_DLY,

    .cb1_th = PP_BAT_CB1_TH,
    .cb1_diff = PP_BAT_CB1_DIFF,

    .mot_th = PP_BAT_MOT_TH,
    .motr_hys = PP_BAT_MOTR_HYS,
    .mot_dly = PP_BAT_MOT_DLY,

    .scd_th = PP_BAT_SCD_TH,
    .scd_dly = PP_BAT_SCD_DLY,

    .uv1_th = PP_BAT_UV1_TH,
    .uvr_hys = PP_BAT_UVR_HYS,
    .uv1_dly = PP_BAT_UV1_DLY,

    .ov1_th = PP_BAT_OV1_TH,
    .ovr_hys = PP_BAT_OVR_HYS,
    .ov1_dly = PP_BAT_OV1_DLY,

    .ov2_th = PP_BAT_OV2_TH,
    .ov2_dly = PP_BAT_OV2_DLY,

    .uv2_th = PP_BAT_UV2_TH,
    .uv2_dly = PP_BAT_UV2_DLY,

    .vdif_th = PP_BAT_VDIF_TH,
    .vdif_hys = PP_BAT_VDIF_HYS,
    .vdif_dly = PP_BAT_VDIF_DLY,

    .zero_th = PP_BAT_ZERO_TH,

    .pdsg_time = PP_BAT_PDSG_TIME,
    .pdsg_scd_th =PP_BAT_PDSG_SCD_TH,

    .crt_rv = PP_PCB_AFE_CRT_RV,
    .crt_calib_w = PP_PCB_AFE_CRT_CALIB_WEIGHT,
    .crt_calib_b = PP_PCB_AFE_CRT_CALIB_BIAS,
    .chgm_fail_dly = PP_PCB_AFE_CHGM_FAIL_DLY,
    .dsgm_fail_dly = PP_PCB_AFE_DSGM_FAIL_DLY,
    .afe_fail_dly = PP_PCB_AFE_AFE_FAIL_DLY,
};

uint8_t _check_range_i(int64_t value, int64_t min, int64_t max) {
    if (value < min || value > max) return 1;
    return 0;
}
uint8_t _check_range_d(double value, double min, double max) {
    if (value < min || value > max) return 1;
    return 0;
}

static int _check_afe_cfg(void) {
    int32_t res = _data_check_afe_cfg(&_afe_cfg);
    if (res != 0) {
        _afe.afe_cfg_err = 1;
        LOG_E("_afe_cfg check  error code is %d", res);
    }
    return 0;
}
INIT_ENV_EXPORT(_check_afe_cfg);

static int32_t _afecfg_hostpre(void * args) {
    PP_ASSERT(args != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *) args;
    PP_ASSERT(handle->recv_head != RT_NULL);

    if (handle->recv_head->opr == 015 && handle->recv_head->index == PP_DATA_EXPORT_ENV_AFE_CFG_SH) {
        PP_ASSERT(handle->pre_data != RT_NULL);
        return _data_check_afe_cfg((pp_data_afe_cfg_t *) handle->pre_data);
    }
    return 0;
}
PP_HOSTPRE_EXPORT_F(_afecfg_hostpre);
