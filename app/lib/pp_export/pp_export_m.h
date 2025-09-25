#pragma once

#include <rtdef.h>
#include "stdint.h"
#include "cson.h"
#include "pp_serialize/pp_serialize.h"

typedef struct {
	const pp_model_t *model_array;
	uint8_t num;
} pp_models_t;

#define PP_EXPORT_M(nm, cm, dt, tp, level) \
    RT_USED const pp_model_t __pp_md_##nm RT_SECTION(".pp_vr."#level) \
        = {.cson_model = cm, .model_size = sizeof(cm)/sizeof(CsonModel), .struct_address = &dt, .struct_size = sizeof(tp)};

#define PP_ENV_EXPORT_M(id, cm, dt, tp) \
    PP_EXPORT_M(01_b_##id, cm, dt, tp, 01_b_#id)

#define PP_HOST_EXPORT_M(id, cm, dt, tp) \
    PP_EXPORT_M(02_b_##id, cm, dt, tp, 02_b_#id)

#define PP_HOSTCMD_EXPORT_M(id, cm, dt, tp) \
    PP_EXPORT_M(03_b_##id, cm, dt, tp, 03_b_#id)

extern pp_models_t _env_models;
extern pp_models_t _host_models;
extern pp_models_t _hostcmd_models;