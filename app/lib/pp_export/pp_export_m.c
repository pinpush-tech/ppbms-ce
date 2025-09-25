#include "pp_export/pp_export_m.h"
#include "pp_assert/pp_assert.h"
#include "rtthread.h"

static uint8_t _tmp_dt;
PP_EXPORT_M(_env_md_start,     RT_NULL, _tmp_dt, uint8_t, 01_a);
PP_EXPORT_M(_env_md_end,       RT_NULL, _tmp_dt, uint8_t, 01_c);
PP_EXPORT_M(_host_md_start,    RT_NULL, _tmp_dt, uint8_t, 02_a);
PP_EXPORT_M(_host_md_end,      RT_NULL, _tmp_dt, uint8_t, 02_c);
PP_EXPORT_M(_hostcmd_md_start, RT_NULL, _tmp_dt, uint8_t, 03_a);
PP_EXPORT_M(_hostcmd_md_end,   RT_NULL, _tmp_dt, uint8_t, 03_c);

pp_models_t _env_models;
pp_models_t _host_models;
pp_models_t _hostcmd_models;

static int _models_init(void) {
    _env_models.model_array = &__pp_md__env_md_start + 1;
    _env_models.num         = &__pp_md__env_md_end - &__pp_md__env_md_start - 1;
    PP_ASSERT(_env_models.num >= 0);
    PP_ASSERT(_env_models.model_array != RT_NULL);

    _host_models.model_array = &__pp_md__host_md_start + 1;
    _host_models.num         = &__pp_md__host_md_end - &__pp_md__host_md_start - 1;
    PP_ASSERT(_host_models.num >= 0);
    PP_ASSERT(_host_models.model_array != RT_NULL);

    _hostcmd_models.model_array = &__pp_md__hostcmd_md_start + 1;
    _hostcmd_models.num         = &__pp_md__hostcmd_md_end - &__pp_md__hostcmd_md_start - 1;
    PP_ASSERT(_hostcmd_models.num >= 0);
    PP_ASSERT(_hostcmd_models.model_array != RT_NULL);

    return 0;
}
INIT_BOARD_EXPORT(_models_init);