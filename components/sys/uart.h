#ifndef SYS_UART_H
#define SYS_UART_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "driver/uart.h"

typedef enum {
    UART_LOGICAL_GPS = 0,
    UART_LOGICAL_TELEMETRY,
    UART_LOGICAL_COUNT,
} uart_logical_port_t;

// Backward-compatible init: initializes GPS logical port.
bool uart_init(uart_port_t port, uint8_t tx_pin, uint8_t rx_pin, int baudrate);

// New API: select logical port explicitly.
bool uart_init_logical(uart_logical_port_t logical_port, uart_port_t port, uint8_t tx_pin, uint8_t rx_pin, int baudrate);

int uart_read_logical(uart_logical_port_t logical_port, uint8_t *data, size_t data_size, TickType_t timeout_ticks);

int uart_write_logical(uart_logical_port_t logical_port, const uint8_t *data, size_t data_size);

int uart_flush_rx_logical(uart_logical_port_t logical_port);

int uart_flush_tx_logical(uart_logical_port_t logical_port);

// Backward-compatible GPS-only wrappers.
int uart_ublox_read(uint8_t *data, uint8_t data_size);

int uart_ublox_write(uint8_t *data, uint8_t data_size);

int uart_flush_rx(void);

int uart_flush_tx(void);

#endif