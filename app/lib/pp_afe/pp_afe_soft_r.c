#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_sys.h"
#include "pp_afe/pp_afe_chip.h"
#include "pp_afe/pp_afe.h"

void pp_afe_soft_init(void) {
    if (_sys.mos_lock_chg) {
        pp_afe_mos_ctl(-1, 0, -1, -2);
    }
    if (_sys.mos_lock_dsg) {
        pp_afe_mos_ctl(0, -1, -1, -2);
    }
}
