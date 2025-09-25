#pragma once

#include "cson.h"
#include <stdint.h>

typedef struct {
	CsonModel *cson_model;
	uint16_t model_size;
	void *struct_address;
	uint16_t struct_size;
} pp_model_t;

void pp_serialize_big(const pp_model_t * model, uint8_t * buffer_addr);
void pp_serialize(const pp_model_t * model, uint8_t * buffer_addr);
void pp_deserialize_big(uint8_t * buffer_addr ,const pp_model_t * model);
void pp_deserialize(uint8_t *buffer_addr , const pp_model_t *model);
uint16_t pp_serialize_cal_length(const pp_model_t *model);
