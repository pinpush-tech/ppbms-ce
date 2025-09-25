#include "pp_bl/pp_bl_set_data.h"
#include "pp_data/pp_data_custset.h"
#include "pp_data/pp_data_sys.h"
#include "pp_persist/pp_persist.h"

static int _bl_init(void) {
    if (_custset.bootcomm == 1) { // 串口
        pp_bl_data.dev_type = 0;
        pp_bl_set_user_data("usart1");
    }

    return 0;
}
INIT_ENV_EXPORT(_bl_init);
