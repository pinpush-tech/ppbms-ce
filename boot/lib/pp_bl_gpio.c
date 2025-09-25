#include <stdint.h>
#include "pp_bl_gpio.h"
#include "pp_bl/pp_bl_data.h"
#include "pp_drv_gpio.h"

void pp_bl_gpio_init(void) {
    for(uint8_t i = 0; i < sizeof(pp_bl_data.gpio_ids); i++) {
        if(pp_bl_data.gpio_ids[i] == 0xff)  break;
        pp_drv_gpio_init(pp_bl_data.gpio_ids[i]);
        pp_drv_gpio_write(pp_bl_data.gpio_ids[i], pp_bl_data.gpio_level[i]);
    }
}

void pp_bl_gpio_deinit(void) {
    for(uint8_t i = 0; i < sizeof(pp_bl_data.gpio_ids); i++) {
        if(pp_bl_data.gpio_ids[i] == 0xff)  break;
        pp_drv_gpio_deinit(pp_bl_data.gpio_ids[i]);
    }
}