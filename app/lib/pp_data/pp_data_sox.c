#include "pp_data/pp_data_sox.h"
#include "pp_data/pp_data_afe.h"
#include "pp_cfg.h"
#include "rtthread.h"
#include "pp_export/pp_export_m.h"

pp_data_sox_t _sox = {
    .rmah_pct = 0XFF, // 代表未被初始化
    .soh = 100
};

pp_data_sox_cfg_t _sox_cfg = {
#ifdef PP_BAT_OCV_SHEET
    .ocv_sheet = PP_BAT_OCV_SHEET,
#endif
    .tf_curve_tmpr = PP_BAT_TF_CURVE_TMPR,
    .tf_curve_fmah = PP_BAT_TF_CURVE_FMAH,
};
