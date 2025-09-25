#include "stdint.h"
#include "pp_assert/pp_assert.h"
#include "pp_data/pp_data_afe_cfg.h"

int32_t MV2MA(double mv) {
    PP_ASSERT(_afe_cfg.crt_rv > 0);
    PP_ASSERT(_afe_cfg.crt_calib_w != 0);
    return (mv / _afe_cfg.crt_rv - _afe_cfg.crt_calib_b) / _afe_cfg.crt_calib_w;
}

uint32_t MV2MA_DSGTH(double mv) {
    PP_ASSERT(_afe_cfg.crt_rv > 0);
    PP_ASSERT(_afe_cfg.crt_calib_w != 0);
    return (mv / _afe_cfg.crt_rv + _afe_cfg.crt_calib_b) / _afe_cfg.crt_calib_w;
}
uint32_t MV2MA_CHGTH(double mv) {
    PP_ASSERT(_afe_cfg.crt_rv > 0);
    PP_ASSERT(_afe_cfg.crt_calib_w != 0);
    return ( mv / _afe_cfg.crt_rv - _afe_cfg.crt_calib_b) / _afe_cfg.crt_calib_w;
}

double MA2MV_DSGTH(uint32_t ma) {
    PP_ASSERT(_afe_cfg.crt_rv > 0);
    return (ma * _afe_cfg.crt_calib_w - _afe_cfg.crt_calib_b) * _afe_cfg.crt_rv;
}
double MA2MV_CHGTH(uint32_t ma) {
    PP_ASSERT(_afe_cfg.crt_rv > 0);
    return (ma * _afe_cfg.crt_calib_w + _afe_cfg.crt_calib_b) * _afe_cfg.crt_rv;
}
