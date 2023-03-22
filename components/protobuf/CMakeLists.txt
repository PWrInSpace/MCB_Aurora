idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock protobuf-c)

execute_process (
    COMMAND bash -c "portoc --c_out=. lora.proto"
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")