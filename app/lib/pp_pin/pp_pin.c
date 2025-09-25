#include "pp_pin/pp_pin.h"
#include "pp_assert/pp_assert.h"

typedef struct {
    void (*irq_cb)(void);
} _handle_rf_t;


static void _irq_rf(void *args) {
    PP_ASSERT(args != RT_NULL);
    _handle_rf_t * handle = args;
    PP_ASSERT(handle->irq_cb != RT_NULL);
    handle->irq_cb();
}

static void _init_irq(rt_base_t pin, rt_base_t pin_mode, rt_uint32_t irq_mode, void (*irq_cb)(void)) {
    PP_ASSERT(irq_cb != RT_NULL);
    _handle_rf_t * handle = rt_malloc(sizeof(_handle_rf_t));
    PP_ASSERT(handle != RT_NULL);
    handle->irq_cb = irq_cb;
    rt_pin_mode(pin, pin_mode);
    rt_pin_attach_irq(pin, irq_mode, _irq_rf, handle);
    rt_pin_irq_enable(pin, PIN_IRQ_ENABLE);
}

void pp_pin_init_irq_rising(rt_base_t pin, rt_base_t pin_mode, void (*irq_cb)(void)) {
    PP_ASSERT(irq_cb != RT_NULL);
    _init_irq(pin, pin_mode, PIN_IRQ_MODE_RISING, irq_cb);
}

void pp_pin_init_irq_failing(rt_base_t pin, rt_base_t pin_mode, void (*irq_cb)(void)) {
    PP_ASSERT(irq_cb != RT_NULL);
    _init_irq(pin, pin_mode, PIN_IRQ_MODE_FALLING, irq_cb);
}

pp_pin_handle_t * pp_pin_init_irq_level(rt_base_t pin, rt_base_t pin_mode, uint8_t init_high, void (*poll_cb)(uint8_t high), void (*check_cb)(uint8_t high)) {
    pp_pin_handle_t * handle = rt_malloc(sizeof(pp_pin_handle_t));
    PP_ASSERT(handle != RT_NULL);
    handle->pin = pin;
    handle->high = init_high;
    handle->poll_cb = poll_cb;
    handle->check_cb = check_cb;
    rt_pin_mode(pin, pin_mode);
    return handle;
}

void pp_pin_check_level(pp_pin_handle_t * handle) {
    PP_ASSERT(handle != RT_NULL);
    uint8_t old_high = handle->high;
    handle->high = rt_pin_read(handle->pin);
    if (handle->poll_cb != RT_NULL && handle->high != old_high) {
        handle->poll_cb(handle->high);
    }
    if (handle->check_cb != RT_NULL) {
        handle->check_cb(handle->high);
    }
}