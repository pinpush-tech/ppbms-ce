#include "pp_host.h"
#include "pp_export/pp_export_f.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_assert/pp_assert.h"

static int32_t _hostcmd_hostpost(void * args) {
    PP_ASSERT(args != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *) args;
    PP_ASSERT(handle->recv_head != RT_NULL);

    if (handle->recv_head->opr == 011) {
        int32_t res = pp_export_f_hostcmd(RT_NULL);
        if (res == PP_HOST_ERR_CMD_INDEX)
            return PP_HOST_ERR_CMD_ARGS;
        return res;
    }
    return 0;
}
PP_HOSTPOST_EXPORT_F(_hostcmd_hostpost);