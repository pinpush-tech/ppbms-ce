#include "rtthread.h"
#include "rtdevice.h"
#include "stdint.h"
#include "can.h"
#include "pp_cfg.h"
#include "pp_assert/pp_assert.h"

typedef struct {
	struct rt_can_msg msgs[8];
	uint8_t rx_idx;
	uint8_t tx_idx;
	uint8_t tx_data_idx;
} msg_buf;

typedef struct {
	struct rt_device parent;
	rt_device_t can_device;

	rt_err_t (*can_rx_indicate)(rt_device_t dev, rt_size_t size);
	rt_size_t (*can_read)   (rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
	msg_buf can_mb;
	msg_buf uart_can_mb;
} pp_uart_can_t;

static pp_uart_can_t _uart_can = {0};

static uint8_t _next_idx(uint8_t mb) {
	return mb == 7 ? 0 : mb + 1;
}

static void _func_swap(rt_size_t (**func_a)(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size),
                       rt_size_t (**func_b)(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)) {
    PP_ASSERT(func_a != RT_NULL);
    PP_ASSERT(func_b != RT_NULL);
    rt_size_t (*temp)(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) = *func_a;
    *func_a = *func_b;
    *func_b = temp;
}

static rt_err_t _can_rx_indicate(rt_device_t dev, rt_size_t size) {
    PP_ASSERT(dev != RT_NULL);
    struct rt_can_msg rxmsg = {0};
    rxmsg.hdr = -1;
	uint8_t next_idx = 0;
    pp_uart_can_t *uart_can = &_uart_can;
    PP_ASSERT(uart_can->can_device != RT_NULL);

    _func_swap(&uart_can->can_read, &uart_can->can_device->read);
    rt_size_t read_len = rt_device_read(dev, 0, &rxmsg, sizeof(struct rt_can_msg));
    PP_ASSERT(read_len == sizeof(struct rt_can_msg));
    if (rxmsg.id == 0x7FF) {
        next_idx = _next_idx(uart_can->uart_can_mb.rx_idx);
        if (next_idx != uart_can->uart_can_mb.tx_idx) {
            uart_can->uart_can_mb.msgs[uart_can->uart_can_mb.rx_idx] = rxmsg;
            uart_can->uart_can_mb.rx_idx = next_idx;
            if (uart_can->parent.rx_indicate != RT_NULL) {
				for (uint8_t i = 0; i < rxmsg.len; i++) {
					rt_err_t ret = uart_can->parent.rx_indicate(dev, 1);
					PP_ASSERT(ret == RT_EOK);
				}
			}
        }
    } else {
        next_idx = _next_idx(uart_can->can_mb.rx_idx);
        if (next_idx != uart_can->can_mb.tx_idx) {
            uart_can->can_mb.msgs[uart_can->can_mb.rx_idx] = rxmsg;
            uart_can->can_mb.rx_idx = next_idx;
            PP_ASSERT(uart_can->can_rx_indicate != RT_NULL);
            rt_err_t ret = uart_can->can_rx_indicate(dev, size);
            PP_ASSERT(ret == RT_EOK);
        }
    }
    _func_swap(&uart_can->can_read, &uart_can->can_device->read);
    return 0;
}
static rt_size_t _uart_can_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size) {
    PP_ASSERT(dev != RT_NULL);
    PP_ASSERT(buffer != RT_NULL);
	if (size == 0) {
		return 0;
	}
	pp_uart_can_t * uart_can = (pp_uart_can_t *)dev;
	uint8_t msg_cnt = (size + 7) / 8;
	uint8_t remain = size;
	for (uint8_t i = 0; i < msg_cnt; i++) {
		struct rt_can_msg msg = {0};
		msg.id = 0x7FE;
		msg.ide = RT_CAN_STDID;
		msg.rtr = RT_CAN_DTR;
		msg.len = remain <= 8 ? remain : 8;
		rt_memcpy(msg.data, ((uint8_t *)buffer + (size - remain)), msg.len);
		rt_size_t write_len = rt_device_write(uart_can->can_device, 0, &msg, sizeof(struct rt_can_msg));
		PP_ASSERT(write_len == sizeof(struct rt_can_msg));
		remain -= msg.len;
	}
	return size;
}
static rt_size_t _uart_can_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size) {
    PP_ASSERT(dev != RT_NULL);
    PP_ASSERT(buffer != RT_NULL);
	pp_uart_can_t * uart_can = (pp_uart_can_t *)dev;
	if (uart_can->uart_can_mb.rx_idx == uart_can->uart_can_mb.tx_idx) {
		return 0;
	}
	(* (uint8_t *)buffer) = uart_can->uart_can_mb.msgs[uart_can->uart_can_mb.tx_idx].data[uart_can->uart_can_mb.tx_data_idx];
	if (uart_can->uart_can_mb.tx_data_idx + 1 == uart_can->uart_can_mb.msgs[uart_can->uart_can_mb.tx_idx].len) {
		uart_can->uart_can_mb.tx_data_idx = 0;
		uart_can->uart_can_mb.tx_idx = _next_idx(uart_can->uart_can_mb.tx_idx);
	} else {
		uart_can->uart_can_mb.tx_data_idx++;
	}
	return 1;
}
static rt_size_t _can_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size) {
    PP_ASSERT(dev != RT_NULL);
    PP_ASSERT(buffer != RT_NULL);
	pp_uart_can_t * uart_can = &_uart_can;
	if (uart_can->can_mb.rx_idx == uart_can->can_mb.tx_idx) {
		return 0;
	}
	rt_memcpy(buffer, &uart_can->can_mb.msgs[uart_can->can_mb.tx_idx], sizeof(struct rt_can_msg));
	uart_can->can_mb.tx_idx = _next_idx(uart_can->can_mb.tx_idx);
	return sizeof(struct rt_can_msg);
}

