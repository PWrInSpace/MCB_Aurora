#ifndef DATA_TO_PROTOBUF
#define DATA_TO_PROTOBUF
#include "lora.pb-c.h"

void create_porotobuf_data_frame(LoRaFrame *frame);

void create_protobuf_settings_frame(LoRaSettings *frame);

#endif