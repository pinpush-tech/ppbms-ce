#pragma once

#include "pp_cfg.h"

extern const struct fal_flash_dev onchip_flash;
#define FAL_FLASH_DEV_TABLE     { &onchip_flash }

#if PP_PCB_MCU_FLASH_SIZE == (512 * 1024)
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD,"bootloader",     "onchip",   0,                                       PP_PCB_MCU_FLASH_BL_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,   "bn_data",     "onchip",   PP_PCB_MCU_FLASH_BNDATA_OFFSET,          PP_PCB_MCU_FLASH_BNDATA_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,   "bl_data",     "onchip",   PP_PCB_MCU_FLASH_BLDATA_OFFSET,          PP_PCB_MCU_FLASH_BLDATA_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,       "app",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET,             200*1024, 0}, \
    {FAL_PART_MAGIC_WORD,  "download",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 200*1024,  200*1024, 0}, \
    {FAL_PART_MAGIC_WORD,     "kv_db",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 400*1024,  10 *1024, 0}, \
    {FAL_PART_MAGIC_WORD,     "ts_db",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 410*1024,  86 *1024, 0}, \
}
#endif

#if PP_PCB_MCU_FLASH_SIZE == (256 * 1024)
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD,"bootloader",     "onchip",   0,                                       PP_PCB_MCU_FLASH_BL_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,   "bn_data",     "onchip",   PP_PCB_MCU_FLASH_BNDATA_OFFSET,          PP_PCB_MCU_FLASH_BNDATA_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,   "bl_data",     "onchip",   PP_PCB_MCU_FLASH_BLDATA_OFFSET,          PP_PCB_MCU_FLASH_BLDATA_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,       "app",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET,             100*1024, 0}, \
    {FAL_PART_MAGIC_WORD,  "download",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 100*1024,  100*1024, 0}, \
    {FAL_PART_MAGIC_WORD,     "kv_db",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 200*1024,  10 *1024, 0}, \
    {FAL_PART_MAGIC_WORD,     "ts_db",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 210*1024,  30 *1024, 0}, \
}
#endif

#if PP_PCB_MCU_FLASH_SIZE == (128 * 1024)
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD,"bootloader",     "onchip",   0,                                       PP_PCB_MCU_FLASH_BL_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,   "bn_data",     "onchip",   PP_PCB_MCU_FLASH_BNDATA_OFFSET,          PP_PCB_MCU_FLASH_BNDATA_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,   "bl_data",     "onchip",   PP_PCB_MCU_FLASH_BLDATA_OFFSET,          PP_PCB_MCU_FLASH_BLDATA_SIZE,  0}, \
    {FAL_PART_MAGIC_WORD,       "app",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET,             90 *1024, 0}, \
    {FAL_PART_MAGIC_WORD,     "kv_db",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET +  90*1024,  10 *1024, 0}, \
    {FAL_PART_MAGIC_WORD,     "ts_db",     "onchip",   PP_PCB_MCU_FLASH_APP_OFFSET + 100*1024,  12 *1024, 0}, \
}
#endif
