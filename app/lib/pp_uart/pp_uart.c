#include "pp_cfg.h"
#include "stdint.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "pp_uart/pp_uart.h"
#include "ulog.h"
#include "pp_mcu/pp_mcu.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_data/pp_data_hostcmd.h"
#include "pp_export/pp_export_f.h"
#include "pp_assert/pp_assert.h"

#define ARGS_ERR 11
#define SWITCHED_ERR 12
#define NO_SWITCHED_ERR 13

static pp_uart_handles_t * _uart_handles;
static uint8_t _from_id = 0xFF;
static uint8_t _to_id = 0xFF;

/*获取id对应的串口句柄*/
static pp_uart_handle_t * get_uart_handle_by_id(uint8_t id) {
    if(id >= _uart_handles->num) {
        return RT_NULL;
    }
    return &_uart_handles->uart_handle_array[id];
}

static pp_uart_handle_t * get_uart_handle_by_dev(rt_device_t dev) {
    PP_ASSERT(dev != RT_NULL);
    for (uint8_t i = 0; i < _uart_handles->num; i++) {
        if (_uart_handles->uart_handle_array[i].hrd._device == dev) {
            return &_uart_handles->uart_handle_array[i];
        }
    }
    PP_ASSERT(0);
    return RT_NULL;
}

static rt_err_t _single_tx_irq_cb(rt_device_t dev, rt_size_t size) {
    PP_ASSERT(dev != RT_NULL);
	uint8_t buffer = 0;
	rt_size_t read_len = rt_device_read(dev, 0, &buffer, 1);
	PP_ASSERT(read_len == 1);
	return 0;
}

static rt_err_t _single_tx_mode_cb(rt_device_t dev) {
    PP_ASSERT(dev != RT_NULL);
    pp_uart_handle_t * uart_handle = get_uart_handle_by_dev(dev);
    if (dev->rx_indicate == _single_tx_irq_cb) return 0;
    uart_handle->_irq_cb = dev->rx_indicate;
    rt_err_t ret = rt_device_set_rx_indicate(dev, _single_tx_irq_cb);
    PP_ASSERT(ret == RT_EOK);
    return 0;
}

static int32_t _get_real_baud(pp_uart_handle_t * uart_handle) {
    PP_ASSERT(uart_handle != RT_NULL);
    return uart_handle->hrd.hrd_baud != 0 ? uart_handle->hrd.hrd_baud : uart_handle->baud;
}

static rt_err_t _single_rx_mode_cb(rt_device_t dev) {
    PP_ASSERT(dev != RT_NULL);
    pp_uart_handle_t * uart_handle = get_uart_handle_by_dev(dev);
    rt_err_t ret = rt_thread_mdelay(PP_PCB_DELAY_WRITE_COMPLETE);
    PP_ASSERT(ret == RT_EOK);
	ret = rt_device_set_rx_indicate(dev, uart_handle->_irq_cb);
	PP_ASSERT(ret == RT_EOK);
    return 0;
}

