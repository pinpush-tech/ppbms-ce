#include "pp_serialize/pp_serialize.h"
#include "stdint.h"
#include "cson.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_sox.h"
#include "pp_data/pp_data_sys.h"
#include "pp_data/pp_data_custset.h"
#include "pp_data/pp_data_hostcmd.h"
#include "pp_data/pp_data_bl.h"
#include "pp_data/pp_data_custset_meta.h"
#include "pp_host/pp_host.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_env/pp_env.h"
#include "pp_export/pp_export_f.h"
#include "pp_export/pp_export_m.h"
#include "pp_assert/pp_assert.h"

static int32_t _model_hostopr(void * args) {
    PP_ASSERT(args != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *) args;
    PP_ASSERT(handle->recv_head != RT_NULL);
    PP_ASSERT(handle->send_head != RT_NULL);
    PP_ASSERT(handle->recv_data != RT_NULL);
    PP_ASSERT(handle->send_data != RT_NULL);

    if(handle->recv_head->opr == 010 || handle->recv_head->opr == 011 ||
       handle->recv_head->opr == 012 || handle->recv_head->opr == 013 ||
       handle->recv_head->opr == 014 || handle->recv_head->opr == 015) {
        uint8_t write = handle->recv_head->opr % 2;
        pp_models_t * models = handle->recv_head->opr == 010 || handle->recv_head->opr == 011 ? &_hostcmd_models :
                               handle->recv_head->opr == 012 || handle->recv_head->opr == 013 ? &_host_models     : &_env_models;
        if (handle->recv_head->index >= models->num) return PP_HOST_ERR_INDEX;
        handle->send_head->data_length = pp_serialize_cal_length(&models->model_array[handle->recv_head->index]);
        if (write == 0 && HEAD_LENGTH + handle->send_head->data_length > SEND_MAX_NUM) return PP_HOST_ERR_LENGTH;
        if (write == 1 && handle->send_head->data_length != handle->recv_head->data_length) return PP_HOST_ERR_LENGTH;
        if (write == 1) {
            pp_model_t pre_dtm = models->model_array[handle->recv_head->index];
            void * pre_data = rt_malloc(pre_dtm.struct_size);
            PP_ASSERT(pre_data != RT_NULL);
            pre_dtm.struct_address = pre_data;
            pp_deserialize(handle->recv_data, &pre_dtm);
            handle->pre_data = pre_data;
        }
        int32_t res = pp_export_f_hostpre(handle);
        if (write == 1) {
            rt_free(handle->pre_data);
            handle->pre_data = RT_NULL;
        }
        if (res != 0) return res;
        if (write == 1) pp_deserialize(handle->recv_data, &models->model_array[handle->recv_head->index]);
        pp_serialize(&models->model_array[handle->recv_head->index], handle->send_data);
        res = pp_export_f_hostpost(handle);
        if (res != 0) return res;
        return 0;
    }
    return PP_HOST_ERR_OPR;
}
PP_HOSTOPR_EXPORT_F(_model_hostopr);