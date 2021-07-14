#ifndef DHT22_DRIVER_H
#define DHT22_DRIVER_H

#include <cstddef>

#include <mbed.h>

#include <vitroio-sdk/measurement-api/abstract_sensor_driver.h>

#include <drivers/dht22_defines.h>

//
// Errors 
//
#define DHTDRV_ERR_SUCCESS 0
#define DHTDRV_ERR_OPERATION_NOT_SUPPORTED 1
#define DHTDRV_ERR_UNKNOWN_REQUEST 2
#define DHTDRV_ERR_RESPONSE_TIMEOUT 3
#define DHTDRV_ERR_RESPONSE_DURATION_INVALID 4
#define DHTDRV_ERR_READ_BIT_TIMEOUT 5
#define DHTDRV_ERR_DEFINE_BIT 6
#define DHTDRV_ERR_CHECKSUM 7
#define DHTDRV_ERR_LOW_STATE_TIMEOUT 8
#define DHTDRV_ERR_GENERAL_HIGH_STATE_TIMEOUT 9
#define DHTDRV_ERR_HIGH_STATE_TIMEOUT 10
#define DHTDRV_ERR_HIGH_1_STATE_TIMEOUT 11

namespace vitroio
{

namespace dht_demo
{

class Dht22Driver : public vitroio::sdk::AbstractSensorDriver
{
public:
    Dht22Driver(PinName dataLinePinName);

    ~Dht22Driver();

    /**
     * @brief Function sets the sensor to idle state.
     */
    int open();

    void close();

    /**
     * @brief Function always reads 4 bytes of data. The address and bytes
     * parameters are ignored.
     * @note Provided buffer have to be at least of 4 bytes size.
     */ 
    int read(char* buffer, 
             uint32_t address = 0, 
             uint32_t bytes = DHT_READ_DATA_SIZE);

    int write(const char* buffer, 
              uint32_t address, 
              uint32_t bytes);

    int ioctl(int request, void* value = NULL);

private:
    DigitalInOut dataLine_;
    Timer timer_;

    void setToIdleState();

    /**
     * @brief Function blocks until the value of data line of DHT sensor will
     * be equal to provided one or the timeout will occure.
     * @param tmr - timer, it have to be started before call of this function.
     * Started timer is passed as argument to achieve better measurement
     * accuracy.
     * @return Function returns waiting time in us or -1 if timeout occured.
     */
    inline int waitForLineValue(int value, int timeout);
};

} // namespace dht_demo

} // namespace vitroio

#endif // DHT22_DRIVER_H
