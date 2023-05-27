// Copyright 2022 PWrInSpace, Kuba

#ifndef SIMPLE_SPI
#define SIMPLE_SPI

#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define SPI_MOSI CONFIG_SPI_MOSI
#define SPI_MISO CONFIG_SPI_MISO
#define SPI_SCK CONFIG_SPI_SCK


extern SemaphoreHandle_t mutex_spi;
bool spi_init(spi_host_device_t host, uint8_t mosi, uint8_t miso, uint8_t sck);
#endif
