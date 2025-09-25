#include "pp_cfg.h"
#include "pp_bl_host.h"
#include "pp_drv_uart.h"
#include "pp_drv_can.h"
#include "pp_bl_timer.h"
#include "pp_bl/pp_bl_data.h"
#include "pp_tools/pp_tools.h"
#include "stdio.h"
#include "pp_mcu/pp_mcu_flash.h"
#include "pp_drv_gpio.h"
#include "pp_bl_uart.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_assert.h"
#include "pp_data/pp_data_export.h"

typedef struct {
    uint16_t    code;
    uint16_t    crc;
    uint16_t    opr;
    uint16_t    index;
    uint16_t    data_length;
    uint16_t    pack_num;
    uint16_t    err_code;
} pp_host_head_t;

#define    HEAD_LENGTH           14
#define    HEAD_CODE_CRC_LENGTH   4
#define    SEND_MAX_NUM         256
#define    RECV_MAX_NUM         (4 * 1024) + HEAD_LENGTH

typedef struct {
    uint8_t             recv_buffer[RECV_MAX_NUM + 128];
    uint8_t             send_buffer[SEND_MAX_NUM];
    pp_host_head_t *    recv_head;
    pp_host_head_t *    send_head;
    uint8_t *           recv_data;
    uint8_t *           send_data;
    uint16_t            recv_tail;
    uint8_t             recv_finish;
    uint8_t             processing;
    pp_bl_timer_t          recv_timeout;
    pp_bl_timer_t          fota_timeout;
}pp_host_handle_t;

static pp_host_handle_t _handle;
volatile uint8_t pp_bl_host_fota_state;

static uint8_t _operate(void) {
    PP_ASSERT(_handle.recv_head != NULL);
    PP_ASSERT(_handle.send_head != NULL);
    PP_ASSERT(_handle.recv_buffer != NULL);
    PP_ASSERT(_handle.recv_data != NULL);

    if (_handle.recv_head->crc != pp_tools_crc16(_handle.recv_buffer + HEAD_CODE_CRC_LENGTH, HEAD_LENGTH - HEAD_CODE_CRC_LENGTH + _handle.recv_head->data_length)) {
        return PP_HOST_ERR_CRC;
    }
    if(_handle.recv_head->opr == 001) {
        _handle.send_head->data_length = 0x0000;
        static uint16_t expect_pack_id = 0;
        static uint32_t download_size = 0;
        if(pp_bl_host_fota_state == PP_BL_HOST_STATE_FOTADONE) {
            return PP_HOST_ERR_HAVE_BEEN_OTA;
        }
        if(_handle.recv_head->index == 0) {
            download_size = 0;
            expect_pack_id = 0;
        } else if (_handle.recv_head->index == expect_pack_id - 1) {
            return 0;
        }
        if (expect_pack_id != _handle.recv_head->index) {
            return PP_HOST_ERR_INDEX;
        }
        if (pp_mcu_flash_erase(PP_PCB_MCU_FLASH_APP_OFFSET + download_size, _handle.recv_head->data_length) < 0) {
            return PP_HOST_ERR_FAL;
        }
        if (pp_mcu_flash_write(PP_PCB_MCU_FLASH_APP_OFFSET + download_size, _handle.recv_data, _handle.recv_head->data_length) < 0) {
            return PP_HOST_ERR_FAL;
        }
        download_size += _handle.recv_head->data_length;
        expect_pack_id ++;
        if(expect_pack_id == _handle.recv_head->pack_num) {
            download_size = 0;
            expect_pack_id = 0;
            pp_bl_host_fota_state = PP_BL_HOST_STATE_FOTADONE;
        }
        return 0;
    }
    if (_handle.recv_head->opr == 002) {
        _handle.send_head->data_length = 0;
        pp_bl_host_fota_state = PP_BL_HOST_STATE_FOTAING;
        pp_bl_timer_ctrl(&_handle.fota_timeout, 0);
        return 0;
    }
    if(_handle.recv_head->opr == 012 && _handle.recv_head->index == PP_DATA_EXPORT_HOST_BL) {
        PP_ASSERT(_handle.send_data != NULL);
        _handle.send_head->data_length = 1;
        *_handle.send_data = pp_bl_host_fota_state;
        return 0;
    }
    return PP_HOST_ERR_OPR;
}

