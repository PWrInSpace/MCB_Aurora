// Copyright 2022 PWrInSpace, Kuba

#ifndef SIMPLE_SPI
#define SIMPLE_SPI

#include "driver/spi_common.h"
#include "driver/sdspi_host.h"

typedef struct {
    spi_bus_config_t spi_bus;
    spi_host_device_t spi_host;
} spi_t;

bool SPI_init(spi_t *spi, spi_host_device_t host, uint8_t mosi, uint8_t miso, uint8_t sck);

#endif
