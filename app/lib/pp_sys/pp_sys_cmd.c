#include "pp_uart/pp_uart.h"
#include "stdlib.h"
#include "string.h"
#include "pp_data/pp_data_sys.h"
#include "pp_env/pp_env.h"
#include "pp_data/pp_data_afe.h"
#include "ulog.h"
#include "pp_export/pp_export_f.h"
#include "pp_persist/pp_persist.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_data/pp_data_hostcmd.h"

static int _cmd_restart(void) {
    _sys.restart_flag = 1;
    return 0;
}
static int _cmd_restart4fota(void) {
    pp_bl_data.fota_wait_time = PP_BL_DATA_FOTA_WAIT_TIME;
    pp_write_bl_data();
    _sys.restart4fota_flag = 1;
    return 0;
}
static int _cmd_stop(void) {
    if(_sys.stop_status == 1 || _sys.mos_lock_chg == 1 || _sys.mos_lock_dsg == 1) {
        LOG_E("stage err");
        return PP_HOST_ERR_CMD_STAGE;
    }
    _sys.stop_flag = 1;
    return 0;
}
static int _cmd_wkup(void) {
    if(_sys.stop_status == 0) {
        LOG_E("stage err");
        return PP_HOST_ERR_CMD_STAGE;
    }
    _sys.wkup_flag = 1;
    return 0;
}
static int _cmd_shutdown(void) {
    _sys.shutdown_flag = 1;
    return 0;
}
static int _cmd_restore_afe(void) {
    _sys.restore_afe_flag = 1;
    return 0;
}
static int _cmd_restore_sox(void) {
    _sys.restore_sox_flag = 1;
    return 0;
}
static int _cmd_store(void) {
    _sys.store_flag = 1;
    return 0;
}

#ifdef RT_USING_MSH
static int pp_restart(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_restart();
}
MSH_CMD_EXPORT(pp_restart, example: pp_restart);
static int pp_restart4fota(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_restart4fota();
}
MSH_CMD_EXPORT(pp_restart4fota, example: pp_restart4fota);
static int pp_stop(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_stop();
}
MSH_CMD_EXPORT(pp_stop, example: pp_stop);
static int pp_wkup(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_wkup();
}
MSH_CMD_EXPORT(pp_wkup, example: pp_wkup);
static int pp_shutdown(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_shutdown();
}
MSH_CMD_EXPORT(pp_shutdown, example: pp_shutdown);
static int pp_restore_afe(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_restore_afe();
}
MSH_CMD_EXPORT(pp_restore_afe, example: pp_restore_afe);
static int pp_restore_sox(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_restore_sox();
}
MSH_CMD_EXPORT(pp_restore_sox, example: pp_restore_sox);
static int pp_store(int argc, char *argv[]) {
    if(argc != 1) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_store();
}
MSH_CMD_EXPORT(pp_store, example: pp_store);
#endif

static int32_t _sys_hostcmd(void * args) {
    switch (_hostcmd.index) {
        case  2:  return _cmd_restart()                                 ;break;
        case  3:  return _cmd_restart4fota()                            ;break;
        case  4:  return _cmd_stop()                                    ;break;
        case  5:  return _cmd_wkup()                                    ;break;
        case  6:  return _cmd_shutdown()                                ;break;
        case  7:  return _cmd_restore_afe()                             ;break;
        case  8:  return _cmd_restore_sox()                             ;break;
        case 10:  return _cmd_store()                                   ;break;
        default:  return PP_HOST_ERR_CMD_INDEX;
    }
}
PP_HOSTCMD_EXPORT_F(_sys_hostcmd);
