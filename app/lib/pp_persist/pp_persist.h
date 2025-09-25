#pragma once

#include "rtthread.h"
#include "pp_serialize/pp_serialize.h"
#include "pp_bl/pp_bl_data.h"

typedef void (* pp_persist_iter_log_cb_t)(int64_t ts, uint8_t * buffer, uint8_t length);

void pp_read_env(const pp_model_t * model, char *name);
void pp_store_env(const pp_model_t * model, char *name);
void pp_del_env(char *name);
void pp_iter_log(pp_persist_iter_log_cb_t cb, uint16_t page_num);
void pp_store_log(char * log);

void pp_write_dl_data(rt_uint32_t offset, uint8_t * buffer, rt_uint32_t size);
void pp_write_bl_data(void);
void pp_read_bn_data(uint8_t * buffer, rt_uint32_t size);
