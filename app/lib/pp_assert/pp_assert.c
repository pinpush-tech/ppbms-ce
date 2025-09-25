#include "pp_cfg.h"

#ifdef PP_PCB_CTX_DEBUG
#include "stdint.h"
#include <stdio.h>

void pp_assert(const char *ex, const char *func, int32_t line) {
    printf("ASSERT: (%s) at %s:%d\r\n", ex, func, line);
    while (1);
}
#endif
