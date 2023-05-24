#ifndef GPS_TASK
#define GPS_TASK

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef void (*basic_task_loop)(void);

typedef struct{
    basic_task_loop process_fnc;
    UBaseType_t priority;
    BaseType_t core_id;
    uint32_t stack_depth;
    uint32_t task_delay;
} basic_task_cfg_t;

typedef struct {
    basic_task_loop fnc;
    uint32_t task_delay;
} basic_task_handle_t;

typedef struct {
    TaskHandle_t task_handle;
    basic_task_handle_t task_info;
} basic_task_t;

/**
 * @brief Create and run basic task
 * 
 * @param cfg task config
 * @param task basic task handle
 * @return true :D
 * @return false :C
 */
bool basic_task_create(basic_task_cfg_t *cfg, basic_task_t *task);


#endif