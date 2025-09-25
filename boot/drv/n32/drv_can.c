#include "pp_pcb_cfg.h"
#include "pp_drv_can.h"
#include "pp_drv_delay.h"
#include "pp_assert.h"

#include <stdio.h>
#include <string.h>

typedef struct n32_can {
    char *              name;
    CAN_Module *        CANx;
    CAN_InitType        can_init;
    CAN_FilterInitType  FilterConfig;
    // @changed
    uint32_t            RCC_APB1Periph;
    uint32_t            RCC_APB2Periph;
    GPIO_Module *       GPIOx_TX;
    GPIO_InitType       GPIO_InitStruct_TX;
    GPIO_Module *       GPIOx_RX;
    GPIO_InitType       GPIO_InitStruct_RX;
    uint8_t             GPIO_ConfigPinRemap;
    uint32_t            RmpPin;
    IRQn_Type           IRQn;
} n32_can_t;

static n32_can_t * _n32_can;
static pp_drv_can_rx_cb_t _can_cb;

static void CAN_NVIC_Config(IRQn_Type IRQn, FunctionalState cmd) {
    PP_ASSERT(cmd == ENABLE || cmd == DISABLE);
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel    = IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = cmd;
    if (cmd) {
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    }
    NVIC_Init(&NVIC_InitStructure);
}

void pp_drv_can_init(uint32_t * user_data, pp_drv_can_rx_cb_t can_cb) {
    PP_ASSERT(user_data != NULL);
    PP_ASSERT(can_cb != NULL);

    _n32_can = (n32_can_t *) user_data;
    _can_cb = can_cb;

    PP_ASSERT(_n32_can->CANx != NULL);
    PP_ASSERT(_n32_can->GPIOx_TX != NULL);
    PP_ASSERT(_n32_can->GPIOx_RX != NULL);
    RCC_EnableAPB2PeriphClk(_n32_can->RCC_APB2Periph, ENABLE);
    RCC_EnableAPB1PeriphClk(_n32_can->RCC_APB1Periph, ENABLE);
#if defined(BSP_USING_CAN1) || defined(BSP_USING_CAN2)
    if (_n32_can->GPIO_ConfigPinRemap == 1) GPIO_ConfigPinRemap(_n32_can->RmpPin, ENABLE);
#endif
    GPIO_InitPeripheral(_n32_can->GPIOx_TX, &_n32_can->GPIO_InitStruct_TX);
    GPIO_InitPeripheral(_n32_can->GPIOx_RX, &_n32_can->GPIO_InitStruct_RX);
    CAN_DeInit(_n32_can->CANx);
    uint8_t init_status = CAN_Init(_n32_can->CANx, &_n32_can->can_init);
    PP_ASSERT(init_status == CAN_InitSTS_Success);
#ifdef BSP_USING_CAN1
    if (_n32_can->CANx == CAN1) CAN1_InitFilter(&_n32_can->FilterConfig);
#endif
#ifdef BSP_USING_CAN2
    if (_n32_can->CANx == CAN2) CAN2_InitFilter(&_n32_can->FilterConfig);
#endif
#ifdef BSP_USING_CAN
    if (_n32_can->CANx == CAN) CAN_InitFilter(&_n32_can->FilterConfig);
#endif
    CAN_NVIC_Config(_n32_can->IRQn, ENABLE);
    CAN_INTConfig(_n32_can->CANx, CAN_INT_FMP0, ENABLE);
}

void pp_drv_can_deinit(void) {
    PP_ASSERT(_n32_can != NULL);
    PP_ASSERT(_n32_can->CANx != NULL);
    PP_ASSERT(_n32_can->GPIOx_TX != NULL);
    PP_ASSERT(_n32_can->GPIOx_RX != NULL);

    CAN_INTConfig(_n32_can->CANx, CAN_INT_FMP0, DISABLE);
    CAN_NVIC_Config(_n32_can->IRQn, DISABLE);
    CAN_DeInit(_n32_can->CANx);
#if defined(BSP_USING_CAN1) || defined(BSP_USING_CAN2)
    if (_n32_can->GPIO_ConfigPinRemap == 1) GPIO_ConfigPinRemap(_n32_can->RmpPin, DISABLE);
#endif
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStructure = _n32_can->GPIO_InitStruct_TX;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
#endif
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
    GPIO_InitPeripheral(_n32_can->GPIOx_TX, &GPIO_InitStructure);
    GPIO_InitStructure = _n32_can->GPIO_InitStruct_RX;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
#endif
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
    GPIO_InitPeripheral(_n32_can->GPIOx_RX, &GPIO_InitStructure);
    RCC_EnableAPB1PeriphClk(_n32_can->RCC_APB1Periph, DISABLE);
    RCC_EnableAPB2PeriphClk(_n32_can->RCC_APB2Periph, DISABLE);
}

void pp_drv_can_tx(uint8_t * data, uint16_t len) {
    PP_ASSERT(_n32_can != NULL);
    PP_ASSERT(data != NULL);

    CanTxMessage msg = {0};
    msg.StdId = 0x7FE;
    msg.IDE = CAN_ID_STD;
    msg.RTR = CAN_RTRQ_DATA;

    uint16_t remaining_len = len;
    while (remaining_len > 0) {
        msg.DLC = remaining_len >= 8 ? 8 : remaining_len;
        memcpy(msg.Data, data, msg.DLC);

        uint8_t can_mb = CAN_TransmitMessage(_n32_can->CANx, &msg);
        uint16_t time_out = 0xFFFF;
        while (CAN_TransmitSTS(_n32_can->CANx, can_mb) != CANTXSTSOK && time_out != 0) time_out--;
        PP_ASSERT(time_out != 0);
        data += msg.DLC;
        remaining_len -= msg.DLC;
    }
}

static void RX0_IRQHandler(void) {
    PP_ASSERT(_n32_can != NULL);
    PP_ASSERT(_can_cb != NULL);

    CanRxMessage msg = {0};
    CAN_ReceiveMessage(_n32_can->CANx, CAN_FIFO0, &msg);
    if (msg.StdId == 0x7FF)
        for (int i = 0; i < msg.DLC; i++)
            _can_cb(msg.Data[i]);
}

#ifdef BSP_USING_CAN1
void USB_LP_CAN1_RX0_IRQHandler(void) {
    RX0_IRQHandler();
}
#endif

#ifdef BSP_USING_CAN2
void CAN2_RX0_IRQHandler(void) {
    RX0_IRQHandler();
}
#endif

#ifdef BSP_USING_CAN
void CAN_RX0_IRQHandler(void) {
    RX0_IRQHandler();
}
#endif
