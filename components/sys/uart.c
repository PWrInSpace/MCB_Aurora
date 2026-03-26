#include "uart.h"

#define BUF_SIZE (1024)

static struct {
    uart_port_t port[UART_LOGICAL_COUNT];
    bool ready[UART_LOGICAL_COUNT];
} gb = {
    .port = {UART_NUM_MAX, UART_NUM_MAX},
    .ready = {false, false},
};

static bool uart_is_valid_logical_port(uart_logical_port_t logical_port) {
    return logical_port >= UART_LOGICAL_GPS && logical_port < UART_LOGICAL_COUNT;
}

static bool uart_setup_port(uart_port_t port, uint8_t tx_pin, uint8_t rx_pin, int baudrate) {
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    if (uart_driver_install(port, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, intr_alloc_flags) != ESP_OK) {
        return false;
    }

    if (uart_param_config(port, &uart_config) != ESP_OK) {
        return false;
    }

    if (uart_set_pin(port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        return false;
    }

    return true;
}

bool uart_init(uart_port_t port, uint8_t tx_pin, uint8_t rx_pin, int baudrate) {
    return uart_init_logical(UART_LOGICAL_GPS, port, tx_pin, rx_pin, baudrate);
}

bool uart_init_logical(uart_logical_port_t logical_port,
                       uart_port_t port,
                       uint8_t tx_pin,
                       uint8_t rx_pin,
                       int baudrate) {
    if (!uart_is_valid_logical_port(logical_port)) {
        return false;
    }

    if (!uart_setup_port(port, tx_pin, rx_pin, baudrate)) {
        return false;
    }

    gb.port[logical_port] = port;
    gb.ready[logical_port] = true;
    return true;
}

int uart_write_logical(uart_logical_port_t logical_port, const uint8_t *data, size_t data_size) {
    if (!uart_is_valid_logical_port(logical_port) || !gb.ready[logical_port] || data == NULL || data_size == 0) {
        return -1;
    }

    return uart_write_bytes(gb.port[logical_port], (const char *) data, data_size);
}

int uart_read_logical(uart_logical_port_t logical_port,
                      uint8_t *data,
                      size_t data_size,
                      TickType_t timeout_ticks) {
    if (!uart_is_valid_logical_port(logical_port) || !gb.ready[logical_port] || data == NULL || data_size == 0) {
        return -1;
    }

    return uart_read_bytes(gb.port[logical_port], data, data_size, timeout_ticks);
}

int uart_flush_rx_logical(uart_logical_port_t logical_port) {
    if (!uart_is_valid_logical_port(logical_port) || !gb.ready[logical_port]) {
        return -1;
    }

    return uart_flush_input(gb.port[logical_port]);
}

int uart_flush_tx_logical(uart_logical_port_t logical_port) {
    if (!uart_is_valid_logical_port(logical_port) || !gb.ready[logical_port]) {
        return -1;
    }

    return uart_wait_tx_done(gb.port[logical_port], pdMS_TO_TICKS(200));
}

int uart_ublox_write(uint8_t *data, uint8_t data_size) {
    return uart_write_logical(UART_LOGICAL_GPS, data, data_size);
}

int uart_ublox_read(uint8_t *data, uint8_t data_size) {
    return uart_read_logical(UART_LOGICAL_GPS, data, data_size, pdMS_TO_TICKS(500));
}

int uart_flush_rx(void) {
    return uart_flush_rx_logical(UART_LOGICAL_GPS);
}

int uart_flush_tx(void) {
    return uart_flush_tx_logical(UART_LOGICAL_GPS);
}