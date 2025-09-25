#include "pp_export/pp_export_m.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_afe_cfg.h"
#include "pp_data/pp_data_bl.h"
#include "pp_data/pp_data_sox.h"
#include "pp_data/pp_data_sys.h"

static CsonModel env_sys_sh[] = {
	CSON_MODEL_OBJ(pp_data_sys_t),
	CSON_MODEL_UCHAR(pp_data_sys_t, log_swich),
	CSON_MODEL_UCHAR(pp_data_sys_t, log_level),
	CSON_MODEL_UCHAR(pp_data_sys_t, mos_lock_chg),
	CSON_MODEL_UCHAR(pp_data_sys_t, mos_lock_dsg),
};
PP_ENV_EXPORT_M(01, env_sys_sh, _sys, pp_data_sys_t);

static CsonModel env_sox_s[] = {
	CSON_MODEL_OBJ(pp_data_sox_t),
	CSON_MODEL_UCHAR(pp_data_sox_t, soh),
	CSON_MODEL_UCHAR(pp_data_sox_t, fmah_lrn_flag),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_100_0_cf_flag),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_100_0_dsg_flag),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_x_100_x),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_x_100_type),
	CSON_MODEL_FLOAT(pp_data_sox_t, ff_x_100_dsg_mah),
	CSON_MODEL_FLOAT(pp_data_sox_t, ff_x_100_chg_mah),
	CSON_MODEL_FLOAT(pp_data_sox_t, comp_mah),
	CSON_MODEL_FLOAT(pp_data_sox_t, fmah),
	CSON_MODEL_UCHAR(pp_data_sox_t, rmah_pct),
	CSON_MODEL_INT(pp_data_sox_t, rmah),
	CSON_MODEL_INT(pp_data_sox_t, cyc_dsg_mah),
	CSON_MODEL_USHORT(pp_data_sox_t, cyc_cnt),
};
PP_ENV_EXPORT_M(02, env_sox_s, _sox, pp_data_sox_t);

static CsonModel env_afe_cfg_sh[] = {
	CSON_MODEL_OBJ(pp_data_afe_cfg_t),
	CSON_MODEL_CHAR(pp_data_afe_cfg_t, utc_th),
	CSON_MODEL_CHAR(pp_data_afe_cfg_t, utd_th),
	CSON_MODEL_CHAR(pp_data_afe_cfg_t, otc_th),
	CSON_MODEL_CHAR(pp_data_afe_cfg_t, otd_th),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, occ1_th),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, occ2_th),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, ocd1_th),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, ocd2_th),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, eoc_crrt),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, eoc_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, eocr_hys),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, uv1_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, uv2_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, uvr_hys),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, ov1_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, ov2_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, ovr_hys),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, vdif_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, vdif_hys),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, cb1_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, cb1_diff),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, fmah),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, pdsg_time),
	CSON_MODEL_UCHAR(pp_data_afe_cfg_t, pdsg_scd_th),
	CSON_MODEL_UCHAR(pp_data_afe_cfg_t, smart_mos_off_flag),
	CSON_MODEL_UCHAR(pp_data_afe_cfg_t, shutdown_off_flag),
	CSON_MODEL_UCHAR(pp_data_afe_cfg_t, cell_count),
	CSON_MODEL_UINT(pp_data_afe_cfg_t, scd_th),
	CSON_MODEL_USHORT(pp_data_afe_cfg_t, scd_dly),
	CSON_MODEL_DOUBLE(pp_data_afe_cfg_t, crt_calib_w),
	CSON_MODEL_INT(pp_data_afe_cfg_t, crt_calib_b),
};
PP_ENV_EXPORT_M(03, env_afe_cfg_sh, _afe_cfg, pp_data_afe_cfg_t);

static CsonModel env_afe_sh[] = {
	CSON_MODEL_OBJ(pp_data_afe_t),
	CSON_MODEL_UCHAR(pp_data_afe_t, dsgm_fail_status),
	CSON_MODEL_UCHAR(pp_data_afe_t, chgm_fail_status),
	CSON_MODEL_UCHAR(pp_data_afe_t, afe_fail_status),
	CSON_MODEL_UCHAR(pp_data_afe_t, fuse_status),
	CSON_MODEL_UCHAR(pp_data_afe_t, pdsg_scd_status),
	CSON_MODEL_UCHAR(pp_data_afe_t, scd_status),
};
PP_ENV_EXPORT_M(04, env_afe_sh, _afe, pp_data_afe_t);

static CsonModel env_sys_s[] = {
	CSON_MODEL_OBJ(pp_data_sys_t),
	CSON_MODEL_ULONG(pp_data_sys_t, timestamp),
	CSON_MODEL_UCHAR(pp_data_sys_t, stop_status),
	CSON_MODEL_UCHAR(pp_data_sys_t, running_status),
};
PP_ENV_EXPORT_M(05, env_sys_s, _sys, pp_data_sys_t);

