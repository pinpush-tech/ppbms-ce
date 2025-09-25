#include "pp_cfg.h"
#include "rtthread.h"
#include "pp_assert/pp_assert.h"
#include "pp_host/pp_host_errcode.h"
#include "pp_data/pp_data_hostcmd.h"
#include "ulog.h"
#include "pp_data/pp_data_sys.h"
#include "rtdevice.h"
#include "pp_export/pp_export_f.h"
#include <sys/time.h>

/* 定时器 1 超时函数 */
static void timeout1(void *parameter) {
    _sys.timestamp++;
}

static int _clock_init(void) {
    rt_timer_t timer1;
    /* 创建定时器 1  周期定时器 */
    timer1 = rt_timer_create("pp_clock", timeout1,
                            RT_NULL, 1,
                            RT_TIMER_FLAG_PERIODIC);

    PP_ASSERT(timer1 != RT_NULL);
    rt_err_t ret = rt_timer_start(timer1);
    PP_ASSERT(ret == RT_EOK);
    return 0;
}
INIT_ENV_EXPORT(_clock_init);

static int _clock_stime(uint64_t timestamp) {
    _sys.timestamp = timestamp;
#ifdef RT_USING_RTC
    time_t t = timestamp / 1000;
    if (stime(&t) != 0)
        return -1;
#endif
    return 0;
}

static int _rtc_init(void) {
#ifdef RT_USING_RTC
    rt_device_t device = rt_device_find("rtc");
    PP_ASSERT(device != RT_NULL);
    rt_err_t ret = rt_device_open(device, 0);
    PP_ASSERT(ret == RT_EOK);
    uint64_t timestamp = (uint64_t) time(RT_NULL) * 1000;
    if (_sys.timestamp > timestamp) {
        int s_ret = _clock_stime(_sys.timestamp);
        PP_ASSERT(s_ret == 0);
    }
    else
        _sys.timestamp = timestamp;
#endif
    return 0;
}
INIT_ENV_EXPORT(_rtc_init);

static int _cmd_set_timestamp(int64_t timestamp_) {
    uint64_t timestamp = (uint64_t) timestamp_;
    if (_clock_stime(timestamp) != 0) {
        LOG_E("exec err");
        return PP_HOST_ERR_CMD_EXEC;
    }
    _sys.store_flag = 1;
    return 0;
}

#ifdef RT_USING_MSH
static int pp_set_timestamp(int argc, char *argv[]) {
    if(argc != 2) {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    char *endptr;
    PP_ASSERT(argv != RT_NULL);
    PP_ASSERT(argv[1] != RT_NULL);
    int64_t timestamp_ = strtoull(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        LOG_E("args err");
        return PP_HOST_ERR_CMD_ARGS;
    }
    return _cmd_set_timestamp(timestamp_);
}
MSH_CMD_EXPORT(pp_set_timestamp, example: pp_set_timestamp 1752999088952);
#endif

static int32_t _clock_hostcmd(void * args) {
    switch (_hostcmd.index) {
        case  9:  return _cmd_set_timestamp(_hostcmd.arg1)              ;break;
        default:  return PP_HOST_ERR_CMD_INDEX;
    }
}
PP_HOSTCMD_EXPORT_F(_clock_hostcmd);
