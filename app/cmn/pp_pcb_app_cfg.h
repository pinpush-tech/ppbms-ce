#pragma once

#include "pp_pcb_cmn_cfg.h"
#include "rtdevice.h"

#define PP_PCB_MCU_I2C_DELAY_US                     (2)

#define PP_PCB_AFE_CRT_RV                       0.001
#define PP_PCB_AFE_CRT_CALIB_WEIGHT             1
#define PP_PCB_AFE_CRT_CALIB_BIAS               0
#define PP_PCB_AFE_CHGM_FAIL_DLY                (10 * 1000)
#define PP_PCB_AFE_DSGM_FAIL_DLY                (10 * 1000)
#define PP_PCB_AFE_AFE_FAIL_DLY                 (10 * 1000)

#define PP_PCB_NTC_25_RV                        10.0
#define PP_PCB_NTC_B_VALUE                      3435.0
#define PP_PCB_NTC_RT_M_VALUE                   0.0015
#define PP_PCB_NTC_TR_M_VALUE                   0.00054

#define PP_PCB_HOST_RECV_BUF_SIZE               (4 * 1024)

#define PP_PCB_SYS_STOP_TIME                    (61 * 60 * 1000)

#define PP_PCB_USING_AFE_CB
#define PP_PCB_USING_AFE_LOAD_DET
#define PP_PCB_USING_AFE_SEPARATE_PORT
#define PP_PCB_USING_AFE_MOT
#define PP_PCB_USING_AFE_INT_WK
#define PP_PCB_USING_AFE_DSGM_FAIL
#define PP_PCB_USING_AFE_FUSE
#define PP_PCB_USING_AFE_PDSG_SCD
#define PP_PCB_USING_AFE_AUTO_MOS_CTRL
#define PP_PCB_USING_SYS_STOP
#define PP_PCB_USING_SYS_SHUTDOWN

#define PP_PCB_CTX_APP
