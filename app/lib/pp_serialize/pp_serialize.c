#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ulog.h"
#include "cson.h"
#include "pp_serialize/pp_serialize.h"
#include "pp_assert/pp_assert.h"

static uint16_t _cal_length(CsonModel *cson_model, uint16_t model_size) {
    PP_ASSERT(cson_model != RT_NULL);
    PP_ASSERT(model_size > 0);

    uint16_t length=0;
	for (uint8_t i = 1; i < model_size; i++) { // 从1开始跳过第一个模型，它是整个结构体对象

        switch(cson_model[i].type) {
			case CSON_TYPE_CHAR:    case CSON_TYPE_UCHAR:
				length += sizeof(int8_t);
				break;
			case CSON_TYPE_SHORT:   case CSON_TYPE_USHORT:
				length += sizeof(int16_t);
				break;
			case CSON_TYPE_INT:     case CSON_TYPE_UINT:    case CSON_TYPE_FLOAT:
				length += sizeof(int32_t);
				break;
			case CSON_TYPE_LONG:    case CSON_TYPE_ULONG:   case CSON_TYPE_DOUBLE:
				length += sizeof(int64_t);
				break;
            case CSON_TYPE_ARRAY:
				length = length + cson_model[i].param.array.size;
				break;
            case CSON_TYPE_STRUCT:
				length += _cal_length(cson_model[i].param.sub.model,cson_model[i].param.sub.size);
				break;
			default:
				LOG_E("err: Unsupported type");
                break;
		}
	}
	return length;
}

uint16_t pp_serialize_cal_length(const pp_model_t *model) {
    PP_ASSERT(model != RT_NULL);

	return _cal_length(model->cson_model, model->model_size);
}

static uint16_t _serialize_big(CsonModel *cson_model, uint16_t model_size, uint8_t *struct_address, uint8_t * buffer_addr) {
    PP_ASSERT(cson_model != RT_NULL);
    PP_ASSERT(model_size > 0);
    PP_ASSERT(struct_address != RT_NULL);
    PP_ASSERT(buffer_addr != RT_NULL);

    uint16_t binary_offset = 0;
    for (uint8_t i = 1; i < model_size; i++) { // 从1开始跳过第一个模型，它是整个结构体对象

		switch(cson_model[i].type) {
			case CSON_TYPE_UCHAR:   case CSON_TYPE_CHAR:
				memcpy(buffer_addr + binary_offset, (struct_address + cson_model[i].offset), sizeof(uint8_t));
                binary_offset += sizeof(uint8_t);
				break;
			case CSON_TYPE_USHORT:  case CSON_TYPE_SHORT:
				memcpy(buffer_addr + binary_offset, (struct_address + cson_model[i].offset + 1), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 1, (struct_address + cson_model[i].offset), sizeof(uint8_t));
                binary_offset += sizeof(uint16_t);
				break;
            case CSON_TYPE_UINT:    case CSON_TYPE_INT:     case CSON_TYPE_FLOAT:
				memcpy(buffer_addr + binary_offset, (struct_address + cson_model[i].offset + 3), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 1, (struct_address + cson_model[i].offset + 2), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 2, (struct_address + cson_model[i].offset + 1), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 3, (struct_address + cson_model[i].offset), sizeof(uint8_t));
                binary_offset += sizeof(int32_t);
				break;
            case CSON_TYPE_ULONG:    case CSON_TYPE_LONG:	case CSON_TYPE_DOUBLE:
				memcpy(buffer_addr + binary_offset, (struct_address + cson_model[i].offset + 7), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 1, (struct_address + cson_model[i].offset + 6), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 2, (struct_address + cson_model[i].offset + 5), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 3, (struct_address + cson_model[i].offset + 4), sizeof(uint8_t));
				memcpy(buffer_addr + binary_offset + 4, (struct_address + cson_model[i].offset + 3), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 5, (struct_address + cson_model[i].offset + 2), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 6, (struct_address + cson_model[i].offset + 1), sizeof(uint8_t));
                memcpy(buffer_addr + binary_offset + 7, (struct_address + cson_model[i].offset), sizeof(uint8_t));
                binary_offset += sizeof(uint64_t);
				break;
			//目前只支持char类型数组
			case CSON_TYPE_ARRAY:
				memcpy(buffer_addr + binary_offset, (struct_address + cson_model[i].offset), cson_model[i].param.array.size);
				binary_offset += cson_model[i].param.array.size;
				break;
            case CSON_TYPE_STRUCT:
                binary_offset += _serialize_big(cson_model[i].param.sub.model
                                            , cson_model[i].param.sub.size, (uint8_t *)(*(rt_uint32_t*)(struct_address + cson_model[i].offset)), buffer_addr + binary_offset);
                break;
			default:
				LOG_E("err: Unsupported type");
                break;
		}
	}
    return binary_offset;
}

