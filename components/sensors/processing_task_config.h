// Copyright 2022 PWrInSpace
#ifndef PROCESSING_TASK_CONFIG_H
#define PROCESSING_TASK_CONFIG_H

#include "processing_task.h"
#include "slave_structs.h"

typedef struct {
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int test6;
    float test_float;
    recovery_data_t recovery;
} sensors_data_t;

/**
 * @brief Initialize configured processing task
 *
 * @return true :D
 * @return false :C
 */
bool initialize_processing_task(void);
#endif