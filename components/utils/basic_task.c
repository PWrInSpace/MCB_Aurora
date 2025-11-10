#include "basic_task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

static void basic_task(void *arg) {
    basic_task_handle_t * info = (basic_task_handle_t*)arg;
    while (1) {
        if (info->fnc != NULL) {
            info->fnc();
        }
        vTaskDelay(pdMS_TO_TICKS(info->task_delay));
    }
}


bool basic_task_create(basic_task_cfg_t *cfg, basic_task_t *task) {
    if (cfg->process_fnc == NULL) {
        return false;
    }

    task->task_handle = NULL;
    task->task_info.fnc = cfg->process_fnc;
    task->task_info.task_delay = cfg->task_delay;

    xTaskCreatePinnedToCore(
        basic_task,
        "GPS",
        cfg->stack_depth,
        &task->task_info,
        cfg->priority,
        &task->task_handle,
        cfg->core_id);

    if (task->task_handle == NULL) {
        // log free heap to aid debugging OOM
        size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
        ESP_LOGE("BASIC_TASK", "task create failed, free heap: %u", (unsigned)free_heap);
        return false;
    }

    return true;
}