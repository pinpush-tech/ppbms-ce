#pragma once
#include "stdint.h"
#include "pp_cfg.h"

typedef struct {
    char soft_version[20]; // ch[r]
    char hard_version[20]; // ch[r]
    char proto_version[20]; // ch[r]

    uint64_t timestamp; // sch[r]

    uint8_t log_swich; // sch[rw]
    uint8_t log_level; // sch[rw]

    uint8_t ota_done;

    char bat_id[32]; // ch[r]

    uint8_t stop_status; // s[r]
    uint8_t running_status; // s[r]

    uint8_t mos_lock_chg; // sch[rw]
    uint8_t mos_lock_dsg; // sch[rw]

    uint8_t restart_flag; // h[w]
    uint8_t restart4fota_flag; // h[w]
    uint8_t wkup_flag; // h[w]
	uint8_t store_flag; // h[w]
    uint8_t restore_afe_flag; // h[w]
	uint8_t restore_sox_flag; // h[w]
    uint8_t stop_flag; // h[w]
    uint8_t stop_reset_flag;
    uint8_t shutdown_flag; // h[w]

    uint16_t host_recv_data_size; // h[r]
    uint8_t first_cycle_flag;
} pp_data_sys_t;

extern pp_data_sys_t _sys;
