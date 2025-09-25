#include "pp_afe_chip_cfg.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "stdint.h"
#include "pp_assert/pp_assert.h"
#include "pp_log/pp_log.h"
#include "pp_cfg.h"
#include "math.h"
#include "ulog.h"
#include "stdlib.h"
#include "pp_tools/pp_tools.h"
#include "pp_afe/pp_afe.h"
#include "pp_data/pp_data_afe.h"

rt_base_t pp_afe_scl = 0;
rt_base_t pp_afe_sda = 0;
struct rt_i2c_bus_device *pp_afe_i2c_bus = RT_NULL;

int16_t _afe_cal_temp(double rv) { //K欧姆
    PP_ASSERT(rv > 0);
    PP_ASSERT(PP_PCB_NTC_25_RV > 0);
    PP_ASSERT(PP_PCB_NTC_B_VALUE != 0);
    if (rv <= 0) return 999;
    return round((1.0 / (1.0 / 298.15 + (log(rv / PP_PCB_NTC_25_RV) + rv * PP_PCB_NTC_RT_M_VALUE) / PP_PCB_NTC_B_VALUE)) - 273.15);
}

int32_t _afe_cal_resi(int16_t tv) { //欧姆

    return exp((1.0 / (tv + 273.15) - 1.0 / 298.15) * PP_PCB_NTC_B_VALUE * (1 - abs(80 - tv) * PP_PCB_NTC_TR_M_VALUE)) * PP_PCB_NTC_25_RV * 1000;
}

int64_t _check_range(int64_t value, int64_t max, int64_t min, char * name) {
    PP_ASSERT(name != RT_NULL);
    PP_ASSERT(min <= max);

    int64_t new_value = value;
    new_value = (new_value > max ? max : new_value);
    new_value = (new_value < min ? min : new_value);
    if (new_value != value) {
        _afe.afe_cfg_err = 1;
        LOG_E("%s is Out of range : %d", name, new_value);
    }
    return new_value;
}

void _afe_i2c_recv(uint8_t * buffer, uint8_t reg_addr, uint8_t size) {
    PP_ASSERT(buffer != RT_NULL);

    uint8_t temp_buffer[6] = {(PP_AFE_CHIP_I2C_ADDR << 1), 0x00, (PP_AFE_CHIP_I2C_ADDR << 1) + 1, 0x00, 0x00, 0x00};
    uint8_t crc = 0;
    uint8_t crc_index = (PP_AFE_CHIP_I2C_REG_SIZE == 2 ? 5 : 4);
    for(uint8_t i = 0; i < size; i++) {
        uint8_t crc_err_cnt;
        for(crc_err_cnt = 0; crc_err_cnt < 3; crc_err_cnt++) {
            temp_buffer[1] = reg_addr;
            rt_i2c_master_send(pp_afe_i2c_bus, PP_AFE_CHIP_I2C_ADDR, RT_I2C_NO_STOP, &reg_addr, 1);
            rt_pin_write(pp_afe_scl, PIN_HIGH);
            rt_pin_write(pp_afe_sda, PIN_HIGH);
            rt_i2c_master_recv(pp_afe_i2c_bus, PP_AFE_CHIP_I2C_ADDR, RT_NULL, &temp_buffer[3], PP_AFE_CHIP_I2C_REG_SIZE + 1);
            crc = pp_tools_crc8(temp_buffer, PP_AFE_CHIP_I2C_REG_SIZE + 3);
            if(crc == temp_buffer[crc_index]) {
                break;
            } else {
                LOG_E("afe read err, addr:0X%02X,err_value:%02X %02X,cnt:%d",reg_addr,temp_buffer[3],temp_buffer[4],crc_err_cnt);
            }
        }
        if(crc_err_cnt < 3) {
            if(PP_AFE_CHIP_I2C_REG_SIZE == 2) {
                *(buffer + i * PP_AFE_CHIP_I2C_REG_SIZE) = temp_buffer[4];
                *(buffer + i * PP_AFE_CHIP_I2C_REG_SIZE + 1) = temp_buffer[3];
            } else {
                *(buffer + i * PP_AFE_CHIP_I2C_REG_SIZE) = temp_buffer[3];
            }
        } else {
            LOG_E("afe read fatal err,addr:0X%02X", reg_addr);
        }
        reg_addr += 1;
    }
}

