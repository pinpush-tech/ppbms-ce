#include "rtthread.h"
#include "pp_log/pp_log.h"
#include "ulog.h"
#include "stdio.h"
#include "pp_cfg.h"
#include "pp_persist/pp_persist.h"
#include "pp_assert/pp_assert.h"

static void _log_(char id, char * fmt, va_list args) {
    PP_ASSERT(fmt != RT_NULL);
    char logbuff[PP_PCB_LOG_BUFFER_SIZE] = {0};
    uint8_t offset = snprintf(logbuff, PP_PCB_LOG_BUFFER_SIZE, "%c:", id);
    vsnprintf(logbuff + offset, PP_PCB_LOG_BUFFER_SIZE - offset, fmt, args);
    pp_store_log(logbuff);
    LOG_E(logbuff);
}
void pp_log_F_afe(char *fmt, ...) {
    PP_ASSERT(fmt != RT_NULL);
    va_list args;
    va_start(args, fmt);
    _log_('a', fmt, args);
    va_end(args);
}
void pp_log_F_sox(char *fmt, ...) {
    PP_ASSERT(fmt != RT_NULL);
    va_list args;
    va_start(args, fmt);
    _log_('s', fmt, args);
    va_end(args);
}
void pp_log_F_mcu(char *fmt, ...) {
    PP_ASSERT(fmt != RT_NULL);
    va_list args;
    va_start(args, fmt);
    _log_('m', fmt, args);
    va_end(args);
}
