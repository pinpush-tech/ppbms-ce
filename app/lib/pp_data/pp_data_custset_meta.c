#include "pp_data/pp_data_custset_meta.h"
#include "pp_export/pp_export_m.h"

pp_data_custset_meta_t _custset_meta = {.meta = PP_PCB_CUSTSET_META};

static CsonModel host_custset_meta[] = {
    CSON_MODEL_OBJ(pp_data_custset_meta_t),
    CSON_MODEL_ARRAY(pp_data_custset_meta_t, meta, CSON_TYPE_UCHAR, sizeof(_custset_meta.meta))
};
PP_HOST_EXPORT_M(00, host_custset_meta, _custset_meta, pp_data_custset_meta_t);
