#ifndef GPS_TASK
#define GPS_TASK

#include <stdbool.h>

#define GPS_TASK_PERIOD CONFIG_GPS_TASK_PERIOD_MS
#define GPS_TASK_PRIORITY CONFIG_GPS_TASK_PRIORITY
#define GPS_TASK_DEPTH CONFIG_GPS_TASK_DEPTH
#define GPS_TASK_CPU CONFIG_GPS_TASK_CPU

typedef void (*gps_task_process)(void);

typedef struct{
    gps_task_process process_fnc;
} gps_task_cfg_t;


bool gps_create_task(gps_task_cfg_t *cfg);


#endif