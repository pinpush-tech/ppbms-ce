#include "pp_pcb_cfg.h"
#include "pp_assert.h"
#include "pp_drv_core.h"
#include "stdio.h"
#pragma clang optimize off
typedef void(*pc_t)(void);
static pc_t pc;

static void set_sp(uint32_t addr) {
    PP_ASSERT(addr >= SRAM_BASE);
    __ASM("MSR MSP, r0");
    __ASM("BX r14");
}

void pp_core_jump(pp_drv_core_cb_t cb) {
    uint32_t sp = *(uint32_t *)(PP_PCB_MCU_FLASH_START_ADRESS + PP_PCB_MCU_FLASH_APP_OFFSET);
    printf("Launch the App\r\n");
    /* 反初始化回调 */
    if(cb != NULL) {
        cb();
    }
    /* 关闭全局中断 */
    __set_PRIMASK(1);
    /* 跳转 */
    pc =(pc_t)(*(uint32_t *)(PP_PCB_MCU_FLASH_START_ADRESS + PP_PCB_MCU_FLASH_APP_OFFSET + 4));
    PP_ASSERT(pc != NULL);
    set_sp(sp);
    pc();
}
#pragma clang optimize on
