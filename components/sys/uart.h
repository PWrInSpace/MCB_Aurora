#ifndef SYS_UART_H
#define SYS_UART_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"


bool uart_init(uart_port_t port, uint8_t tx_pin, uint8_t rx_pin, int baudrate);

int uart_ublox_read(uint8_t *data, uint8_t data_size);

int uart_ublox_write(uint8_t *data, uint8_t data_size);


#endif