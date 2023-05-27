#include "mag_wrapper.h"
#include "i2c.h"


static struct {
    mmc5983_t dev;
}gb;

static void mag_delay(uint32_t millis) {
    vTaskDelay(pdMS_TO_TICKS(millis));
}

bool mag_init(void) {
    gb.dev.dev_addr = 0x30;
    gb.dev.read_fnc = i2c_sensors_read;
    gb.dev.write_fnc = i2c_sensors_write;
    gb.dev.delay_fnc = mag_delay;

    return mmc5893_init(&gb.dev);
}

bool mag_set_continous_mode(mmc5983_freq_t freq, mmc5983_prd_set_t prd) {
    return mmc5893_set_continous_mode(&gb.dev, freq, prd);
}

bool mag_data_ready() {
    return mmc5893_mag_ready(&gb.dev);
}

bool mag_get_data(mmc5983_mag_t *mag) {
    return mmc5893_get_mag_data(&gb.dev, mag);
}