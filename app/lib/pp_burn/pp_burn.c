#include <string.h>
#include <stdio.h>
#include "rtthread.h"
#include "pp_assert/pp_assert.h"
#include "pp_cfg.h"
#include "pp_data/pp_data_burn.h"
#include "pp_persist/pp_persist.h"
#include "pp_data/pp_data_sys.h"

static int _burn_init(void) {
    pp_read_bn_data((uint8_t *)&_burn, sizeof(_burn));
    if (_burn.hard_version[0] == 0xFF) {
        const char * str = "0.0.0.0";
        PP_ASSERT(sizeof(_burn.hard_version) > strlen(str));
        strcpy(_burn.hard_version, str);
    }
    if (_burn.tmp_bat_id == 0xFFFFFFFF) {
        _burn.tmp_bat_id = 0x00000000;
    }
    return 0;
}
INIT_DEVICE_EXPORT(_burn_init);
