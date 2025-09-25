#pragma once

#define    PP_HOST_ERR_OPR              1
#define    PP_HOST_ERR_INDEX            2//state code = 2 报ERR_INDEX
#define    PP_HOST_ERR_LENGTH           3
#define    PP_HOST_ERR_CRC              4
#define    PP_HOST_ERR_FAL              5//OTA写flash错误
#define    PP_HOST_ERR_HAVE_BEEN_OTA    6//曾经OTA过，OTA接收成功但设备正在使用没有重启，防止重复接收

#define    PP_HOST_ERR_CMD_ARGS        10
#define    PP_HOST_ERR_CMD_EXEC        11
#define    PP_HOST_ERR_CMD_STAGE       12
#define    PP_HOST_ERR_CMD_INDEX       13
