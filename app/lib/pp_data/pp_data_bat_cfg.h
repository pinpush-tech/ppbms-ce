#pragma once

#if !defined(PP_BAT_CELL_COUNT)
#error "You must defined"
#endif
#if !defined(PP_BAT_FMAH)
#error "You must defined"
#endif

#if !defined(PP_BAT_MAX_CYC)
#error "You must defined"
#endif
#if !defined(PP_BAT_OV1_TH)
#error "You must defined"
#endif
#if !defined(PP_BAT_UV1_TH)
#error "You must defined"
#endif

#define PP_BAT_TF_CURVE_TMPR    {-20, -10, 5, 25, 45}
#define PP_BAT_TF_CURVE_FMAH    {90.0, 92.0, 94.0, 100.0, 103.0}

#define PP_BAT_UTC_TH      0
#define PP_BAT_UTD_TH      -20
#define PP_BAT_UT_DLY      4000

#define PP_BAT_OTC_TH      50
#define PP_BAT_OTD_TH      60
#define PP_BAT_OT_DLY      4000

#define PP_BAT_OCC1_TH     (1 * PP_BAT_FMAH)
#define PP_BAT_OCC1_DLY    2000

#define PP_BAT_OCC2_TH     (1.5 * PP_BAT_FMAH)
#define PP_BAT_OCC2_DLY    40

#define PP_BAT_OCD1_TH     (2 * PP_BAT_FMAH)
#define PP_BAT_OCD1_DLY    2000

#define PP_BAT_OCD2_TH     (3 * PP_BAT_FMAH)
#define PP_BAT_OCD2_DLY    100

#define PP_BAT_EOC_CRRT    (0.02 * PP_BAT_FMAH)
#define PP_BAT_EOC_TH      (PP_BAT_OV1_TH - 100)
#define PP_BAT_EOCR_HYS    200
#define PP_BAT_EOC_DLY     4000

#define PP_BAT_CB1_TH      (PP_BAT_UV1_TH + (PP_BAT_OV1_TH - PP_BAT_UV1_TH) * 0.7)
#define PP_BAT_CB1_DIFF    60

#define PP_BAT_MOT_TH      80
#define PP_BAT_MOTR_HYS    5
#define PP_BAT_MOT_DLY     2000

#define PP_BAT_SCD_TH      (100000 + 5 * PP_BAT_FMAH)
#define PP_BAT_SCD_DLY     233 //us

#define PP_BAT_UVR_HYS     ((PP_BAT_OV1_TH - PP_BAT_UV1_TH) * 0.15)
#define PP_BAT_UV1_DLY      2000

#define PP_BAT_OVR_HYS     ((PP_BAT_OV1_TH - PP_BAT_UV1_TH) * 0.15)
#define PP_BAT_OV1_DLY      2000

#define PP_BAT_OV2_TH      (PP_BAT_OV1_TH + 200)
#define PP_BAT_OV2_DLY     4000

#define PP_BAT_UV2_TH       (PP_BAT_UV1_TH - 300)
#define PP_BAT_UV2_DLY      4000

#define PP_BAT_VDIF_TH     ((PP_BAT_OV1_TH - PP_BAT_UV1_TH) * 0.5)
#define PP_BAT_VDIF_HYS    300
#define PP_BAT_VDIF_DLY    5000

#define PP_BAT_ZERO_TH     29

#define PP_BAT_PDSG_TIME   2000
#define PP_BAT_PDSG_SCD_TH 30
