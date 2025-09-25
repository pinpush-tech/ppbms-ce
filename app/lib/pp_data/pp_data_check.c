#include "stdint.h"
#include "pp_afe/pp_afe_crt_calib.h"
#include "pp_data/pp_data_afe_cfg.h"

int32_t _data_check_afe_cfg(pp_data_afe_cfg_t *data) {
	if (_check_range_i(data->utc_th, -20, 5)) return 99;
	if (_check_range_i(data->utd_th, -30, 0)) return 98;
	if (_check_range_i(data->otc_th, 45, 70)) return 97;
	if (_check_range_i(data->otd_th, 50, 80)) return 96;
	if (_check_range_i(data->occ1_th, MV2MA_CHGTH(1), MV2MA_CHGTH(63))) return 95;
	if (_check_range_i(data->occ2_th, MV2MA_CHGTH(4), MV2MA_CHGTH(256))) return 94;
	if (_check_range_i(data->ocd1_th, MV2MA_DSGTH(1), MV2MA_DSGTH(63))) return 93;
	if (_check_range_i(data->ocd2_th, MV2MA_DSGTH(4), MV2MA_DSGTH(256))) return 92;
	if (_check_range_i(data->eoc_crrt, 0, 1000)) return 91;
	if (_check_range_i(data->eoc_th, 500, 4595)) return 90;
	if (_check_range_i(data->eocr_hys, 20, 1000)) return 89;
	if (_check_range_i(data->uv1_th, 0, 4095)) return 88;
	if (_check_range_i(data->uv2_th, 0, 4095)) return 87;
	if (_check_range_i(data->uvr_hys, 20, 1000)) return 86;
	if (_check_range_i(data->ov1_th, 500, 4595)) return 85;
	if (_check_range_i(data->ov2_th, 500, 4595)) return 84;
	if (_check_range_i(data->ovr_hys, 20, 1000)) return 83;
	if (_check_range_i(data->vdif_th, 500, 1000)) return 82;
	if (_check_range_i(data->vdif_hys, 20, 1000)) return 81;
	if (_check_range_i(data->cb1_th, 0, 4595)) return 80;
	if (_check_range_i(data->cb1_diff, 20, 1000)) return 79;
	if (_check_range_i(data->fmah, 2000, 100000)) return 78;
	if (_check_range_i(data->pdsg_time, 500, 4000)) return 77;
	if (_check_range_i(data->pdsg_scd_th, 0, 100)) return 76;
	if (_check_range_i(data->cell_count, 1, 24)) return 75;
	if (_check_range_i(data->scd_th, MV2MA_DSGTH(1), MV2MA_DSGTH(630))) return 74;
	if (_check_range_i(data->scd_dly, 0, 991)) return 73;
	if (_check_range_d(data->crt_calib_w, 0.5, 2)) return 72;
	if (_check_range_i(data->crt_calib_b, -1000, 1000)) return 71;
	return 0;
}