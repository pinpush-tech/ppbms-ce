#include "pp_data/pp_data_afe.h"
#include "pp_afe/pp_afe_crt_calib.h"
#include "pp_afe_chip_cfg.h"
#include "pp_host/pp_host.h"
#include "pp_cfg.h"
#include "ulog.h"
#include "pp_export/pp_export_f.h"
#include "rtthread.h"
#include "pp_export/pp_export_m.h"

pp_data_afe_t _afe = {
    .afe_times = 1,
    .afe_cyc_time = PP_AFE_CHIP_CYC_TIME,
    .afe_status = {
        [0 ... PP_DATA_AFE_STATUS_LENTH - 2] = '0' ,
        [      PP_DATA_AFE_STATUS_LENTH - 1] = '\0',
    },
};
