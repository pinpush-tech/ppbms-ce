#pragma once

#include "stdint.h"
#include "rtthread.h"

typedef struct _uart_handle_t pp_uart_handle_t;
typedef struct _uart_handle_hrd_t pp_uart_handle_hrd_t;

typedef rt_err_t (*pp_uart_irq_cb_t)(rt_device_t dev, rt_size_t size);
typedef rt_device_t (*pp_uart_init_cb_t)(char * uart_name,int32_t baud, uint8_t finsh);
typedef rt_err_t (*pp_uart_hrd_init_cb_t)(void);
typedef rt_err_t (*pp_uart_rs485_tx_mode_cb_t)(rt_device_t dev);
typedef rt_err_t (*pp_uart_rs485_rx_mode_cb_t)(rt_device_t dev);

struct _uart_handle_hrd_t {
    char * uart_name;
    int32_t hrd_baud;
    uint8_t is_single;
    pp_uart_hrd_init_cb_t init_cb;
    pp_uart_rs485_tx_mode_cb_t rs485_tx_mode_cb;
    pp_uart_rs485_rx_mode_cb_t rs485_rx_mode_cb;
    // pp_uart负责初始化
    rt_device_t _device;
};

struct _uart_handle_t {
    int32_t baud;
    pp_uart_irq_cb_t irq_cb;
    pp_uart_init_cb_t init_cb;
    uint8_t finsh;
    pp_uart_handle_hrd_t hrd;
    pp_uart_irq_cb_t _irq_cb;
};

// 顺序代表初始化的优先级
typedef struct {
	pp_uart_handle_t * uart_handle_array;
	uint8_t num;
} pp_uart_handles_t;

void pp_uart_init(pp_uart_handles_t * uart_handles);
void pp_uart_handle_init(pp_uart_handle_t * uart_handle);
void pp_uart_handle_deinit(pp_uart_handle_t * uart_handle);
int32_t pp_uart_handle_switch(uint8_t from, uint8_t to);
int32_t pp_uart_handle_switch_back(void);