static rt_err_t _uart_can_open(rt_device_t dev, rt_uint16_t oflag) {
	PP_ASSERT(dev != RT_NULL);
	if (dev->ref_count == 0) {
		pp_uart_can_t * uart_can = (pp_uart_can_t *)dev;
        PP_ASSERT(uart_can->can_device != RT_NULL);
		uart_can->can_rx_indicate = uart_can->can_device->rx_indicate;
		uart_can->can_read = uart_can->can_device->read;
		uart_can->can_device->rx_indicate = _can_rx_indicate;
		uart_can->can_device->read = _can_read;
	}
	return RT_EOK;
}

static rt_err_t _uart_can_close(rt_device_t dev) {
	PP_ASSERT(dev != RT_NULL);
	if (dev->ref_count == 0) {
		pp_uart_can_t * uart_can = (pp_uart_can_t *)dev;
        PP_ASSERT(uart_can->can_device != RT_NULL);
		uart_can->can_device->rx_indicate = uart_can->can_rx_indicate;
		uart_can->can_device->read = uart_can->can_read;
		uart_can->can_rx_indicate = RT_NULL;
		uart_can->can_read = RT_NULL;
		rt_memset(&uart_can->can_mb, 0, sizeof(msg_buf));
		rt_memset(&uart_can->uart_can_mb, 0, sizeof(msg_buf));
	}
	return RT_EOK;
}

static rt_err_t _uart_can_init(rt_device_t dev) {
	return RT_EOK;
}
static rt_err_t _uart_can_control(rt_device_t dev, int cmd, void *args) {
	return RT_EOK;
}

static rt_err_t _can_rx_call(rt_device_t dev, rt_size_t size) {
    return RT_EOK;
}

void pp_uart_can_init(char * can_name, char * uart_can_name) {
    PP_ASSERT(can_name != RT_NULL);
    PP_ASSERT(uart_can_name != RT_NULL);

	_uart_can.can_device 		 = rt_device_find(can_name);

	_uart_can.parent.type 		 = RT_Device_Class_Char;
	_uart_can.parent.open        = _uart_can_open;
	_uart_can.parent.close       = _uart_can_close;
	_uart_can.parent.read        = _uart_can_read;
	_uart_can.parent.write       = _uart_can_write;
	_uart_can.parent.control	 = _uart_can_control;
	_uart_can.parent.init		 = _uart_can_init;
	rt_err_t ret = rt_device_register(&_uart_can.parent, uart_can_name, 0);
	PP_ASSERT(ret == RT_EOK);
    ret = rt_device_set_rx_indicate(_uart_can.can_device, _can_rx_call);
    PP_ASSERT(ret == RT_EOK);
	ret = rt_device_open(_uart_can.can_device, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
	PP_ASSERT(ret == RT_EOK);
	ret = rt_device_control(_uart_can.can_device, RT_CAN_CMD_SET_BAUD, (void *)CAN250kBaud);
	PP_ASSERT(ret == RT_EOK);
}
