#include "pp_pcb_cfg.h"
#include "stdio.h"
#include "pp_assert.h"

typedef struct {
    void * port;
    uint16_t pin;
} _gpio_t;

static _gpio_t get_pin_form_id(uint8_t gpio_id) {
    _gpio_t gpio = {0};
    PP_ASSERT(gpio_id < (16 * 7));
    uint8_t port_number = gpio_id / 16;
    gpio.pin = 1 << (gpio_id % 16);
    switch(port_number) {
        case 0:     gpio.port = GPIOA;     break;
        case 1:     gpio.port = GPIOB;     break;
        case 2:     gpio.port = GPIOC;     break;
        case 3:     gpio.port = GPIOD;     break;
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
        case 4:     gpio.port = GPIOE;     break;
#endif
#ifdef SOC_N32G45X
        case 5:     gpio.port = GPIOF;     break;
        case 6:     gpio.port = GPIOG;     break;
#endif /* SOC_N32G45X */
    }
    return gpio;
}

static void _gpio_clock_enable(GPIO_Module* gpio_port, FunctionalState cmd) {
    PP_ASSERT(gpio_port != NULL);
    PP_ASSERT(cmd == ENABLE || cmd == DISABLE);

	if(gpio_port == GPIOA) {
		RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, cmd);
	}
    if (gpio_port == GPIOB) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, cmd);
    }
    if (gpio_port == GPIOC) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, cmd);
    }
    if (gpio_port == GPIOD) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, cmd);
    }
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    if (gpio_port == GPIOE) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOE, cmd);
    }
#endif
#ifdef SOC_N32G45X
    if (gpio_port == GPIOF) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOF, cmd);
    }
    if (gpio_port == GPIOG) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOG, cmd);
    }
#endif
}

void pp_drv_gpio_init(uint8_t gpio_id) {
    _gpio_t gpio = get_pin_form_id(gpio_id);
    PP_ASSERT(gpio.port != NULL);

    _gpio_clock_enable(gpio.port, ENABLE);
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = gpio.pin;
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#endif
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(gpio.port, &GPIO_InitStructure);
}

void pp_drv_gpio_deinit(uint8_t gpio_id) {
    _gpio_t gpio = get_pin_form_id(gpio_id);
    PP_ASSERT(gpio.port != NULL);

    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = gpio.pin;
#if defined(SOC_N32L43X) || defined(SOC_N32L40X) || defined(SOC_N32G43X)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
#endif
#if defined(SOC_N32G45X) || defined(SOC_N32WB452)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
    GPIO_InitPeripheral(gpio.port, &GPIO_InitStructure);

    _gpio_clock_enable(gpio.port, DISABLE);
}

void pp_drv_gpio_write(uint8_t gpio_id, uint8_t level) {
    PP_ASSERT(level == 0 || level == 1);
    _gpio_t gpio = get_pin_form_id(gpio_id);
    PP_ASSERT(gpio.port != NULL);

    GPIO_WriteBit(gpio.port, gpio.pin, level);
}
