idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock driver soc esp_rom sys)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")