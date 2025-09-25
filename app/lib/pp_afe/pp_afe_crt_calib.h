#pragma once

#include "stdint.h"

int32_t MV2MA(double mv);

uint32_t MV2MA_DSGTH(double mv);
uint32_t MV2MA_CHGTH(double mv);

double MA2MV_DSGTH(uint32_t ma);
double MA2MV_CHGTH(uint32_t ma);
