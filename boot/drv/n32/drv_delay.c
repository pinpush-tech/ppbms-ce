#include "pp_pcb_cfg.h"
#include "pp_assert.h"

static void _drv_delay_us(uint32_t nus) {
    PP_ASSERT(nus <= 1000);

    uint32_t temp;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    //select system clock
    SysTick->LOAD=nus*(SystemCoreClock/1000000); //time relode
    SysTick->VAL=0x00;        //clear timer value
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //Start countdown
    do {
        temp=SysTick->CTRL;
    } while(temp&0x01&&!(temp&(1<<16)));//wait for the time reach
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //close the count
    SysTick->VAL =0X00;       //clear timer value
}

void pp_drv_delay_ms(uint16_t nms) {
    for (; nms > 0; nms--) {
        _drv_delay_us(1000);
    }
}
