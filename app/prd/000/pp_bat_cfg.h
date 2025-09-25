#pragma once

#define PP_BAT_CELL_COUNT               (8)
#define PP_BAT_OV1_TH                    (3650)
#define PP_BAT_UV1_TH                    (2500)
#define PP_BAT_FMAH                     (20000)
#define PP_BAT_MAX_CYC                  (1000)

#include "pp_data/pp_data_bat_cfg.h"

#undef PP_BAT_ZERO_TH
#define PP_BAT_ZERO_TH     150
