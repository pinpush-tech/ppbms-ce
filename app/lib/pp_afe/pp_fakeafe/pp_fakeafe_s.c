#include "stdint.h"
#include "pp_data/pp_data_afe.h"

void pp_afe_chip_set4flag(void) {
    // 将寄存器中flag相关的原始数据，更新到_afe中
    _afe.afe_has_data = 1;
}

void pp_afe_chip_set4data(void) {
    // 将寄存器中data相关的原始数据，更新到_afe中
    int32_t bat_vol = 0;
    for(int i = 0; i < PP_BAT_CELL_COUNT; i++) {
        *(&_afe.cell_vol_1 + i) = 3000;
        bat_vol += *(&_afe.cell_vol_1 + i);
    }
    _afe.bat_vol = bat_vol;
    _afe.cell_max_vol = 3000;
    _afe.cell_min_vol = 3000;
    _afe.maxv_cell_ch = 1;
    _afe.minv_cell_ch = 1;

    _afe.chip_temp = 25;
    _afe.cell0_temp = 25;
    _afe.cell1_temp = 25;
    _afe.mos_temp   = 25;
    _afe.cell_min_temp = 25;
    _afe.cell_max_temp = 25;
    _afe.cell_avg_temp = 25;
}