#include "pp_cfg.h"
#include "stdint.h"

int pp_mcu_wdg_init(void) {
#ifndef PP_PCB_CTX_DEBUG
    static __IO uint32_t LsiFreq     = 40000;
    uint16_t reload_value = 3000;
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV32);
    reload_value = ((uint32_t)reload_value * LsiFreq / 1000) / 32;
    IWDG_CntReload(reload_value);
    IWDG_WriteConfig(IWDG_WRITE_DISABLE);

    RCC_EnableLsi(ENABLE);
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRD) == RESET) ;
#elif defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    while (RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_LSIRD) == RESET) ;
#endif
    IWDG_ReloadKey();
    IWDG_Enable();
#endif
    return 0;
}

void pp_mcu_wdg_keepalive(void) {
#ifndef PP_PCB_CTX_DEBUG
    IWDG_ReloadKey();
#endif
}
