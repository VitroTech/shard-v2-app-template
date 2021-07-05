#include "current_meter_interface.h"
#include <vitroio-sdk/debug.h>
#include <drivers/current_meter_driver.h>
#include <sensors/sensors_parameters.h>

using namespace vitroio::sdk;
using namespace vitroio::dht_demo;

#define CURRENT_METER_MODULE_NAME "CURRENT METER"
#define CURRENT_METER_INFO(format, ...) VITROIO_DEBUG_INFO(CURRENT_METER_MODULE_NAME, format, __VA_ARGS__);
#define CURRENT_METER_WARNING(format, ...) VITROIO_DEBUG_WARNING(CURRENT_METER_MODULE_NAME, format, __VA_ARGS__);
#define CURRENT_METER_ERROR(format, ...) VITROIO_DEBUG_ERROR(CURRENT_METER_MODULE_NAME, format, __VA_ARGS__);

#define CURRENT_METER_DRIVER(driver) reinterpret_cast<CurrentMeterDriver*>(driver)

CurrentMeterInterface::CurrentMeterInterface(vitroio::sdk::AbstractSensorDriver* driv) :
    vitroio::sdk::AbstractSensorInterface(driv)
{
    parameters_[0] = SENSPARAM_CURRENT;
}

CurrentMeterInterface::~CurrentMeterInterface()
{
    driver()->close();
}

uint32_t CurrentMeterInterface::getValues(vitroio::sdk::SensorParameterValue* values)
{
    char data[4];
    
    if(0 != CURRENT_METER_DRIVER(driver())->read(data, 0, 1))
    {
        CURRENT_METER_ERROR("Failed to read data from sensor. Check if current meter is connected.", -1);
        return 0;
    }

    values[0].parameter = SENSPARAM_CURRENT;
    values[0].value = (uint32_t)data[0] | (uint32_t)data[1] << 8 | (uint32_t)data[2] << 16 | (uint32_t)data[3] << 24;

    CURRENT_METER_INFO("Current: %u", values[0].value);

    return 1;
}