#include "pp_pcb_cfg.h"
#include "pp_drv_timer.h"
#include "pp_assert.h"

#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))

static pp_drv_timer_cb_t _timer_cb;

static void TIM_NVIC_Config(FunctionalState cmd) {
    PP_ASSERT(cmd == ENABLE || cmd == DISABLE);
    NVIC_InitType NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel    = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = cmd;
    if(cmd)
    {
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    }
    NVIC_Init(&NVIC_InitStructure);
}

void pp_drv_timer_init(pp_drv_timer_cb_t cb) {
    PP_ASSERT(cb != NULL);
    _timer_cb = cb;

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM6, ENABLE);

    RCC_ClocksType RCC_ClockFreq;
    RCC_GetClocksFreqValue(&RCC_ClockFreq);
    uint32_t clk = RCC_ClockFreq.Pclk1Freq;
    uint8_t clkpre = GET_BITS(RCC->CFG, 8, 10);
    if (clkpre >= 4) {
        clk = clk * 2;
    }
    uint32_t pre = (clk / 10000) - 1;

    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.Prescaler = pre;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period    = 10 - 1;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.RepetCnt  = 0;
    TIM_InitTimeBase(TIM6, &TIM_TimeBaseStructure);

    TIM_ConfigInt(TIM6, TIM_INT_UPDATE, ENABLE);
    TIM_Enable(TIM6, ENABLE);
    TIM_NVIC_Config(ENABLE);
}

void pp_drv_timer_deinit(void) {
    TIM_Enable(TIM6, DISABLE);
    TIM_ConfigInt(TIM6, TIM_INT_UPDATE, DISABLE);
    TIM_NVIC_Config(DISABLE);
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM6, DISABLE);
}

void TIM6_IRQHandler(void) {
    if (TIM_GetIntStatus(TIM6, TIM_INT_UPDATE) != RESET) {
        TIM_ClrIntPendingBit(TIM6, TIM_INT_UPDATE);
        PP_ASSERT(_timer_cb != NULL);
        _timer_cb();
    }
}
