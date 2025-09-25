#include "pp_cfg.h"
#include "pp_bl_sota.h"
#include "pp_mcu/pp_mcu_flash.h"
#include "pp_bl/pp_bl_data.h"

int32_t pp_bl_sota(void) {
    int32_t state = pp_mcu_flash_erase(PP_PCB_MCU_FLASH_APP_OFFSET, pp_bl_data.app_size);
    if(state < 0)   return state;
    state = pp_mcu_flash_write(PP_PCB_MCU_FLASH_APP_OFFSET, (uint8_t *)(pp_bl_data.dl_offset + PP_PCB_MCU_FLASH_START_ADRESS), pp_bl_data.app_size);
    return state;
}
