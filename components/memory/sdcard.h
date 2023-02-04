// Copyright 2022 PWrInSpace, Kuba

#ifndef SD_CARD_H
#define SD_CARD_H

#include <string.h>
#include <stdbool.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define MOUNT_POINT "/sdcard"

typedef struct {
    sdmmc_card_t *card;
    spi_host_device_t spi_host;
    const char* mount_point;
    uint8_t cs_pin;
    bool mounted;
} sd_card_t;

bool SD_init(sd_card_t *sd_card, spi_host_device_t host, uint8_t cs, const char *m_point);
bool SD_write(sd_card_t *sd_card, const char* path, const char* data, size_t length);
bool SD_file_exists(const char *path);
bool SD_remount(sd_card_t *sd_card);
bool SD_unmount(sd_card_t *sd_card);

#endif
