idf_component_register(SRCS "bmi08_wrapper.c" 
                        "bmp5_wrapper.c" 
                        "mag_wrapper.c" 
                        "mmc5983.c"
                        "processing_task.c"
                        "BMP5-Sensor-API/bmp5.c"
                        "BMI08x-Sensor-API/bmi08a.c"
                        "BMI08x-Sensor-API/bmi08g.c"
                        "BMI08x-Sensor-API/bmi08xa.c"
                        "BMI08x-Sensor-API/bmi088_mma.c"
                        "fusion_wrapper.c"
                    INCLUDE_DIRS "." "BMP5-Sensor-API" "BMI08x-Sensor-API" "Fusion/Fusion"
                    REQUIRES cmock data sys)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")