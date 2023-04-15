// Copyright 2022 PWrInSpace
#ifndef PROCESSING_TASK_CONFIG_H
#define PROCESSING_TASK_CONFIG_H

#include "processing_task.h"

typedef struct {
    uint8_t test;
} sensors_data_t;


bool initialize_processing_task(void);
#endif