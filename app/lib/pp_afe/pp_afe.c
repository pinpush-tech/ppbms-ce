#include <string.h>
#include "rtthread.h"
#include "pp_assert/pp_assert.h"
#include "pp_mcu/pp_mcu_wdg.h"
#include "rtdevice.h"
#include "pp_continued/pp_continued.h"
#include "pp_afe/pp_afe_chip.h"
#include "pp_afe_chip_cfg.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_sys.h"
#include "pp_afe/pp_afe.h"
#include "ulog.h"
#include "pp_cfg.h"
#include "pp_log/pp_log.h"
#include "pp_sox/pp_sox.h"
#include "pp_afe/pp_afe_soft.h"
#include "pp_pin/pp_pin.h"
#include "pp_export/pp_export_f.h"

static struct rt_semaphore _afe_chip_sem;

static void _syspulse_entry(void *args) {
    while (1) {
        pp_mcu_wdg_keepalive();
#ifdef PP_PCB_USING_AFE_INT_WK
        rt_sem_take(&_afe_chip_sem, 4 * _afe.afe_cyc_time);
#else
        rt_sem_take(&_afe_chip_sem, 1 * _afe.afe_cyc_time);
#endif

        // 有flag有data、无flag无data、有flag无data
        pp_afe_chip_clt4flag();
        pp_afe_chip_set4flag();
        pp_afe_soft_set4flag();
        pp_afe_chip_opr4flag();
        pp_afe_soft_opr4flag();

        // 无flag无data
        if(_afe.afe_has_data == 0 && _afe.afe_has_flag == 0) {// syspulse: timeout
            LOG_E("in_afe_has_data == 0 && in_afe_has_flag == 0");
            _afe.afe_times = 4;

            pp_afe_chip_set4data();
            pp_afe_soft_set4data();
            pp_afe_chip_opr4data();
            pp_afe_soft_opr4data();
            pp_export_f_pulse(RT_NULL);
        }

        // 有flag有data
        if(_afe.afe_has_data == 1) {// syspulse: normal
            pp_afe_chip_clt4data();
            pp_afe_chip_set4data();
            pp_afe_soft_set4data();
            pp_afe_chip_opr4data();
            pp_afe_soft_opr4data();
            pp_sox_cal();
            pp_export_f_pulse(RT_NULL);
        }
    }
}
#ifdef PP_PCB_USING_AFE_INT_WK
static void _afe_irq_cb(void) {
    rt_err_t ret = rt_sem_release(&_afe_chip_sem);
    PP_ASSERT(ret == RT_EOK);
}
#endif

static int32_t _afe_init(void * args) {
    rt_err_t ret = rt_sem_init(&_afe_chip_sem, "afe_chip_sem", 0, RT_IPC_FLAG_FIFO);
    PP_ASSERT(ret == RT_EOK);

    rt_device_t i2c_handle = rt_device_find(PP_PCB_AFE_I2C_NAME);
    PP_ASSERT(i2c_handle != RT_NULL);
    if (pp_afe_chip_init(i2c_handle, PP_PCB_AFE_I2C_SCL_PIN, PP_PCB_AFE_I2C_SDA_PIN) != 0) {
        _afe.afe_chip_init_fail = 1;
    }
    pp_afe_soft_init();

#ifdef PP_PCB_USING_AFE_INT_WK
    pp_pin_init_irq_failing(PP_PCB_AFE_INT_WK_PIN, PIN_MODE_INPUT_PULLUP, _afe_irq_cb);
#endif

#if defined(PP_PCB_USING_AFE_FUSE) && !defined(PP_PCB_CTX_DEBUG)
    rt_pin_mode(PP_PCB_AFE_FUSE_PIN, PIN_MODE_OUTPUT);
#endif

    rt_thread_t pulse_e_thread = rt_thread_create("pulse_e", _syspulse_entry, RT_NULL, 3 * 1024 , 20, 10);
    PP_ASSERT(pulse_e_thread != RT_NULL);
    ret = rt_thread_startup(pulse_e_thread);
    PP_ASSERT(ret == RT_EOK);
    return 0;
}
PP_APP0_EXPORT_F(_afe_init);

