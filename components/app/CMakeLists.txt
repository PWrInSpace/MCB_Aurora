idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock esp_now state_machine sys lora utils protobuf esp_timer
                    cli sd_card data sensors timers commands errors flash_memory pysd_gen
                    gps gpio_expander settings)
 
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")