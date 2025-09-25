#include "pp_cfg.h"

void pp_mcu_restart(void) {
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}
