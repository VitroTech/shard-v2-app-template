#ifndef J16_DRIVER_H
#define J16_DRIVER_H

#include <mbed.h>

#include <vitroio-sdk/measurement-api/abstract_sensor_driver.h>
#include <j16_defines.h>

namespace vitroio
{

namespace dht_demo
{

class J16Driver : public vitroio::sdk::AbstractSensorDriver
{
public:
    J16Driver(PinName dataPinName);

    ~J16Driver();

    int open();

    void close();

    int read(char* buffer, uint32_t address, uint32_t bytes);

    int write(const char* buffer, uint32_t address, uint32_t bytes);

    int ioctl(int request, void* value = NULL);

private:
    DigitalInOut    dataLine_;
    void            setInputMode();
    void            setOutputMode();
    void            setPullupMode();

    uint8_t         mode;
};

} // namespace dht_demo

} // namespace vitroio

#endif // J16_DRIVER_H
