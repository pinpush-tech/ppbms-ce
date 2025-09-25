#pragma once
#include <stdint.h>

typedef void(*pp_bl_timer_cb_t)(void *);

typedef struct _timer pp_bl_timer_t;
struct _timer{
    uint32_t time;
    void *param;
    pp_bl_timer_cb_t callback;
    uint32_t count;
    uint8_t en;
    pp_bl_timer_t *next;
};

void pp_bl_timer_hw_init(void);
void pp_bl_timer_hw_deinit(void);
void pp_bl_timer_init(pp_bl_timer_t *timer, uint32_t time, void* param, pp_bl_timer_cb_t callback);
void pp_bl_timer_ctrl(pp_bl_timer_t *timer, uint8_t en);