void pp_serialize_big(const pp_model_t * model, uint8_t * buffer_addr) {
    PP_ASSERT(model != RT_NULL);
    PP_ASSERT(buffer_addr != RT_NULL);

    _serialize_big(model->cson_model, model->model_size, model->struct_address, buffer_addr);
}

static uint16_t _serialize(CsonModel *cson_model, uint16_t model_size, uint8_t *struct_address, uint8_t * buffer_addr) {
    PP_ASSERT(cson_model != RT_NULL);
    PP_ASSERT(model_size > 0);
    PP_ASSERT(struct_address != RT_NULL);
    PP_ASSERT(buffer_addr != RT_NULL);

    uint16_t binary_offset = 0;
    for (uint8_t i = 1; i < model_size; i++) { // 从1开始跳过第一个模型，它是整个结构体对象

		switch(cson_model[i].type) {
			case CSON_TYPE_CHAR:    case CSON_TYPE_UCHAR:
				memcpy(buffer_addr + binary_offset, (uint8_t *)(struct_address + cson_model[i].offset), sizeof(int8_t));
                binary_offset += sizeof(int8_t);
				break;
			case CSON_TYPE_SHORT:   case CSON_TYPE_USHORT:
				memcpy(buffer_addr + binary_offset, (uint8_t *)(struct_address + cson_model[i].offset), sizeof(int16_t));
                binary_offset += sizeof(int16_t);
				break;
			case CSON_TYPE_INT:     case CSON_TYPE_UINT:    case CSON_TYPE_FLOAT:
				memcpy(buffer_addr + binary_offset, (uint8_t *)(struct_address + cson_model[i].offset), sizeof(int32_t));
                binary_offset += sizeof(int32_t);
				break;
			case CSON_TYPE_LONG:    case CSON_TYPE_ULONG:   case CSON_TYPE_DOUBLE:
				memcpy(buffer_addr + binary_offset, (uint8_t *)(struct_address + cson_model[i].offset), sizeof(int64_t));
                binary_offset += sizeof(int64_t);
				break;
			//目前只支持char类型数组
			case CSON_TYPE_ARRAY:
				memcpy(buffer_addr + binary_offset, (uint8_t *)(struct_address + cson_model[i].offset), cson_model[i].param.array.size);
				binary_offset += cson_model[i].param.array.size;
				break;
            case CSON_TYPE_STRUCT:
				binary_offset += _serialize(cson_model[i].param.sub.model
                                            , cson_model[i].param.sub.size, (uint8_t *)(*(rt_uint32_t*)(struct_address + cson_model[i].offset)), buffer_addr + binary_offset);
				break;
			default:
				LOG_E("err: Unsupported type");
                break;
		}
	}
    return binary_offset;
}

void pp_serialize(const pp_model_t * model, uint8_t * buffer_addr) {
    PP_ASSERT(model != RT_NULL);
    PP_ASSERT(buffer_addr != RT_NULL);

    _serialize(model->cson_model, model->model_size, model->struct_address, buffer_addr);
}

