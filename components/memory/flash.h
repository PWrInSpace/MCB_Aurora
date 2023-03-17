// Copyright 2022 PWr in Space, Krzysztof Gliwiński
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "spi_flash_mmap.h"

#include "esp_log.h"
#include "esp_spiffs.h"

#define PATH "/spiffs"
#define FLASH_FILE_NAME "flash"
#define MAX_FILES 1

#define FLASH_PATH PATH "/" FLASH_FILE_NAME

/*!
  \file flash.h
  \brief contains API to store huge data files in ESP
          internal flash. Description based on
  https://esp32tutorials.com/esp32-spiffs-esp-idf/

  \section How to create a spiffs partition in flash:
  \subsection Create a file and name it 'partitions.csv' e.g.:
    # Name,   Type, SubType, Offset,  Size, Flags
    # Note: if you change the phy_init or app partition offset,
    make sure to change the offset in Kconfig.projbuild
    nvs,      data, nvs,     ,        0x6000,
    phy_init, data, phy,     ,        0x1000,
    factory,  app,  factory, ,        1M,
    storage,  data, spiffs,  ,        1M
  \subsection Then create a folder named spiffs data and include files e.g.
              data.txt.
  \subsection Then at the end of CMakeLists.txt in /main folder add
  spiffs_create_partition_image(storage ../spiffs_data FLASH_IN_PROJECT)

  \subsection Launch idf.py menuconfig, and go to > Serial flasher condif
  \subsection Go to partition table and make sure that these are set:
            Flash SPI mode (DIO)  --->
            Flash Sampling Mode (STR Mode)  --->
            Flash SPI speed (40 MHz)  --->
            Flash size (YOUR MCUS FLASH SIZE HERE)  --->
  \subsection Now go to back, and to Partition Table, select these
            (partitions.csv) Custom partition CSV file
            (0x8000) Offset of partition table
            [*] Generate an MD5 checksum for the partition table
  ! NOTE: if you change the phy_init or app partition offset,
          make sure to change the partition table offset (above) accordingly
*/

typedef enum {
  FLASH_OK,
  FLASH_INIT_ERROR,
  FLASH_ALREADY_INITIALIZED,
  FLASH_IS_NOT_INITIALIZED,
  FLASH_CHECK_FAILED,
  FLASH_WRITE_ERROR,
  FLASH_OPEN_ERROR,
  FLASH_FORMAT_ERROR,
  FLASH_READ_NO_DATA,
  FLASH_READ_ALL_SIZE,
  FLASH_ALLOC_ERROR,
} FlashResult;


FlashResult FLASH_init(uint8_t max_files);
FlashResult FLASH_write(const char* file_name, const char* data, size_t size);
FlashResult FLASH_read_all_data(const char* file_name, char* data_container,
                                size_t size);
size_t FLASH_get_used_size(void);
size_t FLASH_get_total_size(void);
FlashResult FLASH_format(void);