static CsonModel host_bl[] = {
	CSON_MODEL_OBJ(pp_data_bl_t),
	CSON_MODEL_UCHAR(pp_data_bl_t, bl_state),
};
PP_HOST_EXPORT_M(01, host_bl, _bl, pp_data_bl_t);

static CsonModel host_sys[] = {
	CSON_MODEL_OBJ(pp_data_sys_t),
	CSON_MODEL_ARRAY(pp_data_sys_t, soft_version, CSON_TYPE_UCHAR, sizeof(_sys.soft_version)),
	CSON_MODEL_ARRAY(pp_data_sys_t, hard_version, CSON_TYPE_UCHAR, sizeof(_sys.hard_version)),
	CSON_MODEL_ARRAY(pp_data_sys_t, proto_version, CSON_TYPE_UCHAR, sizeof(_sys.proto_version)),
	CSON_MODEL_ARRAY(pp_data_sys_t, bat_id, CSON_TYPE_UCHAR, sizeof(_sys.bat_id)),
	CSON_MODEL_USHORT(pp_data_sys_t, host_recv_data_size),
};
PP_HOST_EXPORT_M(02, host_sys, _sys, pp_data_sys_t);

static CsonModel host_sox[] = {
	CSON_MODEL_OBJ(pp_data_sox_t),
	CSON_MODEL_UCHAR(pp_data_sox_t, soh),
	CSON_MODEL_UCHAR(pp_data_sox_t, fmah_lrn_flag),
	CSON_MODEL_FLOAT(pp_data_sox_t, fmah),
	CSON_MODEL_UCHAR(pp_data_sox_t, rmah_pct),
	CSON_MODEL_USHORT(pp_data_sox_t, cyc_cnt),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_100_0_cf_flag),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_100_0_dsg_flag),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_x_100_x),
	CSON_MODEL_UCHAR(pp_data_sox_t, ff_x_100_type),
	CSON_MODEL_FLOAT(pp_data_sox_t, ff_x_100_dsg_mah),
	CSON_MODEL_FLOAT(pp_data_sox_t, ff_x_100_chg_mah),
	CSON_MODEL_FLOAT(pp_data_sox_t, comp_mah),
	CSON_MODEL_INT(pp_data_sox_t, mah_t1),
	CSON_MODEL_INT(pp_data_sox_t, mah_t2),
	CSON_MODEL_INT(pp_data_sox_t, rmah),
	CSON_MODEL_INT(pp_data_sox_t, cyc_dsg_mah),
};
PP_HOST_EXPORT_M(03, host_sox, _sox, pp_data_sox_t);

static CsonModel host_afe_vol[] = {
	CSON_MODEL_OBJ(pp_data_afe_t),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_max_vol),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_min_vol),
	CSON_MODEL_UCHAR(pp_data_afe_t, maxv_cell_ch),
	CSON_MODEL_UCHAR(pp_data_afe_t, minv_cell_ch),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_1),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_2),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_3),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_4),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_5),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_6),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_7),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_8),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_9),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_10),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_11),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_12),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_13),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_14),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_15),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_16),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_17),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_18),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_19),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_20),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_21),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_22),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_23),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_vol_24),
};
PP_HOST_EXPORT_M(04, host_afe_vol, _afe, pp_data_afe_t);

static CsonModel host_afe_other[] = {
	CSON_MODEL_OBJ(pp_data_afe_t),
	CSON_MODEL_UINT(pp_data_afe_t, bat_vol),
	CSON_MODEL_INT(pp_data_afe_t, crt),
	CSON_MODEL_INT(pp_data_afe_t, crt_original),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_min_temp),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_max_temp),
	CSON_MODEL_SHORT(pp_data_afe_t, cell_avg_temp),
	CSON_MODEL_SHORT(pp_data_afe_t, mos_temp),
	CSON_MODEL_SHORT(pp_data_afe_t, chip_temp),
	CSON_MODEL_UCHAR(pp_data_afe_t, bat_status),
};
PP_HOST_EXPORT_M(05, host_afe_other, _afe, pp_data_afe_t);

static CsonModel host_afe_status[] = {
	CSON_MODEL_OBJ(pp_data_afe_t),
	CSON_MODEL_ARRAY(pp_data_afe_t, afe_status, CSON_TYPE_UCHAR, sizeof(_afe.afe_status)),
};
PP_HOST_EXPORT_M(06, host_afe_status, _afe, pp_data_afe_t);

static CsonModel host_afe_cfg[] = {
	CSON_MODEL_OBJ(pp_data_afe_cfg_t),
	CSON_MODEL_DOUBLE(pp_data_afe_cfg_t, crt_rv),
};
PP_HOST_EXPORT_M(07, host_afe_cfg, _afe_cfg, pp_data_afe_cfg_t);
