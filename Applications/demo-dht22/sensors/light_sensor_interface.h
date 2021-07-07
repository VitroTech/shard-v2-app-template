#ifndef LIGHT_SENSOR_INTERFACE_H
#define LIGHT_SENSOR_INTERFACE_H

#include <mbed.h>

#include <vitroio-sdk/measurement-api/abstract_sensor_interface.h>

namespace vitroio
{

namespace dht_demo
{

class LightSensorInterface : public vitroio::sdk::AbstractSensorInterface
{
public:
    LightSensorInterface(vitroio::sdk::AbstractSensorDriver* driver);

    ~LightSensorInterface();

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
    static const int parametersCount_ = 1;
    uint32_t parameters_[parametersCount_];
};

} // namespace dht_demo

} // namespace vitroio

#endif // LIGHT_SENSOR_INTERFACE_H

