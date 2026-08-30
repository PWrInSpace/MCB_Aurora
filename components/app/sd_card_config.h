#ifndef SD_CARD_CONFIG
#define SD_CARD_CONFIG

#include <stdint.h>
#include <stdbool.h>

#define MCB_DATA_PATH "mcb"

/**
 * @brief Initalize sd card task
 * 
 * @return true :D
 * @return false :C
 */
bool initialize_sd_card(void);

#endif