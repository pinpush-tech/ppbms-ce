#include "pp_pcb_cfg.h"
#include "pp_drv_uart.h"
#include "stdio.h"
#include "pp_assert.h"

typedef struct n32_uart {
    USART_Module* uart_periph;
    IRQn_Type     irqn;
    uint32_t      per_clk;
    uint32_t      tx_gpio_clk;
    uint32_t      rx_gpio_clk;
    GPIO_Module*  tx_port;
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_ModeType tx_af;
    uint16_t      tx_pin;
    GPIO_Module*  rx_port;
    GPIO_ModeType rx_af;
    uint16_t      rx_pin;
#elif defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    uint32_t      tx_af;
    uint16_t      tx_pin;
    GPIO_Module*  rx_port;
    uint32_t      rx_af;
    uint16_t      rx_pin;
    // @changed
    GPIO_PuPdType pp_tx_gpio_pull;
    GPIO_ModeType pp_tx_gpio_mode;
    GPIO_PuPdType pp_rx_gpio_pull;
    GPIO_ModeType pp_rx_gpio_mode;
#endif
    struct rt_ser1ial_device * serial;
    char *device_name;
    // @changed
    uint32_t RCC_APB2Periph;
    uint8_t RCC_EnableAPB1PeriphClk;
    uint32_t RCC_APB1Periph;
    uint8_t GPIO_ConfigPinRemap_SW_JTAG;
    uint8_t GPIO_ConfigPinRemap;
    uint32_t RmpPin;
    GPIO_InitType GPIO_InitStruct_TX;
    GPIO_InitType GPIO_InitStruct_RX;
    USART_InitType USART_InitStructure;
} n32_uart_t;

static pp_drv_uart_cb_t _uart_cb;
static n32_uart_t * _n32_uart;

static void USART_NVIC_Config(FunctionalState cmd) {
    PP_ASSERT(_n32_uart != NULL);
    PP_ASSERT(cmd == ENABLE || cmd == DISABLE);

    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel            = _n32_uart->irqn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = cmd;
    NVIC_Init(&NVIC_InitStructure);
}

void pp_drv_uart_init(uint32_t * user_data, pp_drv_uart_cb_t uart_cb) {
    PP_ASSERT(user_data != NULL);
    PP_ASSERT(uart_cb != NULL);

    _uart_cb = uart_cb;
    _n32_uart = (n32_uart_t *)user_data;

    PP_ASSERT(_n32_uart->uart_periph != NULL);
    PP_ASSERT(_n32_uart->tx_port != NULL);
    PP_ASSERT(_n32_uart->rx_port != NULL);

    RCC_EnableAPB2PeriphClk(_n32_uart->RCC_APB2Periph, ENABLE);
    if (_n32_uart->RCC_EnableAPB1PeriphClk == 1)     RCC_EnableAPB1PeriphClk(_n32_uart->RCC_APB1Periph, ENABLE);
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    if (_n32_uart->GPIO_ConfigPinRemap_SW_JTAG == 1) GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_DISABLE, ENABLE);
    if (_n32_uart->GPIO_ConfigPinRemap == 1)         GPIO_ConfigPinRemap(_n32_uart->RmpPin, ENABLE);
#endif
    GPIO_InitPeripheral(_n32_uart->tx_port, &_n32_uart->GPIO_InitStruct_TX);
    GPIO_InitPeripheral(_n32_uart->rx_port, &_n32_uart->GPIO_InitStruct_RX);        
    USART_Init(_n32_uart->uart_periph, &_n32_uart->USART_InitStructure);
    NVIC_SetPriority(_n32_uart->irqn, 0);
    NVIC_EnableIRQ(_n32_uart->irqn);
    while (USART_GetFlagStatus(_n32_uart->uart_periph, USART_FLAG_RXDNE) != RESET)
        USART_ReceiveData(_n32_uart->uart_periph);
    USART_NVIC_Config(ENABLE);
    USART_ConfigInt(_n32_uart->uart_periph, USART_INT_RXDNE, ENABLE);
    USART_Enable(_n32_uart->uart_periph, ENABLE);
}

void pp_drv_uart_deinit(void) {
    PP_ASSERT(_n32_uart != NULL);
    PP_ASSERT(_n32_uart->uart_periph != NULL);
    PP_ASSERT(_n32_uart->tx_port != NULL);
    PP_ASSERT(_n32_uart->rx_port != NULL);

    USART_Enable(_n32_uart->uart_periph, DISABLE);
    USART_ConfigInt(_n32_uart->uart_periph, USART_INT_RXDNE, DISABLE);
    USART_NVIC_Config(DISABLE);
    NVIC_DisableIRQ(_n32_uart->irqn);
    USART_DeInit(_n32_uart->uart_periph);
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStructure = _n32_uart->GPIO_InitStruct_TX;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
#endif
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
    GPIO_InitPeripheral(_n32_uart->tx_port, &GPIO_InitStructure);
    GPIO_InitStructure = _n32_uart->GPIO_InitStruct_RX;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
#endif
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
    GPIO_InitPeripheral(_n32_uart->rx_port, &GPIO_InitStructure);
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    if (_n32_uart->GPIO_ConfigPinRemap_SW_JTAG == 1) GPIO_ConfigPinRemap(GPIO_RMP_SW_JTAG_DISABLE, DISABLE);
    if (_n32_uart->GPIO_ConfigPinRemap == 1)         GPIO_ConfigPinRemap(_n32_uart->RmpPin, DISABLE);
#endif
    RCC_EnableAPB2PeriphClk(_n32_uart->RCC_APB2Periph, DISABLE);
    if (_n32_uart->RCC_EnableAPB1PeriphClk == 1)     RCC_EnableAPB1PeriphClk(_n32_uart->RCC_APB1Periph, DISABLE);
}

void pp_drv_uart_tx(uint8_t * data, uint16_t len) {
    PP_ASSERT(_n32_uart != NULL);
    PP_ASSERT(data != NULL);

    for (int i = 0; i < len; i++) {
        USART_SendData(_n32_uart->uart_periph, data[i]);
        while (USART_GetFlagStatus(_n32_uart->uart_periph, USART_FLAG_TXDE) == RESET);
    }
}

static void _UART_IRQHandler(void) {
    PP_ASSERT(_n32_uart != NULL);
    PP_ASSERT(_uart_cb != NULL);

	uint8_t data = 0;
    if (USART_GetIntStatus(_n32_uart->uart_periph, USART_INT_RXDNE) != RESET &&
        USART_GetFlagStatus(_n32_uart->uart_periph, USART_FLAG_RXDNE) != RESET) {
		data = USART_ReceiveData(_n32_uart->uart_periph);
        _uart_cb(data);
	}
}

void USART1_IRQHandler(void) {
    _UART_IRQHandler();
}
void USART2_IRQHandler(void) {
    _UART_IRQHandler();
}
void USART3_IRQHandler(void) {
    _UART_IRQHandler();
}
void UART4_IRQHandler(void) {
    _UART_IRQHandler();
}
void UART5_IRQHandler(void) {
    _UART_IRQHandler();
}
void UART6_IRQHandler(void) {
    _UART_IRQHandler();
}
void UART7_IRQHandler(void) {
    _UART_IRQHandler();
}
