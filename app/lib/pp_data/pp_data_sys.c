#include "pp_data/pp_data_sys.h"
#include "pp_cfg.h"
#include "pp_data/pp_data_burn.h"
#include "rtthread.h"
#include "pp_export/pp_export_m.h"

pp_data_sys_t _sys = {
    .soft_version = PP_PRD_SOFTWARE_VERSION,
    .proto_version= PP_PRD_PROTO_VERSION,
    .log_swich = 1,
    .log_level = 6,
    .host_recv_data_size = PP_PCB_HOST_RECV_BUF_SIZE,
    .first_cycle_flag = 1,
};

static int _sys_data_init(void) {
    rt_snprintf(_sys.bat_id, sizeof(_sys.bat_id), PP_PRD_BAT_PP_NO"%010u", _burn.tmp_bat_id);
    rt_snprintf(_sys.hard_version, sizeof(_sys.hard_version), "%s", _burn.hard_version);
    return 0;
}
INIT_ENV_EXPORT(_sys_data_init);
