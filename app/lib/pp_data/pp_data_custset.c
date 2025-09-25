#include "pp_data/pp_data_custset.h"
#include "pp_cfg.h"
#include "pp_export/pp_export_m.h"

pp_data_custset_t _custset = {
    .bootcomm = PP_PRD_CUSTSET_BOOTCOMM,
#ifdef PP_PRD_CUSTSET_ITEM0
    .setitem0 = PP_PRD_CUSTSET_ITEM0,
#endif
#ifdef PP_PRD_CUSTSET_ITEM1
    .setitem1 = PP_PRD_CUSTSET_ITEM1,
#endif
};

static CsonModel env_custset_sh[] = {
    CSON_MODEL_OBJ(pp_data_custset_t),
    CSON_MODEL_UCHAR(pp_data_custset_t, bootcomm),
    CSON_MODEL_UCHAR(pp_data_custset_t, setitem0),
    CSON_MODEL_UCHAR(pp_data_custset_t, setitem1),
};
PP_ENV_EXPORT_M(00, env_custset_sh, _custset, pp_data_custset_t);
