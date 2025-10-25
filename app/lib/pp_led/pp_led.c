#include "pp_cfg.h"
#include "rtthread.h"
#include "stdint.h"
#include "pp_export/pp_export_f.h"
#include "pp_data/pp_data_sys.h"

static int32_t _led_pulse(void * args) {
    static uint32_t times = 0;
    if (_sys.stop_status == 0) {
        if (times++ < 8 || times % 8 < 2)
            rt_pin_write(PP_PCB_LED_PIN, PIN_HIGH);
        else
            rt_pin_write(PP_PCB_LED_PIN, PIN_LOW);
    }
    return 0;
}
PP_PULSE0_EXPORT_F(_led_pulse);

static int _led_init(void) {
    if (_sys.stop_status == 0) {
        rt_pin_mode(PP_PCB_LED_PIN, PIN_MODE_OUTPUT);
    }
    return 0;
}
INIT_ENV_EXPORT(_led_init);