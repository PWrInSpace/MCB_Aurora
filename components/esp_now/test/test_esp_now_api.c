// Copyright 2022 PWrInSpace, Kuba
#include "unity.h"
#include "esp_now_api.h"

ENA_device_t dev_one = {
    .peer = {.peer_addr = {0xDF, 0xDF, 0xDF, 0xDF, 0xDF, 0xDF}, .channel = 0},
    .on_receive = NULL,
};

ENA_device_t dev_two = {
    .peer = {.peer_addr = {0xCF, 0xCF, 0xCF, 0xCF, 0xAF, 0xAF}, .channel = 0},
    .on_receive = NULL,
};

TEST_CASE("Initialization", "[ENA]") {
    uint8_t mac_address[] = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20};

    TEST_ASSERT_EQUAL(ESP_OK, ENA_init(mac_address));
    // TEST_ASSERT_EQUAL_HEX_ARRAY();
}

TEST_CASE("Add peer", "[ENA]") {
    TEST_ASSERT_EQUAL(ESP_OK, ENA_register_device(&dev_one));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, ENA_register_device(&dev_one));
    TEST_ASSERT_EQUAL(ESP_OK, ENA_register_device(&dev_two));
}

TEST_CASE("Run task", "[ENA]") {
    ENA_config_t cfg = {
        .stack_depth = 8000,
        .core_id = APP_CPU_NUM,
        .priority = 2,
    };
    TEST_ASSERT_EQUAL(ESP_OK, ENA_run(&cfg));
}

TEST_CASE("Check peer", "[ENA]") {
    ENA_device_t dev;
    TEST_ASSERT_EQUAL(ESP_OK, ENA_get_dev_info(&dev, 0));
    TEST_ASSERT_EQUAL_HEX_ARRAY(dev_one.peer.peer_addr, dev.peer.peer_addr, MAC_ADDRESS_SIZE);

    TEST_ASSERT_EQUAL(ESP_OK, ENA_get_dev_info(&dev, 1));
    TEST_ASSERT_EQUAL_HEX_ARRAY(dev_two.peer.peer_addr, dev.peer.peer_addr, MAC_ADDRESS_SIZE);
}
