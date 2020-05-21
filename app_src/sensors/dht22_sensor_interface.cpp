#include "dht22_sensor_interface.h"

#include <vitroio-sdk/debug.h>

#include <drivers/dht22_driver.h>
#include <sensors/sensors_parameters.h>

using namespace vitroio::sdk;
using namespace vitroio::dht_demo;

#define DHT_SENSOR_MODULE_NAME "DHT SENSOR"
#define DHT_SENSOR_INFO(format, ...) VITROIO_DEBUG_INFO(DHT_SENSOR_MODULE_NAME, format, __VA_ARGS__);
#define DHT_SENSOR_WARNING(format, ...) VITROIO_DEBUG_WARNING(DHT_SENSOR_MODULE_NAME, format, __VA_ARGS__);
#define DHT_SENSOR_ERROR(format, ...) VITROIO_DEBUG_ERROR(DHT_SENSOR_MODULE_NAME, format, __VA_ARGS__);

#define DHT_DRIVER(driver) reinterpret_cast<Dht22Driver*>(driver)

Dht22SensorInterface::Dht22SensorInterface(vitroio::sdk::AbstractSensorDriver* driver) :
    vitroio::sdk::AbstractSensorInterface(driver),
    parametersCount_(2),
    lastReadValuesTime_(-DHT_SENSING_PERIOD_S)
{
    parameters_[0] = SENSPARAM_EXT_RH;
    parameters_[1] = SENSPARAM_EXT_T;
}

Dht22SensorInterface::~Dht22SensorInterface()
{
    driver()->close();
}

//
// TODO: Reading of values often failures during upgrade process
//
uint32_t Dht22SensorInterface::getValues(SensorParameterValue* values)
{
    static bool driverOpen = false;
    int err;

    DHT_SENSOR_INFO("%s", __FUNCTION__);

    if( (time(NULL) - lastReadValuesTime_) < DHT_SENSING_PERIOD_S ){
        values[0] = lastValues_[0];
        values[1] = lastValues_[1];
        return 2;
    } 

    if(!driverOpen){
        err = driver()->open();
        if(err != DHTDRV_ERR_SUCCESS){
            DHT_SENSOR_ERROR("Failed to open driver: code %d", err);
            return 0;
        }
        else{
            driverOpen = true;
        }
    }
    
    char data[DHT_READ_DATA_SIZE];

    err = DHT_DRIVER(driver())->read(data, 0, DHT_READ_DATA_SIZE);
    if(err != DHTDRV_ERR_SUCCESS){
        DHT_SENSOR_ERROR("Failed to read data: code %d", err);
        return 0;
    }

    values[0].parameter = SENSPARAM_EXT_RH;
    values[0].value = ((uint32_t)data[DHT_INTEGRAL_RH_DATA_OFFSET] << 8) | 
                      data[DHT_DECIMAL_RH_DATA_OFFSET];

    DHT_SENSOR_INFO("Hum: %d", values[0].value);

    values[1].parameter = SENSPARAM_EXT_T;
    
    uint16_t tempRegValue = ((uint16_t)data[DHT_INTEGRAL_T_DATA_OFFSET] << 8) | 
                            data[DHT_DECIMAL_T_DATA_OFFSET];
    uint32_t tempSign = (tempRegValue >> 15);
    uint32_t tempValue = (tempRegValue & 0x7FFF);

    values[1].value = tempValue | (tempSign << 31);

    DHT_SENSOR_INFO("Temp: %d", values[1].value);

    lastReadValuesTime_ = time(NULL);
    lastValues_[0] = values[0];
    lastValues_[1] = values[1];

    return parametersCount_;
}
