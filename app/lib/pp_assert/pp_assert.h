#include "pp_cfg.h"

#ifdef PP_PCB_CTX_APP
#include "stdio.h"
#include "rtthread.h"
#include "rtdebug.h"
#define PP_ASSERT(EX) RT_ASSERT(EX)
#else
#ifdef PP_PCB_CTX_DEBUG
#include "stdio.h"
void pp_assert(const char *ex_string, const char *func, int32_t line);
#define PP_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    pp_assert(#EX, __FUNCTION__, __LINE__);                           \
}
#else
#define PP_ASSERT(EX)
#endif
#endif