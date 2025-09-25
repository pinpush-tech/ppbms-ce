#pragma once

#include <rtdef.h>
#include "stdint.h"

typedef int32_t (*pp_fn_t)(void * args);

#define PP_EXPORT_F(fn, level) \
    RT_USED const pp_fn_t __pp_fn_##fn RT_SECTION(".pp_fn."#level) = fn

#define PP_PREV0_EXPORT_F(fn) PP_EXPORT_F(fn, 00_0_b)
#define PP_PREV1_EXPORT_F(fn) PP_EXPORT_F(fn, 00_1_b)
#define PP_PREV2_EXPORT_F(fn) PP_EXPORT_F(fn, 00_2_b)

#define PP_PULSE_EXPORT_F(fn) PP_EXPORT_F(fn, 01_b)

#define PP_APP0_EXPORT_F(fn) PP_EXPORT_F(fn, 02_0_b)
#define PP_APP1_EXPORT_F(fn) PP_EXPORT_F(fn, 02_1_b)
#define PP_APP2_EXPORT_F(fn) PP_EXPORT_F(fn, 02_2_b)

#define PP_DEINIT_EXPORT_F(fn) PP_EXPORT_F(fn, 03_b)

#define PP_HOSTPRE_EXPORT_F(fn) PP_EXPORT_F(fn, 04_0_b)
#define PP_HOSTPOST_EXPORT_F(fn) PP_EXPORT_F(fn, 04_1_b)

#define PP_HOSTCMD_EXPORT_F(fn) PP_EXPORT_F(fn, 05_b)

#define PP_HOSTOPR_EXPORT_F(fn) PP_EXPORT_F(fn, 06_b)

int32_t pp_export_f_pulse(void * args);
int32_t pp_export_f_deinit(void * args);
int32_t pp_export_f_hostpre(void * args);
int32_t pp_export_f_hostpost(void * args);
int32_t pp_export_f_hostcmd(void * args);
int32_t pp_export_f_hostopr(void * args);
