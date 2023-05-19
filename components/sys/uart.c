#include "uart.h"

#define BUF_SIZE (1024)

static struct {
    uart_port_t uart_gps_port;
} gb;

bool uart_init(uart_port_t port, uint8_t tx_pin, uint8_t rx_pin, int baudrate) {
 uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;


    if (uart_driver_install(port, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags) != ESP_OK) {
        return false;
    }

    if (uart_param_config(port, &uart_config) != ESP_OK) {
        return false;
    }

    if (uart_set_pin(port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        return false;
    }

    gb.uart_gps_port = port;
    return true;
}


int uart_ublox_write(uint8_t *data, uint8_t data_size) {
    return uart_write_bytes(gb.uart_gps_port, data, data_size);
}


int uart_ublox_read(uint8_t *data, uint8_t data_size) {
    return uart_read_bytes(gb.uart_gps_port, data, data_size, pdMS_TO_TICKS(500));
}