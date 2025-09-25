#include "pp_data/pp_data_sys.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_afe_cfg.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "pp_assert/pp_assert.h"
#include "pp_continued/pp_continued.h"
#include "ulog.h"
#include "pp_afe/pp_afe.h"
#include "pp_afe/pp_afe_chip.h"
#include "pp_afe/pp_afe_soft.h"
#include "pp_afe_chip_cfg.h"
#include "pp_log/pp_log.h"

void pp_afe_soft_opr4flag(void) {
    if (_afe.por_flag) {
        pp_afe_soft_init();
    }
}

void pp_afe_soft_opr4data(void) {
#ifdef PP_PCB_USING_AFE_PDSG_SCD
    _AFE_MOS_OPR4STATUS(pdsg_scd, 1, 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_VDIF
    _AFE_MOS_OPR4STATUS(vdif, 1, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OHT
    _AFE_MOS_OPR4STATUS(oht, 1, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_UTD
    _AFE_MOS_OPR4STATUS(utd, 1, 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_UTC
    _AFE_MOS_OPR4STATUS(utc, 0, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OTC
    _AFE_MOS_OPR4STATUS(otc, 0, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OTD
    _AFE_MOS_OPR4STATUS(otd, 1, 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OV2
    _AFE_MOS_OPR4STATUS(ov2, 0, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_UV2
    _AFE_MOS_OPR4STATUS(uv2, 1, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_MOT
    _AFE_MOS_OPR4STATUS(mot, 1, 1);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CB
    PP_ASSERT(_afe_cfg.cell_count <= PP_PCB_AFE_MAX_CH);
    rt_uint32_t cb_cell_en = 0;
    for(uint8_t i = 0; _afe.bat_status != 1 && i < _afe_cfg.cell_count; i++) {
        if ( (&_afe.cell_vol_1)[i] > _afe_cfg.cb1_th && (&_afe.cell_vol_1)[i] - _afe.cell_min_vol > _afe_cfg.cb1_diff) {
            cb_cell_en |= (1 << i);
        }
    }
    _afe.cb_cell_en = cb_cell_en;
#endif
#ifdef PP_PCB_USING_AFE_FUSE
    if (_afe.dsgm_fail_status == 1 || _afe.chgm_fail_status == 1 || _afe.afe_fail_status == 1 || _afe.ov2_status == 1) {
        if (_afe.fuse_status == 0) {
            pp_log_F_afe("blow_fuse");
            _afe.fuse_status = 1;
            _sys.store_flag = 1;
        }
#ifndef PP_PCB_CTX_DEBUG
        rt_pin_write(PP_PCB_AFE_FUSE_PIN, PIN_HIGH);
#endif
    }
#endif
#ifdef PP_PCB_USING_AFE_AUTO_MOS_CTRL
    if ( !_afe_cfg.smart_mos_off_flag) {
#ifdef PP_PCB_USING_AFE_SEPARATE_PORT
        if (PP_CONTINUED(_afe.dsg_mos_status == 1 && _afe.bat_status != 1, 60 * 1000)) {
            pp_afe_mos_ctl(0, -1, -1, 0);
        }
        if (PP_CONTINUED(_afe.chg_mos_status == 1 && _afe.bat_status != 2, 60 * 1000)) {
            pp_afe_mos_ctl(-1, 0, -1, 0);
        }
#else
        if (PP_CONTINUED((_afe.chg_mos_status == 1 || _afe.dsg_mos_status == 1) && _afe.bat_status == 0, 60 * 1000)) {
            pp_afe_mos_ctl(0, 0, -1, 0);
        }
#endif
        if(_afe.chgrin_flag == 1) {
            pp_afe_mos_ctl(-1, 1, -1, 0);
        }
        if (_afe.pdsg_mos_off_flag == 0 && _afe.ldon_flag == 1) {
            pp_afe_mos_ctl(-1, -1, 1, 0);
        }
        if(_afe.pdsg_mos_off_flag == 1) {
            pp_afe_mos_ctl(1, -1, -1, 0);
        }
    } else {
        if (_sys.first_cycle_flag == 1)
            pp_afe_mos_ctl(1, 1, -1, 0);
    }
#endif
}