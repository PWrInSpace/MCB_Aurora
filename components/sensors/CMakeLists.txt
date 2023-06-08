idf_component_register(SRC_DIRS "." "BMP5-Sensor-API" "BMI08x-Sensor-API"
                    INCLUDE_DIRS "." "BMP5-Sensor-API" "BMI08x-Sensor-API"
                    REQUIRES cmock data sys)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")