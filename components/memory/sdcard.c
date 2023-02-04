// Copyright 2022 PWrInSpace. Kuba

#include <string.h>
#include <sys/stat.h>
#include "sdcard.h"

#define TAG "SDCARD"

static bool SD_mount(sd_card_t *sd_card) {
    esp_err_t res;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = sd_card->spi_host;
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = sd_card->cs_pin;
    slot_config.host_id = host.slot;

    res = esp_vfs_fat_sdspi_mount(sd_card->mount_point, &host, &slot_config,
                                  &mount_config, &sd_card->card);
    if (res != ESP_OK) {
        if (res == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the"
                     "CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(res));
        }
        return false;
    }
    sd_card->mounted = true;
    return true;
}

bool SD_init(sd_card_t *sd_card, spi_host_device_t spi_host, uint8_t cs_pin, const char* m_point) {
    sd_card->spi_host = spi_host;
    sd_card->cs_pin = cs_pin;
    sd_card->mount_point = m_point;

    // Options for mounting the filesystem
    if (SD_mount(sd_card) == false) {
        return false;
    }

    return true;
}

bool SD_file_exists(const char* file_name) {
    struct stat st;
    if (stat(file_name, &st) == 0) {
        return true;
    }

    return false;
}


bool SD_unmount(sd_card_t *sd_card) {
    if (sd_card->mounted == false) {
        return true;
    }

    esp_err_t res;
    res = esp_vfs_fat_sdcard_unmount(sd_card->mount_point, sd_card->card);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "UNMOUNT ERROR\n");
        return false;
    }
    sd_card->mounted = false;
    return true;
}


bool SD_remount(sd_card_t *sd_card) {
    bool res;
    res = SD_unmount(sd_card);
    if (res == false) {
        return false;
    }

    res = SD_mount(sd_card);
    return res;
}

bool SD_write(sd_card_t *sd_card, const char* path, const char* data, size_t length) {
    esp_err_t res;

    if (sd_card->mounted == false) {
        if (SD_mount(sd_card) == false) {
            return false;
        }
    }

    res = sdmmc_get_status(sd_card->card);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "CARD ERROR, REMOOUNTING...");
        SD_remount(sd_card);
        return false;
    }

    FILE *file = fopen(path, "a");
    if (file == NULL) {
        ESP_LOGE(TAG, "FILE OPEN ERROR");
        return false;
    }

    int written_bytes = 0;
    written_bytes = fprintf(file, data, sd_card->card->cid.name);
    fclose(file);

    if (written_bytes < 1) {
        ESP_LOGE(TAG, "UNABLE TO WRITE DATA TO SD CARD");
        return false;
    }

    return true;
}
