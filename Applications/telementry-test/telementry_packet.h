#ifndef TELEMENTRY_PACKET_H
#define TELEMENTRY_PACKET_H

#include <stdint.h>

#define TELEMENTRY_PER_BLOCK    4

/* standard telementry */
typedef struct{
    uint32_t battery_voltage;
    uint32_t temperature_pole_1;
    uint32_t temperature_pole_2;
    uint32_t resistance;
    uint32_t current;
}standard_telementry;

/* discharge telementry */
typedef struct{
    uint32_t battery_voltage;
    uint32_t temperature_pole_1;
    uint32_t temperature_pole_2;
}discharge_telementry;

#endif
