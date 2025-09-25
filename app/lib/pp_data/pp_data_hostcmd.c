#include "pp_data/pp_data_hostcmd.h"
#include "pp_export/pp_export_m.h"

pp_data_hostcmd_t _hostcmd = {0};

static CsonModel hostcmd_cmd[] = {
    CSON_MODEL_OBJ(pp_data_hostcmd_t),
    CSON_MODEL_UCHAR(pp_data_hostcmd_t, index),
    CSON_MODEL_UCHAR(pp_data_hostcmd_t, arg_num),
    CSON_MODEL_LONG(pp_data_hostcmd_t, arg1),
    CSON_MODEL_LONG(pp_data_hostcmd_t, arg2),
};
PP_HOSTCMD_EXPORT_M(00, hostcmd_cmd, _hostcmd, pp_data_hostcmd_t);