#include "pp_host/pp_host.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_export/pp_export_f.h"
#include "pp_assert/pp_assert.h"

static int32_t _ping_hostopr(void * args) {
    PP_ASSERT(args != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *) args;
    PP_ASSERT(handle->recv_head != RT_NULL);
    PP_ASSERT(handle->send_head != RT_NULL);

    if (handle->recv_head->opr == 000) {
        handle->send_head->data_length = 0x0000;
        return 0;
    }
    return PP_HOST_ERR_OPR;
}
PP_HOSTOPR_EXPORT_F(_ping_hostopr);