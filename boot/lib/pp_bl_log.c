#include "pp_cfg.h"
#include "stdio.h"
#include "pp_drv_uart.h"
#include "pp_drv_delay.h"
#include "pp_bl_uart.h"
#include "pp_drv_can.h"
#include "pp_bl/pp_bl_data.h"
#include "pp_assert.h"

static char _log_buff[PP_PCB_LOG_BUFFER_SIZE] = {0};
static int _log_index = 0;

int fputc(int ch, FILE* f) {
    PP_ASSERT(_log_index < PP_PCB_LOG_BUFFER_SIZE);
    _log_buff[_log_index++] = (char)ch;
    if (_log_index == PP_PCB_LOG_BUFFER_SIZE || ch == '\n') {
        if (pp_bl_data.dev_type == 0) pp_bl_uart_tx((uint8_t *)_log_buff, _log_index);
        if (pp_bl_data.dev_type == 1) pp_drv_can_tx((uint8_t *)_log_buff, _log_index);
        _log_index = 0;
    }

    return ch;
}
