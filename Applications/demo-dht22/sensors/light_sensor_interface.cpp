#include "light_sensor_interface.h"

#include <vitroio-sdk/debug.h>
#include <drivers/j16_driver.h>
#include <sensors/sensors_parameters.h>

using namespace vitroio::sdk;
using namespace vitroio::dht_demo;

#define LIGHT_SENSOR_MODULE_NAME "LIGHT SENSOR"
#define LIGHT_SENSOR_INFO(format, ...) VITROIO_DEBUG_INFO(LIGHT_SENSOR_MODULE_NAME, format, __VA_ARGS__);
#define LIGHT_SENSOR_WARNING(format, ...) VITROIO_DEBUG_WARNING(LIGHT_SENSOR_MODULE_NAME, format, __VA_ARGS__);
#define LIGHT_SENSOR_ERROR(format, ...) VITROIO_DEBUG_ERROR(LIGHT_SENSOR_MODULE_NAME, format, __VA_ARGS__);

#define J16_DRV(drv) reinterpret_cast<J16Driver*>(drv)

LightSensorInterface::LightSensorInterface(vitroio::sdk::AbstractSensorDriver* driv) :
    vitroio::sdk::AbstractSensorInterface(driv)
{
    parameters_[0] = SENSPARAM_LIGHT;
    
    while(J16_DRV(driver())->open() != J16_SUCCESS){
        LIGHT_SENSOR_ERROR("Failed to open J16 driver.", -1);
    }
    
    //set J16 driver in input mode
    J16_DRV(driver())->ioctl(J16_INPUT_MODE);
}

LightSensorInterface::~LightSensorInterface()
{
    J16_DRV(driver())->close();
}

uint32_t LightSensorInterface::getValues(vitroio::sdk::SensorParameterValue* values)
{
    char data[4];
    
    if(J16_SUCCESS!=J16_DRV(driver())->read(data, 0, 1))
    {
        LIGHT_SENSOR_ERROR("Failed to read sensor value.", -1);
        return 0;
    }

    values[0].parameter = SENSPARAM_LIGHT;
    values[0].value = !(data[0]);//negation becouse sensor gives 0 if light is on, 1 if light is off

    LIGHT_SENSOR_INFO("Light: %lu", values[0].value);

    return 1;
}
