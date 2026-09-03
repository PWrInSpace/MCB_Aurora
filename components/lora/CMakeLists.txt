idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock driver soc esp_rom sys gpio_expander esp_timer)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")