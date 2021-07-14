#ifndef CURRENT_METER_INTERFACE_H
#define CURRENT_METER_INTERFACE_H

#include <mbed.h>

#include <vitroio-sdk/measurement-api/abstract_sensor_interface.h>

namespace vitroio
{

namespace dht_demo
{

class CurrentMeterInterface : public vitroio::sdk::AbstractSensorInterface
{
public:
    CurrentMeterInterface(vitroio::sdk::AbstractSensorDriver* driver);

    ~CurrentMeterInterface();

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
    static const uint32_t parametersCount_ = 1;
    uint32_t parameters_[parametersCount_];
};

} // namespace dht_demo

} // namespace vitroio

#endif // CURRENT_METER_INTERFACE_H