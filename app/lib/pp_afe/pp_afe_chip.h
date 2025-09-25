#pragma once

#include "stdint.h"
#include "rtthread.h"

#include "pp_cfg.h"

extern rt_base_t pp_afe_scl;
extern rt_base_t pp_afe_sda;
extern struct rt_i2c_bus_device *pp_afe_i2c_bus;

int32_t pp_afe_chip_init(rt_device_t i2c_handle, rt_base_t scl, rt_base_t sda);
void pp_afe_chip_clt4flag(void);
void pp_afe_chip_clt4data(void);
void pp_afe_chip_set4flag(void);
void pp_afe_chip_set4data(void);
void pp_afe_chip_opr4flag(void);
void pp_afe_chip_opr4data(void);

void pp_afe_chip_pdsg_ctl(uint8_t pdsg_enable);
void pp_afe_chip_mos_ctl(uint8_t dsg_enable, uint8_t chg_enable);

void pp_afe_chip_shut_down(void); // pp_afe_xxxx_r.c

#define _AFE_MOS_OPR4STATUS(n, d, c)                                        \
    do {                                                                    \
        static uint8_t old_##n##_status = 0;                                \
        _afe_mos_opr4status(&old_##n##_status, &_afe.n##_status, (d), (c)); \
    } while (0)
void _afe_mos_opr4status(uint8_t *old_status, uint8_t *new_status, uint8_t opr_dsg, uint8_t opr_chg);
#define _AFE_STATUS_CHANGED(n)                                      \
    ({                                                              \
        static uint8_t old_##n##_status = 0;                        \
        _afe_status_changed(&old_##n##_status, &_afe.n##_status);   \
    })
uint8_t _afe_status_changed(uint8_t *old_status, uint8_t *new_status);
void _afe_status_log(char * name, uint8_t status, uint8_t ch);
int16_t _afe_cal_temp(double rv);
int32_t _afe_cal_resi(int16_t tv);
int64_t _check_range(int64_t value, int64_t max, int64_t min, char * name);
void _afe_i2c_recv(uint8_t * buffer, uint8_t reg_addr, uint8_t size);
void _afe_i2c_send(uint8_t * buffer, uint8_t reg_addr, uint8_t size);
int32_t _afe_cfg_check(uint8_t * reg_addr,
#if (PP_AFE_CHIP_I2C_REG_SIZE == 2)
uint16_t * reg_val_mcu
#else
uint8_t * reg_val_mcu
#endif
, uint8_t reg_cnt);
