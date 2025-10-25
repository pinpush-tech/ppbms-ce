#include "pp_data/pp_data_sys.h"
#include "pp_env/pp_env.h"
#include "pp_mcu/pp_mcu.h"
#include "ulog.h"
#include "rtthread.h"
#include "pp_assert/pp_assert.h"
#include "pp_data/pp_data_afe.h"
#include "pp_continued/pp_continued.h"
#include "pp_afe/pp_afe.h"
#include "pp_data/pp_data_burn.h"
#include "pp_log/pp_log.h"
#include "pp_export/pp_export_f.h"
#include "pp_data/pp_data_export.h"

static void _sys_restart(uint8_t store) {
    _sys.running_status = 0;
    if (store == 1) {
        pp_env_store();
    }
    pp_export_f_deinit(RT_NULL);
    rt_err_t ret = rt_thread_mdelay(PP_PCB_DELAY_POWER_COMPLETE);
    PP_ASSERT(ret == RT_EOK);
    pp_mcu_restart();
}

#ifdef PP_PCB_USING_SYS_STOP
static void _sys_chk_stop(void) {
    if (_sys.stop_status == 0 && !(_sys.mos_lock_chg || _sys.mos_lock_dsg)) {
        if (   PP_CONTINUED(_sys.stop_reset_flag == 0 && _afe.bat_status == 0 , PP_PCB_SYS_STOP_TIME)
            || _sys.stop_flag) {
            LOG_I("mcu stop restart");
            _sys.stop_status = 1;
            _sys_restart(1);
        }
        _sys.stop_reset_flag = 0;
    }
}
static void _sys_chk_wkup(void) {
    if (_sys.stop_status == 1) {
        if( PP_CONTINUED(_afe.bat_status != 0 , 1 * 1000)
         || _sys.mos_lock_chg
         || _sys.mos_lock_chg
         || _sys.wkup_flag ) {
            LOG_I("mcu wkup restart");
            _sys.stop_status = 0;
            _sys_restart(1);
        }
    }
}
#endif

static void _sys_chk_restore(void) {
    if( _sys.restore_afe_flag == 1 || _sys.restore_sox_flag == 1 ) {
        _sys.running_status = 0;
        pp_env_store();
        if ( _sys.restore_afe_flag == 1 ) {
            pp_log_F_mcu("restore afe cfg");
            pp_env_del(PP_DATA_EXPORT_ENV_AFE_CFG_SH);
        }
        if( _sys.restore_sox_flag == 1 ) {
            pp_log_F_mcu("restore sox");
            pp_env_del(PP_DATA_EXPORT_ENV_SOX_S);
        }
        _sys_restart(0);
    }
}

#ifdef PP_PCB_USING_SYS_SHUTDOWN
static void _sys_chk_shutdown(void) {
    if (   PP_CONTINUED(_afe_cfg.shutdown_off_flag == 0 && _afe.bat_status != 2 &&  _afe.uv2_status == 1, 10 * 60 * 1000)
        || _sys.shutdown_flag == 1) {
        _sys.running_status = 0;
        _sys.shutdown_flag = 0;
        pp_env_store();
        if (_sys.shutdown_flag == 0) {
            pp_log_F_mcu("cell min vol is %d shutdown", _afe.cell_min_vol);
        }else{
            pp_log_F_mcu("shutdown_flag == 1");
        }
        rt_err_t ret = rt_thread_mdelay(PP_PCB_DELAY_WRITE_COMPLETE);
        PP_ASSERT(ret == RT_EOK);
        pp_export_f_deinit(RT_NULL);
        pp_afe_shut_down();
    }
}
#endif

static void _sys_chk_env_store(void) {
    if(PP_CONTINUED(1, 10 * 60 * 1000) || _sys.store_flag == 1) {
		_sys.store_flag = 0;
        pp_env_store();
    }
}

static void _sys_chk_restart(void) {
    uint8_t restart_flag = 0;
    if (_sys.ota_done == 1) {
        restart_flag = 1;
    }

    if (_sys.restart_flag == 1) {
        restart_flag = 2;
    }
    if (_sys.restart4fota_flag == 1) {
        restart_flag = 3;
    }
    if (restart_flag != 0) {
        LOG_I("pp_mcu_restart : %d", restart_flag);
        _sys_restart(1);
    }
}

static int32_t _sys_chk(void * args) {
    _sys_chk_restore();
#ifdef PP_PCB_USING_SYS_SHUTDOWN
    _sys_chk_shutdown();
#endif
#ifdef PP_PCB_USING_SYS_STOP
    _sys_chk_stop();
    _sys_chk_wkup();
#endif
    _sys_chk_restart();
    _sys_chk_env_store();
    if(_sys.first_cycle_flag == 1) {
        _sys.first_cycle_flag = 0;
    }
    return 0;
}
PP_PULSE2_EXPORT_F(_sys_chk);

static int32_t _sys_chk_abnmrst(void * args) {
    if (_sys.running_status == 1) {
        pp_log_F_mcu("mcu abnormally restart");
    } else {
        _sys.running_status = 1;
        pp_env_store();
    }
    return 0;
}
PP_APP0_EXPORT_F(_sys_chk_abnmrst);
