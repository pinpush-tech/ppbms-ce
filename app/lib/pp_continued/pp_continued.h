#pragma once

#include "stdint.h"

#define PP_CONTINUED(c, m) ({ \
    static int32_t __pp_continued_m_##__COUNTER__ = 0; \
    pp_continued(&__pp_continued_m_##__COUNTER__, (c), (m)); \
})

uint8_t pp_continued(int32_t * count_p, uint8_t condition, int32_t count);