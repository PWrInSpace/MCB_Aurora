#ifndef DATA_TO_PROTOBUF
#define DATA_TO_PROTOBUF
#include "lora.pb-c.h.bk"

void create_protobuf_data_frame(struct obc_lo_ra_frame_t *frame);

void create_protobuf_settings_frame(struct obc_lo_ra_settings_t *frame);

#endif