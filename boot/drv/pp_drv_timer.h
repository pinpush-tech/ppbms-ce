#pragma once

typedef void(*pp_drv_timer_cb_t)(void);

void pp_drv_timer_init(pp_drv_timer_cb_t cb);
void pp_drv_timer_deinit(void);