static uint16_t _sys_mos_lock(void) {
    return (_afe.sys_mos_lock[0] + _afe.sys_mos_lock[1] + _afe.sys_mos_lock[2]);
}
// type   用户-2，保护-1，系统0-2
void pp_afe_mos_ctl(int8_t dsg_enable, int8_t chg_enable, int8_t pdsg_enable, int8_t type) {
    PP_ASSERT(dsg_enable >= -1 && dsg_enable <= 1);
    PP_ASSERT(chg_enable >= -1 && chg_enable <= 1);
    PP_ASSERT(pdsg_enable >= -1 && pdsg_enable <= 1);
    PP_ASSERT(type >= -2 && type <= 2);

    uint8_t dsg_mos_ctl_old = _afe.dsg_mos_ctl; 
    uint8_t chg_mos_ctl_old = _afe.chg_mos_ctl;
    uint8_t pdsg_mos_ctl_old = _afe.pdsg_mos_ctl;

    if (dsg_enable == 1) {
        if ( type >= 0                                            ) _afe.sys_mos_lock[type] = 0;
        if (!_sys.mos_lock_dsg && !_sys_mos_lock() && _afe.can_dsg) _afe.dsg_mos_ctl = 1;
    }
    if (dsg_enable == 0) {
        if (type >= 0                                             ) _afe.sys_mos_lock[type] = 1;
                                                                    _afe.dsg_mos_ctl = 0;
    }
    if (chg_enable == 1) {
        if ( type >= 0                                            ) _afe.sys_mos_lock[type] = 0;
        if (!_sys.mos_lock_chg && !_sys_mos_lock() && _afe.can_chg) _afe.chg_mos_ctl = 1;
    }
    if (chg_enable == 0) {
        if (type >= 0                                             ) _afe.sys_mos_lock[type] = 1;
                                                                    _afe.chg_mos_ctl = 0;
    }
    if (pdsg_enable == 1) {
        if (!_sys.mos_lock_dsg                     && _afe.can_dsg) _afe.pdsg_mos_ctl = 1;
    }
    if (pdsg_enable == 0) {
                                                                    _afe.pdsg_mos_ctl = 0;
    }

    if (pdsg_enable != -1)                      pp_afe_chip_pdsg_ctl(_afe.pdsg_mos_ctl);
    if (dsg_enable != -1 || chg_enable != -1)   pp_afe_chip_mos_ctl(_afe.dsg_mos_ctl, _afe.chg_mos_ctl);
    if (_afe.dsg_mos_ctl != dsg_mos_ctl_old) {
        if (_afe.dsg_mos_ctl == 1 && type == -2) LOG_I("DSG MOS ON by user");
        if (_afe.dsg_mos_ctl == 1 && type == -1) LOG_I("DSG MOS ON by protector");
        if (_afe.dsg_mos_ctl == 1 && type >= 0 ) LOG_I("DSG MOS ON by system(%d)", type);
        if (_afe.dsg_mos_ctl == 0 && type == -2) LOG_I("DSG MOS OFF by user");
        if (_afe.dsg_mos_ctl == 0 && type == -1) LOG_I("DSG MOS OFF by protector");
        if (_afe.dsg_mos_ctl == 0 && type >= 0 ) LOG_I("DSG MOS OFF by system(%d)", type);
    }
    if (_afe.chg_mos_ctl != chg_mos_ctl_old) {
        if (_afe.chg_mos_ctl == 1 && type == -2) LOG_I("CHG MOS ON by user");
        if (_afe.chg_mos_ctl == 1 && type == -1) LOG_I("CHG MOS ON by protector");
        if (_afe.chg_mos_ctl == 1 && type >= 0 ) LOG_I("CHG MOS ON by system(%d)", type);
        if (_afe.chg_mos_ctl == 0 && type == -2) LOG_I("CHG MOS OFF by user");
        if (_afe.chg_mos_ctl == 0 && type == -1) LOG_I("CHG MOS OFF by protector");
        if (_afe.chg_mos_ctl == 0 && type >= 0 ) LOG_I("CHG MOS OFF by system(%d)", type);
    }
    if (_afe.pdsg_mos_ctl != pdsg_mos_ctl_old) {
        if (_afe.pdsg_mos_ctl == 1)              LOG_I("PDSG MOS ON by system(%d)", type);
        if (_afe.pdsg_mos_ctl == 0)              LOG_I("PDSG MOS OFF by system(%d)", type);
    }
}

void pp_afe_shut_down(void) {
    pp_afe_chip_shut_down();
}
