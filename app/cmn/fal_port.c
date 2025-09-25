#include <fal.h>
#include "pp_mcu/pp_mcu_flash.h"
#include "board.h"
#include "pp_cfg.h"

const struct fal_flash_dev onchip_flash = {
    .name       = "onchip",
    .addr       = PP_PCB_MCU_FLASH_START_ADRESS,
    .len        = PP_PCB_MCU_FLASH_SIZE,
    .blk_size   = PP_PCB_MCU_FLASH_BLK_SIZE,
    .ops        = {pp_mcu_flash_init, pp_mcu_flash_read, pp_mcu_flash_write, pp_mcu_flash_erase},//操作，初始化、读、写、擦除
    .write_gran = 32
};