void _afe_i2c_send(uint8_t * buffer, uint8_t reg_addr, uint8_t size) {
    PP_ASSERT(buffer != RT_NULL);

    uint8_t send_buffer[5] = {(PP_AFE_CHIP_I2C_ADDR << 1)};
    for (uint8_t i = 0; i < size; i++) {
        send_buffer[1] = reg_addr;
        if(PP_AFE_CHIP_I2C_REG_SIZE == 2) {
            send_buffer[2] = *(buffer + 1);
            send_buffer[3] = *buffer;
            send_buffer[4] = pp_tools_crc8(send_buffer, 4);
        } else {
            send_buffer[2] = *buffer;
            send_buffer[3] = pp_tools_crc8(send_buffer, 3);
        }
        rt_i2c_master_send(pp_afe_i2c_bus, PP_AFE_CHIP_I2C_ADDR, RT_NULL, send_buffer + 1, 2 + PP_AFE_CHIP_I2C_REG_SIZE);
        reg_addr += 1;
        buffer += PP_AFE_CHIP_I2C_REG_SIZE;
    }
}

int32_t _afe_cfg_check(uint8_t * reg_addr,
#if (PP_AFE_CHIP_I2C_REG_SIZE == 2)
uint16_t * reg_val_mcu
#else
uint8_t * reg_val_mcu
#endif
, uint8_t reg_cnt) {
    PP_ASSERT(reg_addr != RT_NULL);
    PP_ASSERT(reg_val_mcu != RT_NULL);

#if (PP_AFE_CHIP_I2C_REG_SIZE == 2)
    uint16_t buf = 0;
#else
    uint8_t buf = 0;
#endif

    for (uint8_t i = 0; i < reg_cnt; i++) {
        _afe_i2c_recv((uint8_t *)&buf, reg_addr[i], 1);
        LOG_I("%02X : %04X", reg_addr[i], buf);
        if (buf != reg_val_mcu[i]) {
			LOG_I("%02X : %04X  %04X", reg_addr[i], buf, reg_val_mcu[i]);
            LOG_E("afe write otp & cfg err!");
            return -1;
        }
    }
    return 0;
}

uint8_t _afe_status_changed(uint8_t *old_status, uint8_t *new_status) {
    PP_ASSERT(old_status != RT_NULL);
    PP_ASSERT(new_status != RT_NULL);

    uint8_t is_changed = 0;
    if (*old_status != *new_status) is_changed = 1;
    *old_status = *new_status;
    return is_changed;
}

void _afe_status_log(char * name, uint8_t status, uint8_t ch) {
    PP_ASSERT(name != RT_NULL);
    PP_ASSERT(status <= 1);
    PP_ASSERT(ch <= 1);
    if (status) {
        if(ch == 0) pp_log_F_afe("%s", name);
        else pp_log_F_afe("%s|%hhu|%hhu", name, _afe.minv_cell_ch, _afe.maxv_cell_ch);
    } else {
        LOG_I("%s rls", name);
    }
}

void _afe_mos_opr4status(uint8_t *old_status, uint8_t *new_status, uint8_t opr_dsg, uint8_t opr_chg) {
    PP_ASSERT(old_status != RT_NULL);
    PP_ASSERT(new_status != RT_NULL);

    if (_afe_status_changed(old_status, new_status)) {
        if (*new_status == 1) pp_afe_mos_ctl(opr_dsg ? 0 : -1, opr_chg ? 0 : -1, -1, -1);
        if (*new_status == 0) pp_afe_mos_ctl(opr_dsg ? 1 : -1, opr_chg ? 1 : -1, -1, -1);
    }
}
