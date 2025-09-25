#include "pp_cfg.h"
#include "stdint.h"
#include "stdio.h"
#include <string.h>
#include "pp_mcu/pp_mcu_wdg.h"
#include "pp_assert/pp_assert.h"
#ifdef PP_PCB_CTX_APP
#include "ulog.h"
#endif

int pp_mcu_flash_init(void) {
    return 1;
}

int pp_mcu_flash_read(long offset, uint8_t *buf, uint32_t size) {
    PP_ASSERT(offset >= 0);
    PP_ASSERT(buf != NULL);

    uint32_t i;
    uint32_t addr = PP_PCB_MCU_FLASH_START_ADRESS + offset;
    for (i = 0; i < size; i++, addr++, buf++) {
        *buf = *(uint8_t *) addr;
    }
    return size;
}

int pp_mcu_flash_write(long offset, const uint8_t *buf, uint32_t size) {
    PP_ASSERT(offset >= 0);
    PP_ASSERT(buf != NULL);
    PP_ASSERT(offset % 4 == 0);

    uint32_t i;
    uint32_t addr = PP_PCB_MCU_FLASH_START_ADRESS + offset;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit()) {
        return -1;
    }
#endif
    pp_mcu_wdg_keepalive();
    FLASH_Unlock();
    for (i = 0; i < size; i += 4) {
        uint32_t data = 0;
        // 可能不足4个字节，但按4个字节写
        memcpy(&data, buf + i, sizeof(uint32_t));
        if (FLASH_COMPL != FLASH_ProgramWord(addr + i, data)) {
            pp_mcu_wdg_keepalive();
            FLASH_Lock();
            return -1;
        }else{
            pp_mcu_wdg_keepalive();
        }
    }
    FLASH_Lock();
    return size;
}

int pp_mcu_flash_erase(long offset, uint32_t size) {
    PP_ASSERT(offset >= 0);
    PP_ASSERT(offset % PP_PCB_MCU_FLASH_BLK_SIZE == 0);

    uint32_t addr = PP_PCB_MCU_FLASH_START_ADRESS + offset;

    uint32_t erase_pages, i;
    uint32_t PageAddress = 0;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit()) {
        return -1;
    }
#endif
    pp_mcu_wdg_keepalive();
    FLASH_Unlock();
    erase_pages = size / PP_PCB_MCU_FLASH_BLK_SIZE;
    if (size % PP_PCB_MCU_FLASH_BLK_SIZE != 0) {
        erase_pages++;
    }
    for (i = 0; i < erase_pages; i++) {
        PageAddress = addr + (PP_PCB_MCU_FLASH_BLK_SIZE * i);
        if (FLASH_COMPL != FLASH_EraseOnePage(PageAddress)) {
            pp_mcu_wdg_keepalive();
            FLASH_Lock();
            return -1;
        } else{
            pp_mcu_wdg_keepalive();
        }
    }
    FLASH_Lock();

#ifdef PP_PCB_CTX_APP
    LOG_I("### pp_flash_erase offset: %ld K, size: %d K", offset / 1024, size / 1024);
#endif
    return size;
}