static void _route_this(void) {
    PP_ASSERT(_handle.recv_head != NULL);
    PP_ASSERT(_handle.send_head != NULL);
    PP_ASSERT(_handle.send_buffer != NULL);

    if (_handle.recv_tail >= HEAD_LENGTH && _handle.recv_head->code == 0xAEAE) {
        if (_handle.recv_tail == HEAD_LENGTH + _handle.recv_head->data_length) {
            _handle.send_head->code = 0xEAEA;
            _handle.send_head->opr = _handle.recv_head->opr;
            _handle.send_head->index = _handle.recv_head->index;
            _handle.send_head->pack_num = _handle.recv_head->pack_num;
            _handle.send_head->err_code = _operate();
            _handle.send_head->crc = pp_tools_crc16(_handle.send_buffer + HEAD_CODE_CRC_LENGTH, HEAD_LENGTH - HEAD_CODE_CRC_LENGTH + _handle.send_head->data_length);
            if (pp_bl_data.dev_type == 0) pp_bl_uart_tx(_handle.send_buffer, HEAD_LENGTH + _handle.send_head->data_length);
            if (pp_bl_data.dev_type == 1) pp_drv_can_tx(_handle.send_buffer, HEAD_LENGTH + _handle.send_head->data_length);
        }
    }
}

void pp_bl_host_process(void) {
    if (_handle.recv_finish == 0) {
        return;
    }
    if (_handle.recv_tail != 0) {
        _handle.processing = 1;
        _route_this();
        _handle.processing = 0;
        _handle.recv_tail = 0;
    }
    _handle.recv_finish = 0;
}

static void _recv_data(uint8_t data) {
    if (_handle.processing != 1) {
        _handle.recv_buffer[_handle.recv_tail] = data;
        _handle.recv_tail++;
        if (_handle.recv_tail > RECV_MAX_NUM) {
            _handle.recv_tail = 0;
        }
        pp_bl_timer_ctrl(&_handle.recv_timeout, 1);
    }
}

static void _recv_timeout(void *param) {
    pp_bl_timer_ctrl(&_handle.recv_timeout, 0);
    _handle.recv_finish = 1;
}

static void _fota_timeout(void *param) {
    if(pp_bl_host_fota_state == PP_BL_HOST_STATE_WAITING) {
        pp_bl_host_fota_state = PP_BL_HOST_STATE_TIMEOUT;
    }
    pp_bl_timer_ctrl(&_handle.fota_timeout, 0);
}

void pp_bl_host_init(void) {
    _handle.recv_head = (pp_host_head_t *) _handle.recv_buffer;
    _handle.send_head = (pp_host_head_t *) _handle.send_buffer;
    _handle.recv_data = _handle.recv_buffer + HEAD_LENGTH;
    _handle.send_data = _handle.send_buffer + HEAD_LENGTH;
    if (pp_bl_data.dev_type == 0) pp_bl_uart_init(_recv_data);
    if (pp_bl_data.dev_type == 1) pp_drv_can_init(pp_bl_data.user_data, _recv_data);
    uint16_t fota_wait_time = pp_bl_data.fota_wait_time == 0xFFFF ? 0 : pp_bl_data.fota_wait_time;
    printf("fota_wait_time: %d\r\n", fota_wait_time);
    PP_ASSERT(fota_wait_time <= 3000);
    pp_bl_timer_hw_init();
    pp_bl_timer_init(&_handle.recv_timeout, 255, NULL, _recv_timeout);
    pp_bl_timer_init(&_handle.fota_timeout, fota_wait_time, NULL, _fota_timeout);
    pp_bl_timer_ctrl(&_handle.fota_timeout, 1);
}

void pp_bl_host_deinit(void) {
    if (pp_bl_data.dev_type == 0) pp_drv_uart_deinit();
    if (pp_bl_data.dev_type == 1) pp_drv_can_deinit();
    pp_bl_timer_hw_deinit();
}
