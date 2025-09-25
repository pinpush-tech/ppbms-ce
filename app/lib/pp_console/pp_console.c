#include "rtthread.h"
#include "rtdevice.h"
#include "pp_assert/pp_assert.h"
#include "pp_uart/pp_uart.h"

static rt_device_t _console_uart_init(char *uart_name, int32_t baud, uint8_t finsh) {
    PP_ASSERT(uart_name != RT_NULL);
    PP_ASSERT(finsh == 0 || finsh == 1);

    rt_console_set_device(uart_name); // console初始化
#ifdef RT_USING_FINSH
    if (finsh == 1) {
        finsh_set_device(uart_name); // shell初始化
    }
#endif
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = baud;
    rt_device_t  uart_device = rt_device_find(uart_name);
    PP_ASSERT(uart_device != RT_NULL);

    rt_err_t ret = rt_device_control(uart_device, RT_DEVICE_CTRL_CONFIG, &config);
    PP_ASSERT(ret == RT_EOK);
#ifdef RT_USING_FINSH
    if (finsh == 0) {
#endif
        ret = rt_device_set_rx_indicate(uart_device, RT_NULL);
        PP_ASSERT(ret == RT_EOK);
#ifdef RT_USING_FINSH
    }
#endif
    return uart_device;
}

void pp_console_init(pp_uart_handle_t * uart_handle) {
    PP_ASSERT(uart_handle != RT_NULL);
    uart_handle->init_cb = _console_uart_init;
    uart_handle->baud = 115200;
    pp_uart_handle_init(uart_handle);
}