#include <stdint.h>
#include "ulog.h"
#include "pp_data/pp_data_sys.h"
#include "pp_host/pp_host.h"
#include "pp_export/pp_export_f.h"
#include "pp_assert/pp_assert.h"

static int _ulog_init(void) {
    if(_sys.log_swich) {
        int ret = ulog_init();
        PP_ASSERT(ret == 0);
        ulog_global_filter_lvl_set(_sys.log_level);
    } else {
        ulog_deinit();
    }
    return 0;
}
INIT_COMPONENT_EXPORT(_ulog_init);
