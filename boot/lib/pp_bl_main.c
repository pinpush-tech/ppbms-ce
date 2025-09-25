#include "pp_cfg.h"
#include "pp_bl/pp_bl_data.h"
#include "pp_bl_sota.h"
#include "pp_drv_core.h"
#include "stdio.h"
#include "pp_bl_host.h"
#include "pp_drv_gpio.h"
#include "pp_drv_uart.h"
#include "pp_drv_timer.h"
#include "pp_bl_timer.h"
#include "pp_drv_delay.h"
#include "pp_mcu/pp_mcu_wdg.h"
#include "pp_bl_gpio.h"
#include "stdio.h"

static void _core_jump_cb(void) {
    pp_drv_delay_ms(PP_PCB_DELAY_WRITE_COMPLETE);
    pp_bl_data_deinit();
    pp_bl_host_deinit();
    pp_bl_gpio_deinit();
}

int main(void) {
    pp_mcu_wdg_init();
    pp_bl_data_init();
    if(pp_bl_data.comm_cfg_flag != 1) {
        pp_core_jump(NULL);
        return 0;
    }

    pp_bl_host_init();
    pp_bl_gpio_init();

    if(pp_bl_data.sota_flag == 1) {
        if(pp_bl_sota() < 0) printf("Error: sota err\r\n");
        pp_core_jump(_core_jump_cb);
        return 0;
    }
    while(pp_bl_host_fota_state == PP_BL_HOST_STATE_WAITING || pp_bl_host_fota_state == PP_BL_HOST_STATE_FOTAING) {
        pp_drv_delay_ms(5);
        pp_bl_host_process();
        pp_mcu_wdg_keepalive();
    }
    if (pp_bl_host_fota_state == PP_BL_HOST_STATE_FOTADONE) {
        pp_drv_delay_ms(PP_PCB_DELAY_HOST_COMPLETE);
    }
    pp_core_jump(_core_jump_cb);
}
