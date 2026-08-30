// Copyright 2022 PWrInSpace, Kuba

#include "console.h"

static esp_console_repl_t *s_repl = NULL;

esp_err_t console_init(void) {
    esp_err_t ret;
    if (s_repl != NULL) {
        return ESP_OK;
    }

    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "";
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ret = esp_console_new_repl_uart(&hw_config, &repl_config, &s_repl);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
        if (s_repl != NULL) {
            esp_console_deinit();
            s_repl = NULL;
        }
        return ret;
    }

    /* Do not start the repl here: register commands first to avoid race with REPL task */
    return ret;
}

esp_err_t console_start(void) {
    if (s_repl == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_console_start_repl(s_repl);
}

esp_err_t console_register_commands(esp_console_cmd_t *commands, size_t number_of_cmd) {
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < number_of_cmd; ++i) {
        ret = esp_console_cmd_register(&commands[i]);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ret;
}

esp_err_t console_deinit(void) {
    if (s_repl == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Use global deinit (available in this IDF) guarded by s_repl. */
    esp_err_t ret = esp_console_deinit();
    if (ret == ESP_OK) {
        s_repl = NULL;
    }

    return ret;
}

