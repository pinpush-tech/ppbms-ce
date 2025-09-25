#include "pp_bl_timer.h"
#include "pp_drv_timer.h"
#include "stdio.h"
#include "pp_assert.h"

static pp_bl_timer_t *head;

static void _timer_add_node(pp_bl_timer_t *node) {
    PP_ASSERT(node != NULL);
    if(head == NULL) head = node;
    else {
        pp_bl_timer_t *temp_node = head;
        while(temp_node->next != NULL) {
            temp_node = temp_node->next;
        }
        temp_node->next = node;
    }
}

void pp_bl_timer_init(pp_bl_timer_t *timer, uint32_t time, void* param, void (*callback)(void*)) {
    PP_ASSERT(timer != NULL);
    PP_ASSERT(callback != NULL);

    timer->en = 0;
    timer->count = 0;
    timer->time = time;
    timer->param = param;
    timer->callback = callback;
    timer->next = NULL;
    _timer_add_node(timer);
}

void pp_bl_timer_ctrl(pp_bl_timer_t *timer, uint8_t en) {
    PP_ASSERT(timer != NULL);
    PP_ASSERT(en == 0 || en == 1);

    timer->en = en;
    timer->count = 0;
}

static void _timer_process(void) {
    pp_bl_timer_t *temp_head = head;
    while(temp_head != NULL) {
        if(temp_head->en) {
            temp_head->count++;
            if(temp_head->count >= temp_head->time) {
                temp_head->count = 0;
                if(temp_head->callback) {
                    temp_head->callback(temp_head->param);
                }
            }
        }
        temp_head = temp_head->next;
    }
}

void pp_bl_timer_hw_init(void) {
    pp_drv_timer_init(_timer_process);
}
void pp_bl_timer_hw_deinit(void) {
    pp_drv_timer_deinit();
}
