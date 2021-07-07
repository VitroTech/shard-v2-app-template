#ifndef CURRENT_METER_DRIVER_H
#define CURRENT_METER_DRIVER_H

#include <mbed.h>
#include <vitroio-sdk/measurement-api/abstract_sensor_driver.h>

namespace vitroio
{

namespace dht_demo
{

class CurrentMeterDriver : public vitroio::sdk::AbstractSensorDriver
{
public:
    CurrentMeterDriver(PinName adcCLPinName);

    ~CurrentMeterDriver();

    int open();

    void close();

    int read(char* buffer, uint32_t address, uint32_t bytes);

    int write(const char* buffer, uint32_t address, uint32_t bytes);

    int ioctl(int request, void* value = NULL);

private:
    AnalogIn dataLine_;
};

} // namespace dht_demo

} // namespace vitroio

#endif //CURRENT_METER_DRIVER_H