void pp_uart_handle_init(
    pp_uart_handle_t * uart_handle
) {
    PP_ASSERT(uart_handle != RT_NULL);
    int32_t baud = _get_real_baud(uart_handle);
    if (uart_handle->irq_cb != RT_NULL) {
        struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
        config.baud_rate = baud;
        uart_handle->hrd._device = rt_device_find(uart_handle->hrd.uart_name);
        PP_ASSERT(uart_handle->hrd._device != RT_NULL);
        rt_err_t ret = rt_device_control(uart_handle->hrd._device, RT_DEVICE_CTRL_CONFIG, &config);
        PP_ASSERT(ret == RT_EOK);
        ret = rt_device_set_rx_indicate(uart_handle->hrd._device, uart_handle->irq_cb);
        PP_ASSERT(ret == RT_EOK);
        ret = rt_device_open(uart_handle->hrd._device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
        PP_ASSERT(ret == RT_EOK);
    } else {
        PP_ASSERT(uart_handle->init_cb != RT_NULL);
        uart_handle->hrd._device = uart_handle->init_cb(uart_handle->hrd.uart_name, baud, uart_handle->finsh);
        PP_ASSERT(uart_handle->hrd._device != RT_NULL);
    }

    if (uart_handle->hrd.is_single == 1) {
        rt_err_t ret = rt_device_set_tx_starting_pp(uart_handle->hrd._device, _single_tx_mode_cb);
        PP_ASSERT(ret == RT_EOK);
        ret = rt_device_set_tx_complete_pp(uart_handle->hrd._device, _single_rx_mode_cb);
        PP_ASSERT(ret == RT_EOK);
    }

    if (uart_handle->hrd.rs485_tx_mode_cb != RT_NULL) {
        rt_err_t ret = rt_device_set_tx_starting_pp(uart_handle->hrd._device, uart_handle->hrd.rs485_tx_mode_cb);
        PP_ASSERT(ret == RT_EOK);
    }
    if (uart_handle->hrd.rs485_rx_mode_cb != RT_NULL) {
        rt_err_t ret = rt_device_set_tx_complete_pp(uart_handle->hrd._device, uart_handle->hrd.rs485_rx_mode_cb);
        PP_ASSERT(ret == RT_EOK);
        uart_handle->hrd.rs485_rx_mode_cb(uart_handle->hrd._device);
    }
}

void pp_uart_handle_deinit(pp_uart_handle_t * uart_handle) {
    PP_ASSERT(uart_handle != RT_NULL);
    PP_ASSERT(uart_handle->hrd._device != RT_NULL);
    while (uart_handle->hrd._device->ref_count != 0) {
        rt_err_t ret = rt_device_close(uart_handle->hrd._device);
        PP_ASSERT(ret == RT_EOK);
    }
}

void _hrd_switch(pp_uart_handle_t * uart_handle1, pp_uart_handle_t * uart_handle2) {
    PP_ASSERT(uart_handle1 != RT_NULL);
    PP_ASSERT(uart_handle2 != RT_NULL);
    pp_uart_handle_hrd_t hrd;
    hrd = uart_handle1->hrd;
    uart_handle1->hrd = uart_handle2->hrd;
    uart_handle2->hrd = hrd;
}

int32_t pp_uart_handle_switch_back(void) {
    if (_from_id == 0xFF) {
        LOG_E("NO uart switch_back");
        return NO_SWITCHED_ERR;
    }

    uint8_t from = _from_id;
    uint8_t to = _to_id;
    _from_id = 0xFF;
    _to_id = 0xFF;

    pp_uart_handle_t * from_handle = get_uart_handle_by_id(from);
    pp_uart_handle_t * to_handle = get_uart_handle_by_id(to);
    PP_ASSERT(from_handle != RT_NULL);
    PP_ASSERT(to_handle != RT_NULL);

    pp_uart_handle_deinit(from_handle);
    pp_uart_handle_deinit(to_handle);

    _hrd_switch(from_handle, to_handle);

    if (from < to) {
        pp_uart_handle_init(from_handle);
        pp_uart_handle_init(to_handle);
    } else {
        pp_uart_handle_init(to_handle);
        pp_uart_handle_init(from_handle);
    }

    return 0;
}

int32_t pp_uart_handle_switch(uint8_t from, uint8_t to) {
    if (_from_id != 0xFF) {
        LOG_E("A serial port is being switched");
        return SWITCHED_ERR;
    }
    if (from == to) {
        return ARGS_ERR;
    }
    pp_uart_handle_t * from_handle = get_uart_handle_by_id(from);
    pp_uart_handle_t * to_handle = get_uart_handle_by_id(to);
    if (from_handle == RT_NULL || to_handle == RT_NULL) {
        return ARGS_ERR;
    }

    pp_uart_handle_deinit(from_handle);
    pp_uart_handle_deinit(to_handle);

    _hrd_switch(from_handle, to_handle);

    if (from < to) {
        pp_uart_handle_init(from_handle);
        pp_uart_handle_init(to_handle);
    } else {
        pp_uart_handle_init(to_handle);
        pp_uart_handle_init(from_handle);
    }

    _from_id = from;
    _to_id = to;

    return 0;
}

#ifndef RT_USING_MSH
static void _switch_back(void *args) {
    while (1) {
        rt_err_t ret = rt_thread_mdelay(1000);
        PP_ASSERT(ret == RT_EOK);
        if (_from_id != 0xFF) {
            ret = rt_thread_mdelay(5 * 60 * 1000);
            PP_ASSERT(ret == RT_EOK);
            LOG_I("auto switch back");
            pp_uart_handle_switch_back();
        }
    }
}
#endif

void pp_uart_init(pp_uart_handles_t * uart_handles) {
    PP_ASSERT(uart_handles != RT_NULL);
    _uart_handles = uart_handles;
#ifndef RT_USING_MSH
    rt_thread_t thread = rt_thread_create("pp_uart", _switch_back, RT_NULL, 1 * 1024, 20, 10);
    PP_ASSERT(thread != RT_NULL);
    rt_err_t ret = rt_thread_startup(thread);
    PP_ASSERT(ret == RT_EOK);
#endif
}

static int _cmd_uart_switch(int64_t from_, int64_t to_) {
    uint8_t from = (uint8_t) from_;
    uint8_t to = (uint8_t) to_;
    if (pp_uart_handle_switch(from, to) != 0) {
        LOG_E("exec err");
        return PP_HOST_ERR_CMD_EXEC;
    }
    return 0;
}
static int _cmd_uart_switch_back(void) {
    if (pp_uart_handle_switch_back() != 0) {
        LOG_E("exec err");
        return PP_HOST_ERR_CMD_EXEC;
    }
    return 0;
}
#ifdef RT_USING_MSH
static int pp_uart_switch(int argc, char *argv[]) {
    if(argc != 3) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    int64_t from = atoi(argv[1]);
    int64_t to = atoi(argv[2]);
    return _cmd_uart_switch(from, to);
}
MSH_CMD_EXPORT(pp_uart_switch, example: pp_uart_switch 0 2);

static int pp_uart_switch_back(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_uart_switch_back();
}
MSH_CMD_EXPORT(pp_uart_switch_back, example: pp_uart_switch_back);
#endif

static int32_t _uart_hostcmd(void * args) {
    switch (_hostcmd.index) {
        case  0:  return _cmd_uart_switch(_hostcmd.arg1, _hostcmd.arg2) ;break;
        case  1:  return _cmd_uart_switch_back()                        ;break;
        default:  return PP_HOST_ERR_CMD_INDEX;
    }
}
PP_HOSTCMD_EXPORT_F(_uart_hostcmd);