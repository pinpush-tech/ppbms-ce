#pragma once

#include <stdint.h>
#include "pp_uart/pp_uart.h"
#include "rtthread.h"
#include "pp_cfg.h"

#define    HEAD_LENGTH           14
#define    HEAD_CODE_CRC_LENGTH   4
#define    SEND_MAX_NUM         256
#define    RECV_MAX_NUM         PP_PCB_HOST_RECV_BUF_SIZE + HEAD_LENGTH

typedef void (*pp_host_data_cb_t)(pp_uart_handle_t * uart_handle, uint8_t *data, rt_uint32_t len);
typedef void (*pp_host_log_cb_t)(int64_t ts, uint8_t * buffer, uint8_t length);

typedef struct {
    uint16_t    code;
    uint16_t    crc;
    uint16_t    opr;
    uint16_t    index;
    uint16_t    data_length;
    uint16_t    pack_num;
    uint16_t    err_code;
} pp_host_head_t;

typedef struct {
    pp_uart_handle_t *  uart_handle;
    rt_sem_t            sem;
    uint8_t             recv_buffer[RECV_MAX_NUM + 128];
    uint8_t             send_buffer[SEND_MAX_NUM];
    pp_host_head_t *    recv_head;
    pp_host_head_t *    send_head;
    uint8_t *           recv_data;
    uint8_t *           send_data;
    uint16_t            recv_tail;
    uint8_t             time_out;
    pp_host_log_cb_t    log_cb;
    uint8_t             processing;
    void *              pre_data;
}pp_host_handle_t;

pp_host_handle_t * pp_host_init(
     pp_uart_handle_t * uart_handle
    ,uint8_t time_out
	,pp_host_log_cb_t log_cb
);

rt_err_t pp_host_uart_irq_deal(pp_host_handle_t * handle, rt_size_t size);
void pp_host_log_send(pp_host_handle_t * handle, int64_t ts, uint8_t * buffer, uint8_t length);
void pp_host_data_send(pp_host_handle_t * handle, uint8_t *data, rt_uint32_t len);
