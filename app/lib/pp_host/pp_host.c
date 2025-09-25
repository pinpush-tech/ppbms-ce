#include <rtthread.h>
#include <string.h>
#include "pp_host/pp_host.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_uart/pp_uart.h"
#include "pp_persist/pp_persist.h"
#include "stdio.h"
#include "ulog.h"
#include "pp_tools/pp_tools.h"
#include "pp_data/pp_data_sys.h"
#include "pp_data/pp_data_afe.h"
#include "pp_env/pp_env.h"
#include "pp_bl/pp_bl_set_data.h"
#include "pp_export/pp_export_f.h"
#include "pp_assert/pp_assert.h"

void pp_host_log_send(pp_host_handle_t * handle, int64_t ts, uint8_t * buffer, uint8_t length) {
    PP_ASSERT(handle != RT_NULL);
    PP_ASSERT(buffer != RT_NULL);

    char c = '\n';
    char ts_str[19 + 3 + 1];
    snprintf(ts_str, 19 + 3 + 1, "%lld : ", ts);
    pp_host_data_send(handle, (uint8_t *)&ts_str, rt_strlen(ts_str));
    pp_host_data_send(handle, buffer, (size_t)length);
    pp_host_data_send(handle, (uint8_t *)&c, 1);
}

void pp_host_data_send(pp_host_handle_t * handle, uint8_t *data, rt_uint32_t len) {
    PP_ASSERT(handle != RT_NULL);
    PP_ASSERT(data != RT_NULL);

    pp_uart_handle_t * uart_handle = ((pp_host_handle_t *)handle)->uart_handle;
    PP_ASSERT(uart_handle != RT_NULL);

    rt_size_t w_len = rt_device_write(uart_handle->hrd._device, 0, data, len);
    PP_ASSERT(w_len == len);
}

rt_err_t pp_host_uart_irq_deal(pp_host_handle_t * handle, rt_size_t size) {
    PP_ASSERT(handle != RT_NULL);
    PP_ASSERT(handle->uart_handle != RT_NULL);
    PP_ASSERT(handle->recv_buffer != RT_NULL);
    PP_ASSERT(handle->sem != RT_NULL);

    pp_host_handle_t * host_handle = (pp_host_handle_t *)handle;
    for (int i = 0; i < size; i++) {
        if (0 == rt_device_read(host_handle->uart_handle->hrd._device, 0, host_handle->recv_buffer + host_handle->recv_tail, 1)) {
            return RT_ERROR;
        }
        if (host_handle->processing != 1) {
            host_handle->recv_tail++;
            rt_err_t ret = rt_sem_release(host_handle->sem);
            PP_ASSERT(ret == RT_EOK);
        }
    }
    return RT_EOK;
}

static uint16_t _operate(pp_host_handle_t * handle) {
    PP_ASSERT(handle != RT_NULL);
    PP_ASSERT(handle->recv_head != RT_NULL);
    PP_ASSERT(handle->recv_buffer != RT_NULL);

    if (handle->recv_head->crc != pp_tools_crc16(handle->recv_buffer + HEAD_CODE_CRC_LENGTH, HEAD_LENGTH - HEAD_CODE_CRC_LENGTH + handle->recv_head->data_length)) {
        return PP_HOST_ERR_CRC;
    }
    return (uint16_t) pp_export_f_hostopr(handle);
}

static void _route_this(pp_host_handle_t * handle) {
    PP_ASSERT(handle != RT_NULL);
    PP_ASSERT(handle->recv_head != RT_NULL);
    PP_ASSERT(handle->send_head != RT_NULL);
    PP_ASSERT(handle->send_buffer != RT_NULL);

    if (handle->recv_tail >= HEAD_LENGTH && handle->recv_head->code == 0xAEAE) {
        if (handle->recv_tail == HEAD_LENGTH + handle->recv_head->data_length) {
            handle->send_head->code = 0xEAEA;
            handle->send_head->opr = handle->recv_head->opr;
            handle->send_head->index = handle->recv_head->index;
            handle->send_head->pack_num = handle->recv_head->pack_num;
            handle->send_head->err_code = _operate(handle);
            if(handle->recv_head->opr == 000) {
                handle->send_head->crc = pp_tools_crc16(handle->send_buffer + HEAD_CODE_CRC_LENGTH, HEAD_LENGTH - HEAD_CODE_CRC_LENGTH);
                pp_host_data_send(handle, handle->send_buffer, HEAD_LENGTH);
                PP_ASSERT(handle->log_cb != RT_NULL);
                pp_iter_log(handle->log_cb, handle->send_head->pack_num);
            } else {
                handle->send_head->crc = pp_tools_crc16(handle->send_buffer + HEAD_CODE_CRC_LENGTH, HEAD_LENGTH - HEAD_CODE_CRC_LENGTH + handle->send_head->data_length);
                pp_host_data_send(handle, handle->send_buffer, HEAD_LENGTH + handle->send_head->data_length);
            }
        }
    }
}

static void host_thread_entry(void *parameter) {
    PP_ASSERT(parameter != RT_NULL);
    pp_host_handle_t * handle = (pp_host_handle_t *)parameter;
    PP_ASSERT(handle->sem != RT_NULL);

    while (1) {
        rt_err_t ret = rt_sem_control(handle->sem, RT_IPC_CMD_RESET, RT_NULL);
        PP_ASSERT(ret == RT_EOK);
        int32_t res = rt_sem_take(handle->sem, handle->time_out);
        if (res == RT_EOK) {
            if (handle->recv_tail > RECV_MAX_NUM) {
                handle->recv_tail = 0;
            }
        } else {
            if (handle->recv_tail != 0) {
                handle->processing = 1;
                _route_this(handle);
                // _route_that(handle);
                handle->processing = 0;
                handle->recv_tail = 0;
            }
        }
    }
}

pp_host_handle_t * pp_host_init(
     pp_uart_handle_t * uart_handle
    ,uint8_t time_out
    ,pp_host_log_cb_t log_cb
) {
    PP_ASSERT(uart_handle != RT_NULL);
    PP_ASSERT(log_cb != RT_NULL);

    pp_host_handle_t * handle = rt_malloc(sizeof(pp_host_handle_t));
    PP_ASSERT(handle != RT_NULL);

    rt_memset(handle, 0, sizeof(pp_host_handle_t));
    handle->uart_handle = uart_handle;
    handle->recv_head = (pp_host_head_t *) handle->recv_buffer;
    handle->send_head = (pp_host_head_t *) handle->send_buffer;
    handle->recv_data = handle->recv_buffer + HEAD_LENGTH;
    handle->send_data = handle->send_buffer + HEAD_LENGTH;
    handle->time_out = time_out;
    handle->log_cb = log_cb;

    handle->sem = rt_sem_create("host_rx_sem", 0, RT_IPC_FLAG_FIFO);
    PP_ASSERT(handle->sem != RT_NULL);

    rt_thread_t thread = rt_thread_create("host", host_thread_entry, handle, 2048, 20, 10);
    PP_ASSERT(thread != RT_NULL);
    rt_err_t ret = rt_thread_startup(thread);
    PP_ASSERT(ret == RT_EOK);

    return handle;
}