static uint16_t _deserialize_big(uint8_t * buffer_addr, CsonModel *cson_model, uint16_t model_size, uint8_t *struct_address) {
    PP_ASSERT(buffer_addr != RT_NULL);
    PP_ASSERT(cson_model != RT_NULL);
    PP_ASSERT(model_size > 0);
    PP_ASSERT(struct_address != RT_NULL);

    uint16_t binary_offset = 0;
    for (uint8_t i = 1; i < model_size; i++) { // 从1开始跳过第一个模型，它是整个结构体对象

		switch(cson_model[i].type) {
			case CSON_TYPE_CHAR:    case CSON_TYPE_UCHAR:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, sizeof(int8_t));
                binary_offset += sizeof(int8_t);
				break;
			case CSON_TYPE_SHORT:   case CSON_TYPE_USHORT:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset + 1, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 1), buffer_addr + binary_offset, sizeof(int8_t));
                binary_offset += sizeof(int16_t);
				break;
			case CSON_TYPE_INT:     case CSON_TYPE_UINT:    case CSON_TYPE_FLOAT:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset + 3, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 1), buffer_addr + binary_offset + 2, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 2), buffer_addr + binary_offset + 1, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 3), buffer_addr + binary_offset, sizeof(int8_t));
                binary_offset += sizeof(int32_t);
				break;
			case CSON_TYPE_LONG:    case CSON_TYPE_ULONG:   case CSON_TYPE_DOUBLE:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset + 7, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 1), buffer_addr + binary_offset + 6, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 2), buffer_addr + binary_offset + 5, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 3), buffer_addr + binary_offset + 4, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 4), buffer_addr + binary_offset + 3, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 5), buffer_addr + binary_offset + 2, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 6), buffer_addr + binary_offset + 1, sizeof(int8_t));
				memcpy((uint8_t *)(struct_address + cson_model[i].offset + 7), buffer_addr + binary_offset, sizeof(int8_t));
                binary_offset += sizeof(int64_t);
				break;
            //目前只支持char类型数组
			case CSON_TYPE_ARRAY:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, cson_model[i].param.array.size);
				binary_offset += cson_model[i].param.array.size;
				break;

            case CSON_TYPE_STRUCT:
				binary_offset += _deserialize_big(buffer_addr + binary_offset, cson_model[i].param.sub.model
                                            , cson_model[i].param.sub.size, (uint8_t *)(*(rt_int32_t*)(struct_address + cson_model[i].offset)));
				break;
			default:
				LOG_E("err: Unsupported type");
                break;
		}
	}
    return binary_offset;
}

void pp_deserialize_big(uint8_t * buffer_addr ,const pp_model_t * model) {
    PP_ASSERT(buffer_addr != RT_NULL);
    PP_ASSERT(model != RT_NULL);

    _deserialize_big(buffer_addr, model->cson_model, model->model_size, model->struct_address);
}

static uint16_t _deserialize(uint8_t * buffer_addr, CsonModel *cson_model, uint16_t model_size, uint8_t *struct_address) {
    PP_ASSERT(buffer_addr != RT_NULL);
    PP_ASSERT(cson_model != RT_NULL);
    PP_ASSERT(model_size > 0);
    PP_ASSERT(struct_address != RT_NULL);

    uint16_t binary_offset = 0;
    for (uint8_t i = 1; i < model_size; i++) { // 从1开始跳过第一个模型，它是整个结构体对象

		switch(cson_model[i].type) {
			case CSON_TYPE_CHAR:    case CSON_TYPE_UCHAR:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, sizeof(int8_t));
                binary_offset += sizeof(int8_t);
				break;
			case CSON_TYPE_SHORT:   case CSON_TYPE_USHORT:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, sizeof(int16_t));
                binary_offset += sizeof(int16_t);
				break;
			case CSON_TYPE_INT:     case CSON_TYPE_UINT:    case CSON_TYPE_FLOAT:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, sizeof(int32_t));
                binary_offset += sizeof(int32_t);
				break;
			case CSON_TYPE_LONG:    case CSON_TYPE_ULONG:   case CSON_TYPE_DOUBLE:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, sizeof(int64_t));
                binary_offset += sizeof(int64_t);
				break;
            //目前只支持char类型数组
			case CSON_TYPE_ARRAY:
				memcpy((uint8_t *)(struct_address + cson_model[i].offset), buffer_addr + binary_offset, cson_model[i].param.array.size);
				binary_offset += cson_model[i].param.array.size;
				break;

            case CSON_TYPE_STRUCT:
				binary_offset += _deserialize(buffer_addr + binary_offset, cson_model[i].param.sub.model
                                            , cson_model[i].param.sub.size, (uint8_t *)(*(rt_int32_t*)(struct_address + cson_model[i].offset)));
				break;
			default:
				LOG_E("err: Unsupported type");
                break;
		}
	}
    return binary_offset;
}

void pp_deserialize(uint8_t * buffer_addr ,const pp_model_t * model) {
    PP_ASSERT(buffer_addr != RT_NULL);
    PP_ASSERT(model != RT_NULL);

    _deserialize(buffer_addr, model->cson_model, model->model_size, model->struct_address);
}
