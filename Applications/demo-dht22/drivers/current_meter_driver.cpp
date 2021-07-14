#include "current_meter_driver.h"

using namespace vitroio;
using namespace vitroio::dht_demo;

CurrentMeterDriver::CurrentMeterDriver(PinName adcCLPinName) :
    vitroio::sdk::AbstractSensorDriver(),
    dataLine_(adcCLPinName)
{}

CurrentMeterDriver::~CurrentMeterDriver()
{}

int CurrentMeterDriver::open()
{
    return 0;
}

void CurrentMeterDriver::close()
{}

int CurrentMeterDriver::read(char* buffer, uint32_t address, uint32_t bytes)
{
    uint16_t ADC_CL1_value = dataLine_.read_u16();
    uint32_t current = 0;
    float temp_value = 0.0;
    const float a = 0.003147;
    const float b = -25.001562;

    /* There is uint32_t variable available for parameter value.
     * Multiple measured value by 100 and return it as final.
     * E.g. 29.73 is represented as 2973 and so on.
     */
    if (ADC_CL1_value > 7950){
        temp_value = ((float)ADC_CL1_value * a) + b;
        current = (uint32_t)(temp_value*100);
    }
    else if (ADC_CL1_value = 0){
        // 0 value means that Current meter is probably not connected
        return -1;
    }

    buffer[0] = (uint8_t)current;
    buffer[1] = (uint8_t)(current >> 8);
    buffer[2] = (uint8_t)(current >> 16);
    buffer[3] = (uint8_t)(current >> 24);

    return 0;
}

int CurrentMeterDriver::write(const char* buffer, uint32_t address, uint32_t bytes)
{
    return -1;
}

int CurrentMeterDriver::ioctl(int request, void* value)
{
    return -1;
}
