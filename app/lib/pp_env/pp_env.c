#include "pp_persist/pp_persist.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_sox.h"
#include "pp_data/pp_data_sys.h"
#include "pp_host/pp_host.h"
#include "rtthread.h"
#include "pp_env/pp_env.h"
#include "pp_data/pp_data_custset.h"
#include "pp_mcu/pp_mcu_wdg.h"
#include "pp_export/pp_export_f.h"
#include "pp_export/pp_export_m.h"
#include "pp_assert/pp_assert.h"

void pp_env_store(void) {
    char name[3];
    for (int i = 0; i < _env_models.num; i++) {
        rt_snprintf(name, sizeof(name), "%02d", i);
        pp_store_env(&_env_models.model_array[i], name);
    }
}
void pp_env_del(int8_t type) {
    char name[3];
    for (int i = 0; i < _env_models.num; i++) {
        if (type == -1 || type == i) {
            rt_snprintf(name, sizeof(name), "%02d", i);
            pp_del_env(name);
        }
    }
}

static int32_t _env_read(void) {
    char name[3];
    for (int i = 0; i < _env_models.num; i++) {
        rt_snprintf(name, sizeof(name), "%02d", i);
        pp_read_env(&_env_models.model_array[i], name);
    }
    return 0;
}
INIT_DEVICE_EXPORT(_env_read);

static int32_t _env_hostpost(void * args) {
    PP_ASSERT(args != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *) args;
    PP_ASSERT(handle->recv_head != RT_NULL);

    if (handle->recv_head->opr == 015) {
        _sys.store_flag = 1;
    }
    return 0;
}
PP_HOSTPOST_EXPORT_F(_env_hostpost);