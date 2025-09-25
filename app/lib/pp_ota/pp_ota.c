#include "stdint.h"
#include "pp_host/pp_host.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_data/pp_data_sys.h"
#include "pp_bl/pp_bl_set_data.h"
#include "pp_bl/pp_bl_data.h"
#include "pp_export/pp_export_f.h"
#include "pp_persist/pp_persist.h"
#include "pp_assert/pp_assert.h"

static int32_t _ota_hostopr(void * args) {
    PP_ASSERT(args != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *) args;
    PP_ASSERT(handle->recv_head != RT_NULL);
    PP_ASSERT(handle->send_head != RT_NULL);
    PP_ASSERT(handle->recv_data != RT_NULL);

    if (handle->recv_head->opr == 001) {
        handle->send_head->data_length = 0x0000;
        static uint16_t expect_pack_id = 0;
        static uint32_t download_size = 0;
        if(_sys.ota_done == 1) {
            return PP_HOST_ERR_HAVE_BEEN_OTA;
        }
        if(handle->recv_head->index == 0) {
            download_size = 0;
            expect_pack_id = 0;
        } else if (handle->recv_head->index == expect_pack_id - 1) {
            return 0;
        }
        if (expect_pack_id != handle->recv_head->index) {
            return PP_HOST_ERR_INDEX;
        }
        pp_write_dl_data(download_size, handle->recv_data, handle->recv_head->data_length);
        download_size += handle->recv_head->data_length;
        expect_pack_id ++;
        if(expect_pack_id == handle->recv_head->pack_num) {
            pp_bl_data.sota_flag = 1;
            pp_bl_data.app_size = download_size;
            pp_bl_set_dl_offset();
            pp_write_bl_data();
            download_size = 0;
            expect_pack_id = 0;
            _sys.ota_done = 1;
        }
        return 0;
    }
    return PP_HOST_ERR_OPR;
}
PP_HOSTOPR_EXPORT_F(_ota_hostopr);