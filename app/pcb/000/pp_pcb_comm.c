#include "pp_uart/pp_uart.h"
#include "pp_uart/pp_uart_can.h"
#include "pp_console/pp_console.h"
#include "pp_data/pp_data_sys.h"
#include "pp_afe/pp_afe.h"
#include "pp_host/pp_host.h"
#include "pp_data/pp_data_afe.h"
#include "ulog.h"
#include "pp_pin/pp_pin.h"
#include "pp_export/pp_export_f.h"
#include <string.h>
#include <stdio.h>
#include "pp_assert/pp_assert.h"

static pp_host_handle_t *_uart_host_handle;
static pp_uart_handle_t _uart_handle_array[2]; // 0 console; 1 uart
static pp_uart_handles_t _uart_handles = {
    .uart_handle_array = _uart_handle_array,
    .num = sizeof(_uart_handle_array) / sizeof(pp_uart_handle_t),
};

static rt_err_t _uart_irq_cb(rt_device_t dev, rt_size_t size) {
    PP_ASSERT(dev != RT_NULL);
    if (_sys.stop_status == 0)
        _sys.stop_reset_flag = 1;
    else
        _sys.wkup_flag = 1;
    return pp_host_uart_irq_deal(_uart_host_handle, size);
}
static void _uart_log_cb(int64_t ts, uint8_t * buffer, uint8_t length) {
    PP_ASSERT(buffer != RT_NULL);
    pp_host_log_send(_uart_host_handle, ts, buffer, length);
}
static int32_t _comm_init(void * args) {
    _uart_handle_array[0].finsh = 1;
    pp_uart_handle_switch_back();
    return 0;
}
PP_APP1_EXPORT_F(_comm_init);

static int32_t _console_init(void * args) {
    pp_uart_init(&_uart_handles);

    _uart_handle_array[0].hrd.uart_name = "usart2";
    _uart_handle_array[0].finsh = 0;
    pp_console_init(&_uart_handle_array[0]);

    _uart_handle_array[1].hrd.uart_name = "usart1";
    _uart_handle_array[1].irq_cb = _uart_irq_cb;
    _uart_handle_array[1].baud = 19200;
    _uart_handle_array[1].hrd.hrd_baud = 19200;
    _uart_host_handle = pp_host_init(
        &_uart_handle_array[1], 50,
        _uart_log_cb);
    PP_ASSERT(_uart_host_handle != RT_NULL);
    pp_uart_handle_init(&_uart_handle_array[1]);

    pp_uart_handle_switch(1, 0);

    return 0;
}
PP_PREV0_EXPORT_F(_console_init);
