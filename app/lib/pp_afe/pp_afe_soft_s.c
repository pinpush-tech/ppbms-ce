#include "stdint.h"
#include "string.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_afe_cfg.h"
#include "pp_afe/pp_afe_chip.h"
#include "pp_afe_chip_cfg.h"
#include "pp_afe/pp_afe.h"
#include "pp_continued/pp_continued.h"
#include "pp_data/pp_data_sys.h"
#include "ulog.h"
#include "pp_log/pp_log.h"
#include "pp_assert/pp_assert.h"

void pp_afe_soft_set4flag(void) {
}

#define _SOFT_DET(n, s, c, d, t, r) \
    do { \
        static int32_t tms_##n = 0; \
        static int32_t rms_##n = 0; \
        _status_set(#n, &_afe.n##_status, (t), (r), &tms_##n, &rms_##n, (d), (s), (c)); \
    } while (0)
static void _status_set(char * name, uint8_t *status, uint8_t trig, uint8_t rls, int32_t * tms_p, int32_t * rms_p, int32_t delay_ms, uint8_t is_store, uint8_t ch) {
    PP_ASSERT(name != RT_NULL);
    PP_ASSERT(status != RT_NULL);

    if (pp_continued(tms_p, *status == 0 && trig, delay_ms)) {
        *status = 1;
        if (is_store) _sys.store_flag = 1;
        _afe_status_log(name, 1, ch);
    }
    if (pp_continued(rms_p, *status == 1 && rls, delay_ms)) {
        *status = 0;
        if (is_store) _sys.store_flag = 1;
        _afe_status_log(name, 0, ch);
    }
}
static void _write_afe_status(void) {
    PP_ASSERT(&_afe.__status_tail__ >= &_afe.__status_head__);
    uint8_t status_len = &_afe.__status_tail__ - &_afe.__status_head__  - 1;
    PP_ASSERT(status_len < PP_DATA_AFE_STATUS_LENTH - 1);
    for (uint8_t i = 0; i < status_len; i++) {
        _afe.afe_status[i] = (&_afe.__status_head__)[1 + i]?'1':'0';
    }
    _afe.afe_status[status_len] = 0;
}

static uint8_t _is_protection(uint8_t * begin, uint8_t * end) {
    PP_ASSERT(begin != RT_NULL);
    PP_ASSERT(end != RT_NULL);
    PP_ASSERT(end >= begin);

    uint8_t status_len = end - begin - 1;
    for (uint8_t i = 0; i < status_len; i++) {
        if (begin[1 + i] != 0) return 1;
    }
    return 0;
}

void pp_afe_soft_set4data(void) {
    _SOFT_DET(afe_fail, 1, 0, _afe_cfg.afe_fail_dly,
        _afe.afe_fail_status == 0 && _afe.afe_comm_err == 1,
        0);

    // 状态
    if (_afe.pdsg_mos_status == 1 || (_afe.crt < _afe_cfg.zero_th && _afe.crt >  -1 * _afe_cfg.zero_th )) {
        _afe.bat_status = 0;
    } else if (_afe.crt < 0) {
        _afe.bat_status = 1;
    } else {
        _afe.bat_status = 2;
    }
    _afe.occ_status = _afe.occ2_status | _afe.occ1_status;
    _afe.ocd_status = _afe.ocd2_status | _afe.ocd1_status;

    // flag 依赖 data
#ifdef PP_AFE_CHIP_SOFT_CHGRIN_FLAG_CHK
    _afe.chgrin_flag = 0;
    if (_AFE_STATUS_CHANGED(chgrin)) {
        if (_afe.chgrin_status == 1) _afe.chgrin_flag = 1;
    }
#endif
#ifdef PP_AFE_CHIP_SOFT_CHGROFF_FLAG_CHK
    _afe.chgroff_flag = 0;
    if (_AFE_STATUS_CHANGED(chgroff)) {
        if (_afe.chgroff_status == 1) _afe.chgroff_flag = 1;
    }
#endif
#ifdef PP_AFE_CHIP_SOFT_LDON_FLAG_CHK
    _afe.ldon_flag = 0;
    if (_AFE_STATUS_CHANGED(ldon)) {
        if (_afe.ldon_status == 1) _afe.ldon_flag = 1;
    }
#endif
#ifdef PP_AFE_CHIP_SOFT_LDOFF_FLAG_CHK
    _afe.ldoff_flag = 0;
    if (_AFE_STATUS_CHANGED(ldoff)) {
        if (_afe.ldoff_status == 1) _afe.ldoff_flag = 1;
    }
#endif
    _afe.pdsg_mos_off_flag = 0;
    if (_AFE_STATUS_CHANGED(pdsg_mos)) {
        if (_afe.pdsg_mos_status == 0) _afe.pdsg_mos_off_flag = 1;
    }
#ifdef PP_PCB_USING_AFE_PDSG_SCD
    _SOFT_DET(pdsg_scd, 1 ,0, 0,
        _afe.pdsg_mos_off_flag && _afe.load_vol < _afe.bat_vol * _afe_cfg.pdsg_scd_th / 100.0,
        _afe.ldoff_status == 1);
    if(_afe.pdsg_mos_off_flag == 1) {
        _afe.load_vol = 0;
    }
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_VDIF
    _SOFT_DET(vdif, 1, 1, _afe_cfg.vdif_dly,
        _afe.cell_max_vol - _afe.cell_min_vol > _afe_cfg.vdif_th,
        _afe.cell_max_vol - _afe.cell_min_vol < _afe_cfg.vdif_th - _afe_cfg.vdif_hys);
#endif
    _SOFT_DET(chgm_fail, 1, 0, _afe_cfg.chgm_fail_dly,
        _afe.pchg_mos_status == 0 && _afe.chg_mos_status == 0 &&  _afe.crt > (_afe_cfg.zero_th * 10),
        0);
#ifdef PP_PCB_USING_AFE_DSGM_FAIL
    _SOFT_DET(dsgm_fail, 1, 0, _afe_cfg.dsgm_fail_dly,
        _afe.pdsg_mos_status == 0 && _afe.dsg_mos_status == 0 &&  _afe.crt < (_afe_cfg.zero_th * -10),
        0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OHT
    _SOFT_DET(oht, 1, 0, _afe_cfg.ot_dly,
        _afe.chip_temp > PP_AFE_CHIP_SOFT_OHT_TH,
        _afe.chip_temp < PP_AFE_CHIP_SOFT_OHT_TH - PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL0_UTD
    _SOFT_DET(cell0_utd, 0, 0, _afe_cfg.ut_dly,
        _afe.cell0_temp < _afe_cfg.utd_th,
        _afe.cell0_temp > _afe_cfg.utd_th + PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL0_UTC
    _SOFT_DET(cell0_utc, 0, 0, _afe_cfg.ut_dly,
        _afe.cell0_temp < _afe_cfg.utc_th,
        _afe.cell0_temp > _afe_cfg.utc_th + PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL0_OTC
    _SOFT_DET(cell0_otc, 0, 0, _afe_cfg.ot_dly,
        _afe.cell0_temp > _afe_cfg.otc_th,
        _afe.cell0_temp < _afe_cfg.otc_th - PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL0_OTD
    _SOFT_DET(cell0_otd, 0, 0, _afe_cfg.ot_dly,
        _afe.cell0_temp > _afe_cfg.otd_th,
        _afe.cell0_temp < _afe_cfg.otd_th - PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL1_UTD
    _SOFT_DET(cell1_utd, 0, 0, _afe_cfg.ut_dly,
        _afe.cell1_temp < _afe_cfg.utd_th,
        _afe.cell1_temp > _afe_cfg.utd_th + PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL1_UTC
    _SOFT_DET(cell1_utc, 0, 0, _afe_cfg.ut_dly,
        _afe.cell1_temp < _afe_cfg.utc_th,
        _afe.cell1_temp > _afe_cfg.utc_th + PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL1_OTC
    _SOFT_DET(cell1_otc, 0, 0, _afe_cfg.ot_dly,
        _afe.cell1_temp > _afe_cfg.otc_th,
        _afe.cell1_temp < _afe_cfg.otc_th - PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_CELL1_OTD
    _SOFT_DET(cell1_otd, 0, 0, _afe_cfg.ot_dly,
        _afe.cell1_temp > _afe_cfg.otd_th,
        _afe.cell1_temp < _afe_cfg.otd_th - PP_AFE_CHIP_SOFT_TEMP_HYS);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_UTD
    _SOFT_DET(utd, 1, 0, 0,
        _afe.cell1_utd_status | _afe.cell0_utd_status,
        _afe.cell1_utd_status == 0 && _afe.cell0_utd_status == 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_UTC
    _SOFT_DET(utc, 1, 0, 0,
        _afe.cell1_utc_status | _afe.cell0_utc_status,
        _afe.cell1_utc_status == 0 && _afe.cell0_utc_status == 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OTC
    _SOFT_DET(otc, 1, 0, 0,
        _afe.cell1_otc_status | _afe.cell0_otc_status,
        _afe.cell1_otc_status == 0 && _afe.cell0_otc_status == 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OTD
    _SOFT_DET(otd, 1, 0, 0,
        _afe.cell1_otd_status | _afe.cell0_otd_status,
        _afe.cell1_otd_status == 0 && _afe.cell0_otd_status == 0);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_OV2
    _SOFT_DET(ov2, 1, 1, _afe_cfg.ov2_dly,
        _afe.cell_max_vol > _afe_cfg.ov2_th,
        _afe.cell_max_vol < _afe_cfg.ov2_th - _afe_cfg.ovr_hys);
#endif
#ifdef PP_AFE_CHIP_SOFT_PRT_UV2
    _SOFT_DET(uv2, 1, 1, _afe_cfg.uv2_dly,
        _afe.cell_min_vol < _afe_cfg.uv2_th,
        _afe.cell_min_vol > _afe_cfg.uv2_th + _afe_cfg.uvr_hys);
#endif
#if defined(PP_AFE_CHIP_SOFT_PRT_MOT) && defined(PP_PCB_USING_AFE_MOT)
    _SOFT_DET(mot, 1, 0, _afe_cfg.mot_dly,
        _afe.mos_temp > _afe_cfg.mot_th,
        _afe.mos_temp < _afe_cfg.mot_th - _afe_cfg.motr_hys);
#endif
    // 数据
    if (  _is_protection(&_afe.__status_chdsg_begin__, &_afe.__status_chdsg_end__)
        | _is_protection(&_afe.__status_chg_begin__  , &_afe.__status_chg_end__  )
    ) {
        _afe.can_chg = 0;
    } else {
        _afe.can_chg = 1;
    }
    if (  _is_protection(&_afe.__status_chdsg_begin__, &_afe.__status_chdsg_end__)
        | _is_protection(&_afe.__status_dsg_begin__  , &_afe.__status_dsg_end__  )
    ) {
        _afe.can_dsg = 0;
    } else {
        _afe.can_dsg = 1;
    }
    _write_afe_status();
    // 其他
    if ( _afe.ldon_flag || _afe.chgrin_flag || _afe.ldoff_flag || _afe.chgroff_flag)
    LOG_I("ldon_flag: %d, chgrin_flag: %d, ldoff_flag: %d, chgroff_flag: %d", \
    _afe.ldon_flag, _afe.chgrin_flag, _afe.ldoff_flag, _afe.chgroff_flag);

    if(_AFE_STATUS_CHANGED(pdsg_mos) ||
       _AFE_STATUS_CHANGED(pchg_mos) ||
       _AFE_STATUS_CHANGED(dsg_mos) ||
       _AFE_STATUS_CHANGED(chg_mos) ) {
        LOG_I("%s  %s  %s  %s",
        _afe.pdsg_mos_status ? "PDSG_ON" : "PDSG_OFF",
        _afe.pchg_mos_status ? "PCHG_ON" : "PCHG_OFF",
        _afe.dsg_mos_status  ? "DSG_ON"  : "DSG_OFF" ,
        _afe.chg_mos_status  ? "CHG_ON"  : "CHG_OFF");
    }
}