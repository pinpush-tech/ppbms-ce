#include "pp_continued/pp_continued.h"
#include "ulog.h"
#include "pp_assert/pp_assert.h"
#include "pp_data/pp_data_afe.h"

/**
 *  @brief      各类状态计数检测，如果满足条件会一直返回1，直到不满足为止
 *  @param      index           状态类别
 *  @param      condition       计数条件
 *  @param      count          目标计数值
 *  @return     是否达到目标计数值
*/
static uint8_t pp_continued0(int32_t * ms_p, uint8_t condition, int32_t ms, uint8_t reset) {
    PP_ASSERT(ms_p != RT_NULL);
    PP_ASSERT(condition == 0 || condition == 1);
    PP_ASSERT(ms >= 0);
    PP_ASSERT(reset == 0 || reset == 1);

    if(condition) {
        *ms_p += _afe.afe_times * _afe.afe_cyc_time;
        if(*ms_p  >= ms) {
            if(reset) {
                *ms_p = 0;
            } else {
                *ms_p = ms;
            }
            return 1;
        } else {
            return 0;
        }
    } else {
        *ms_p = 0;
        return 0;
    }
}

uint8_t pp_continued(int32_t * ms_p, uint8_t condition, int32_t ms) {
    PP_ASSERT(ms_p != RT_NULL);
    PP_ASSERT(condition == 0 || condition == 1);
    PP_ASSERT(ms >= 0);

    return pp_continued0(ms_p, condition, ms, 1);
}