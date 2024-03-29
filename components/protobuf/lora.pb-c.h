/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: lora.proto */

#ifndef PROTOBUF_C_lora_2eproto__INCLUDED
#define PROTOBUF_C_lora_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1004001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct LoRaFrame LoRaFrame;
typedef struct LoRaCommand LoRaCommand;
typedef struct LoRaSettings LoRaSettings;


/* --- enums --- */


/* --- messages --- */

struct  LoRaFrame
{
  ProtobufCMessage base;
  /*
   * mcb frame
   */
  uint32_t obc_state;
  uint32_t dc_time;
  uint32_t flight_time;
  float mcb_battery;
  float gps_lat;
  float gps_long;
  float gps_sat;
  float mcb_altitude;
  float mcb_velocity;
  int32_t mcb_temperature;
  float euler_fi;
  float euler_psi;
  float euler_theta;
  /*
   * recovery frame
   */
  uint32_t recov_pressure_1;
  uint32_t recov_pressure_2;
  uint32_t recov_byte_data;
  /*
   * pitot frame
   */
  float pitot_battery;
  int32_t pitot_altitude;
  int32_t pitot_velocity;
  int32_t pitot_temperature;
  /*
   * main valve
   */
  float mval_battery;
  uint32_t mval_byte_data;
  /*
   * vent valve
   */
  float vent_battery;
  float tank_pressure;
  uint32_t vent_byte_data;
  /*
   * tanwa frame
   */
  float tanwa_battery;
  uint32_t tanwa_byte_data;
  uint32_t tanwa_state;
  float rocket_weight;
  float tank_weight;
  float temperature1;
  float temperature2;
  float pressure;
  /*
   * payload frame
   */
  float payload_battery;
  uint32_t esp_now_byte_data;
  uint32_t errors;
};
#define LO_RA_FRAME__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&lo_ra_frame__descriptor) \
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }


struct  LoRaCommand
{
  ProtobufCMessage base;
  uint32_t lora_dev_id;
  uint32_t sys_dev_id;
  uint32_t command;
  int32_t payload;
};
#define LO_RA_COMMAND__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&lo_ra_command__descriptor) \
    , 0, 0, 0, 0 }


struct  LoRaSettings
{
  ProtobufCMessage base;
  int32_t lora_freq_khz;
  int32_t lora_transmit_ms;
  int32_t countdown_time;
  int32_t ingition_time;
  uint32_t flash_enable;
  uint32_t buzzer_enable;
};
#define LO_RA_SETTINGS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&lo_ra_settings__descriptor) \
    , 0, 0, 0, 0, 0, 0 }


/* LoRaFrame methods */
void   lo_ra_frame__init
                     (LoRaFrame         *message);
size_t lo_ra_frame__get_packed_size
                     (const LoRaFrame   *message);
size_t lo_ra_frame__pack
                     (const LoRaFrame   *message,
                      uint8_t             *out);
size_t lo_ra_frame__pack_to_buffer
                     (const LoRaFrame   *message,
                      ProtobufCBuffer     *buffer);
LoRaFrame *
       lo_ra_frame__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   lo_ra_frame__free_unpacked
                     (LoRaFrame *message,
                      ProtobufCAllocator *allocator);
/* LoRaCommand methods */
void   lo_ra_command__init
                     (LoRaCommand         *message);
size_t lo_ra_command__get_packed_size
                     (const LoRaCommand   *message);
size_t lo_ra_command__pack
                     (const LoRaCommand   *message,
                      uint8_t             *out);
size_t lo_ra_command__pack_to_buffer
                     (const LoRaCommand   *message,
                      ProtobufCBuffer     *buffer);
LoRaCommand *
       lo_ra_command__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   lo_ra_command__free_unpacked
                     (LoRaCommand *message,
                      ProtobufCAllocator *allocator);
/* LoRaSettings methods */
void   lo_ra_settings__init
                     (LoRaSettings         *message);
size_t lo_ra_settings__get_packed_size
                     (const LoRaSettings   *message);
size_t lo_ra_settings__pack
                     (const LoRaSettings   *message,
                      uint8_t             *out);
size_t lo_ra_settings__pack_to_buffer
                     (const LoRaSettings   *message,
                      ProtobufCBuffer     *buffer);
LoRaSettings *
       lo_ra_settings__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   lo_ra_settings__free_unpacked
                     (LoRaSettings *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*LoRaFrame_Closure)
                 (const LoRaFrame *message,
                  void *closure_data);
typedef void (*LoRaCommand_Closure)
                 (const LoRaCommand *message,
                  void *closure_data);
typedef void (*LoRaSettings_Closure)
                 (const LoRaSettings *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor lo_ra_frame__descriptor;
extern const ProtobufCMessageDescriptor lo_ra_command__descriptor;
extern const ProtobufCMessageDescriptor lo_ra_settings__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_lora_2eproto__INCLUDED */
