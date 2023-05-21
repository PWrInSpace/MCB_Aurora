#include "settings_mem.h"

Settings settings;

// Private declarations:

#define SETTINGS_NAMESPACE "Settings"
#define SETTINGS_LORA_FREQ_MHZ "lora_MHz"
#define SETTINGS_LORA_FREQ_MS "lora_ms"
#define SETTINGS_COUNTDOWN_TIME "countdownTime"
#define SETTINGS_IGNIT_TIME "ignitTime"

nvs_handle_t settings_handle;

int32_t settings_read(char* setting_name);
esp_err_t settings_save(char* setting_name, int32_t newVal);

// Function definitions:

esp_err_t settings_init() {

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}

esp_err_t settings_save_all() {

    esp_err_t err = settings_save(SETTINGS_LORA_FREQ_MHZ, settings.loraFreq_MHz);
    if (err) ESP_LOGI(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_LORA_FREQ_MS, settings.loraFreq_ms);
    if (err) ESP_LOGI(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_COUNTDOWN_TIME, settings.countdownTime);
    if (err) ESP_LOGI(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_IGNIT_TIME, settings.ignitTime);
    if (err) ESP_LOGI(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    return ESP_OK;
}

esp_err_t settings_read_all() {

    settings.loraFreq_MHz   = settings_read(SETTINGS_LORA_FREQ_MHZ);
    settings.loraFreq_ms    = settings_read(SETTINGS_LORA_FREQ_MS);
    settings.countdownTime  = settings_read(SETTINGS_COUNTDOWN_TIME);
    settings.ignitTime      = settings_read(SETTINGS_IGNIT_TIME);

    return ESP_OK;
}

int32_t settings_read(char* setting_name) {

    nvs_open(SETTINGS_NAMESPACE, NVS_READONLY, &settings_handle);

    int32_t requestedVal = 0;
    nvs_get_i32(settings_handle, setting_name, &requestedVal);

    nvs_close(settings_handle);
    return requestedVal;
}

esp_err_t settings_save(char* setting_name, int32_t newVal) {

    // Check if the new value is different:
    int32_t oldVal = settings_read(setting_name);
    if (oldVal == newVal) return ESP_OK;

    // Save new value:
    esp_err_t err = nvs_open(SETTINGS_NAMESPACE, NVS_READWRITE, &settings_handle);

    err = nvs_set_i32(settings_handle, setting_name, newVal);

    // Commit changes to flash:
    err = nvs_commit(settings_handle);
    nvs_close(settings_handle);

    return err;
}