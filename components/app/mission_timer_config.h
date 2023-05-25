#ifndef MISSION_TIMER_CONFIG_H
#define MISSION_TIMER_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initalize hybrid rocket mission timer
 * 
 * @return true :D
 * @return false :C
 */
bool hybrid_mission_timer_init();

/**
 * @brief Start hybrid rocket mission timer
 * 
 * @param countdown_time coutndown time, negative value -> T:-x
 * @param ignition_time ignitonion time, negative value -> T:-x
 * @return true :D
 * @return false :C
 */
bool hybrid_mission_timer_start(int32_t countdown_time, int32_t ignition_time);

/**
 * @brief Stop mission timer
 * 
 * @return true :D
 * @return false :C
 */
bool hybrid_mission_timer_stop();

/**
 * @brief Get hybrid rocket mission time
 *
 * @return int time in microseconds
 */
int hybrid_mission_timer_get_time();

#endif