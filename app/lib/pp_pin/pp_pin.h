#pragma once

#include "stdint.h"
#include "rtdevice.h"

typedef struct {
    rt_base_t pin;
    uint8_t high;
    void (*poll_cb)(uint8_t high);
    void (*check_cb)(uint8_t high);
} pp_pin_handle_t;


void pp_pin_init_irq_rising(rt_base_t pin, rt_base_t pin_mode, void (*irq_cb)(void));
void pp_pin_init_irq_failing(rt_base_t pin, rt_base_t pin_mode, void (*irq_cb)(void));
pp_pin_handle_t * pp_pin_init_irq_level(rt_base_t pin, rt_base_t pin_mode, uint8_t init_high, void (*poll_cb)(uint8_t high), void (*check_cb)(uint8_t high));
void pp_pin_check_level(pp_pin_handle_t * handle);