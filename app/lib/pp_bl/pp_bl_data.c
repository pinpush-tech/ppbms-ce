#include "pp_bl/pp_bl_data.h"
#include "pp_mcu/pp_mcu_flash.h"
#include "pp_cfg.h"
#include "pp_assert/pp_assert.h"

pp_bl_data_t pp_bl_data = {
    .sota_flag = 0x00,
    .dl_offset = 0xFFFFFFFF,
    .app_size = 0xFFFFFFFF,
    .comm_cfg_flag = 1,
    .fota_wait_time = 0,
    .gpio_ids = {0xFF, 0xFF, 0xFF, 0xFF},
    .rs_485_mode_gpio_id = 0xFF
};

#ifdef PP_PCB_CTX_APP
#include "pp_persist/pp_persist.h"
#include "pp_export/pp_export_f.h"

static int32_t _bl_data_write(void * args) {
    pp_write_bl_data();
    return 0;
}
PP_APP2_EXPORT_F(_bl_data_write);
#else
void pp_bl_data_init(void) {
    PP_ASSERT(sizeof(pp_bl_data) <= PP_PCB_MCU_FLASH_BLDATA_SIZE);
    int32_t len = pp_mcu_flash_read(PP_PCB_MCU_FLASH_BLDATA_OFFSET, (uint8_t *)&pp_bl_data, sizeof(pp_bl_data));
    PP_ASSERT(len == sizeof(pp_bl_data));
}

void pp_bl_data_deinit(void) {
    if (pp_bl_data.sota_flag == 0 && pp_bl_data.fota_wait_time == PP_BL_DATA_FOTA_WAIT_TIME) return;
    int32_t len = pp_mcu_flash_erase(PP_PCB_MCU_FLASH_BLDATA_OFFSET, PP_PCB_MCU_FLASH_BLDATA_SIZE);
    PP_ASSERT(len == PP_PCB_MCU_FLASH_BLDATA_SIZE);
    pp_bl_data.sota_flag = 0;
    pp_bl_data.fota_wait_time = PP_BL_DATA_FOTA_WAIT_TIME;
    len = pp_mcu_flash_write(PP_PCB_MCU_FLASH_BLDATA_OFFSET, (uint8_t *)&pp_bl_data, sizeof(pp_bl_data));
    PP_ASSERT(len == sizeof(pp_bl_data));
}
#endif
