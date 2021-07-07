#ifndef DHT22_SENSOR_INTERFACE_H
#define DHT22_SENSOR_INTERFACE_H

#include <mbed.h>

#include <vitroio-sdk/measurement-api/abstract_sensor_interface.h>

namespace vitroio
{

namespace dht_demo
{

class Dht22SensorInterface : public vitroio::sdk::AbstractSensorInterface
{
public:
    Dht22SensorInterface(vitroio::sdk::AbstractSensorDriver* driver);

    ~Dht22SensorInterface();

    const uint32_t* getParameters() const
    {
        return parameters_;
    }

    uint32_t getParametersCount() const
    {
        return parametersCount_;
    }

    uint32_t getValues(vitroio::sdk::SensorParameterValue* values);

private:
    const uint32_t parametersCount_;
    uint32_t parameters_[2];
    int lastReadValuesTime_;
    vitroio::sdk::SensorParameterValue lastValues_[2];
};

} // namespace dht_demo

} // namespace vitroio

#endif // DHT22_SENSOR_INTERFACE_H
