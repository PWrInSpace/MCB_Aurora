idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock fatfs nvs_flash spi_flash spiffs pysd_gen sys app esp_timer errors)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")