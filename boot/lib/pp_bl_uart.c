#include "pp_cfg.h"
#include "stdint.h"
#include "pp_drv_uart.h"
#include "pp_drv_gpio.h"
#include "pp_bl/pp_bl_data.h"
#include "pp_drv_delay.h"
#include "pp_assert.h"

static pp_drv_uart_cb_t _recv_cb;
static uint8_t _send_flag = 0;

static void pp_bl_485_t_mode(void) {
    pp_drv_gpio_write(pp_bl_data.rs_485_mode_gpio_id, 1);
}

static void pp_bl_485_r_mode(void) {
    pp_drv_delay_ms(PP_PCB_DELAY_WRITE_COMPLETE);
    pp_drv_gpio_write(pp_bl_data.rs_485_mode_gpio_id, 0);
}

static void _uart_cb(uint8_t data) {
    if (pp_bl_data.uart_is_single != 1 || _send_flag != 1) {
        PP_ASSERT(_recv_cb != NULL);
        _recv_cb(data);
    }
}

void pp_bl_uart_init(pp_drv_uart_cb_t recv_cb) {
    PP_ASSERT(recv_cb != NULL);
    _recv_cb = recv_cb;
    if (pp_bl_data.rs_485_mode_gpio_id != 0xFF) {
        pp_drv_gpio_init(pp_bl_data.rs_485_mode_gpio_id);
        pp_bl_485_r_mode();
    }
    pp_drv_uart_init(pp_bl_data.user_data, _uart_cb);    
}

void pp_bl_uart_tx(uint8_t * data, uint16_t len) {
    PP_ASSERT(data != NULL);
    _send_flag = 1;
    if (pp_bl_data.rs_485_mode_gpio_id != 0xFF) pp_bl_485_t_mode();
    pp_drv_uart_tx(data, len);
    if (pp_bl_data.rs_485_mode_gpio_id != 0xFF) pp_bl_485_r_mode();
    _send_flag = 0;
}
