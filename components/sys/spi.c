// Copyright 2022 PWrInSpace, Kuba

#include "spi.h"

bool SPI_init(spi_t *spi, spi_host_device_t host, uint8_t mosi, uint8_t miso, uint8_t sck) {
    esp_err_t ret;
    spi->spi_bus.mosi_io_num = mosi;
    spi->spi_bus.miso_io_num = miso;
    spi->spi_bus.sclk_io_num = sck;
    spi->spi_bus.quadwp_io_num = -1;
    spi->spi_bus.quadhd_io_num = -1;
    spi->spi_bus.max_transfer_sz = 4000;
    spi->spi_host = host;

    ret = spi_bus_initialize(spi->spi_host, &spi->spi_bus, SDSPI_DEFAULT_DMA);
    return ret == ESP_OK ? true : false;
//     esp_err_t ret;
//     spi_bus_config_t bus = {
//       .miso_io_num = 19,
//       .mosi_io_num = 23,
//       .sclk_io_num = 18,
//       .quadwp_io_num = -1,
//       .quadhd_io_num = -1,
//       .max_transfer_sz = 4000,
//    };

//    ret = spi_bus_initialize(VSPI_HOST, &bus, SDSPI_DEFAULT_DMA);
//    assert(ret == ESP_OK);
//    return ret;
}


esp_err_t spi_add_lora(spi_device_handle_t *spi_dev_handle, spi_t *spi) {
    esp_err_t ret;
    spi_device_interface_config_t dev = {
       .clock_speed_hz = 9000000,
       .mode = 0,
       .spics_io_num = -1,
       .queue_size = 1,
       .flags = 0,
       .pre_cb = NULL
    };
    ret = spi_bus_add_device(spi->spi_host, &dev, spi_dev_handle);
    assert(ret == ESP_OK);
    return ret;
}