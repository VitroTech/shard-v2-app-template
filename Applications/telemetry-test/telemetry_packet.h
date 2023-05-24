#ifndef telemetry_PACKET_H
#define telemetry_PACKET_H

#include <stdint.h>

#define telemetry_PER_BLOCK    4

/* standard telemetry */
typedef struct{
    uint32_t battery_voltage;
    uint32_t temperature_pole_1;
    uint32_t temperature_pole_2;
    uint32_t resistance;
    uint32_t current;
}standard_telemetry;

/* discharge telemetry */
typedef struct{
    uint32_t battery_voltage;
    uint32_t temperature_pole_1;
    uint32_t temperature_pole_2;
}discharge_telemetry;

#endif
