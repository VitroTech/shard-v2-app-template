#include "dht22_driver.h"

using namespace vitroio;
using namespace vitroio::dht_demo;

#define DHT_DATA_BYTES 5

//
// TODO:
// - this value is determined experimentally, the sensor have to be tested
//   with longer connection cables to validate correctness of operating
//
#define DHT_ACCEPTABLE_DURATION_DIFF_US 20

#define DHT_WAIT_FOR_RESPONSE_DURATION_US 40 // 20-200us typ. 30

#define DHT_RESPONSE_LOW_DURATION_US 80     // 75-85us  typ. 80
#define DHT_RESPONSE_HIGH_DURATION_US 80    // 75-85us  typ. 80

#define DHT_BITS_GAP_DURATION_US 65     // 48-55us   typ.50
#define DHT_BIT_0_DURATION_US 20        // 22-30us   typ.26
#define DHT_BIT_1_DURATION_US 65        // 68-75us   typ.70

#define DHT_MAX_BIT_DURATION_US 75      // 75us

#define DURATION_TIMEOUT_US(duration) ( duration + DHT_ACCEPTABLE_DURATION_DIFF_US )
#define DURATION_TIMEOUTED(value, duration) ( value > DURATION_TIMEOUT_US(duration) )
#define DURATION_VALID(value, duration) ( (value >= (duration-DHT_ACCEPTABLE_DURATION_DIFF_US)) && (value <= (duration+DHT_ACCEPTABLE_DURATION_DIFF_US)) )

#define STOP_READ_AND_RETURN(err) timer_.stop(); setToIdleState(); return err;

Dht22Driver::Dht22Driver(PinName dataLinePinName) :
    vitroio::sdk::AbstractSensorDriver(),
    dataLine_(dataLinePinName, PIN_INPUT, OpenDrainNoPull, 1)
{}

Dht22Driver::~Dht22Driver()
{}

int Dht22Driver::open()
{
    setToIdleState();
    return DHTDRV_ERR_SUCCESS;
}

void Dht22Driver::close()
{}

int Dht22Driver::read(char* buffer, uint32_t address, uint32_t bytes)
{
    int durationUs = 0;

    timer_.start();

    //
    // Send start signal and keep it at least 2ms
    //
    dataLine_.output();
    dataLine_.write(0);
    wait_ms(2);
    //dataLine_.write(1);

    //
    // Get back to input mode and read response from sensor
    //
    dataLine_.input();
    dataLine_.mode(OpenDrainNoPull);

    //
    // Wait for response low signal
    //
    durationUs = waitForLineValue(0, DURATION_TIMEOUT_US(DHT_WAIT_FOR_RESPONSE_DURATION_US));
    if(durationUs == -1){
        STOP_READ_AND_RETURN(DHTDRV_ERR_RESPONSE_TIMEOUT);
    }

    //
    // Validate response low signal
    //
    durationUs = waitForLineValue(1, DURATION_TIMEOUT_US(DHT_RESPONSE_LOW_DURATION_US));
    if(durationUs == -1){
        STOP_READ_AND_RETURN(DHTDRV_ERR_RESPONSE_TIMEOUT);
    }
    if(!DURATION_VALID(durationUs, DHT_RESPONSE_LOW_DURATION_US)){
        STOP_READ_AND_RETURN(DHTDRV_ERR_RESPONSE_DURATION_INVALID);
    }

    //
    // Validate response high signal
    //
    durationUs = waitForLineValue(0, DURATION_TIMEOUT_US(DHT_RESPONSE_HIGH_DURATION_US));
    if(durationUs == -1){
        STOP_READ_AND_RETURN(DHTDRV_ERR_RESPONSE_TIMEOUT);
    }
    if(!DURATION_VALID(durationUs, DHT_RESPONSE_HIGH_DURATION_US)){
        STOP_READ_AND_RETURN(DHTDRV_ERR_RESPONSE_DURATION_INVALID);
    }

    //
    // Read data bits
    //
    for(int byte=0; byte<DHT_DATA_BYTES; byte++){

        char readByte = 0;

        for(int bit=0; bit<8; bit++){
            
            readByte <<= 1;

            if(waitForLineValue(1, DURATION_TIMEOUT_US(DHT_BITS_GAP_DURATION_US)) == -1){
                STOP_READ_AND_RETURN(DHTDRV_ERR_LOW_STATE_TIMEOUT);
            }

            durationUs = waitForLineValue(0, DURATION_TIMEOUT_US(DHT_MAX_BIT_DURATION_US));
            if(durationUs == -1){
                STOP_READ_AND_RETURN(DHTDRV_ERR_GENERAL_HIGH_STATE_TIMEOUT);
            }

            if( DURATION_VALID(durationUs, DHT_BIT_0_DURATION_US) ){
                continue;
            }
            else if( DURATION_VALID(durationUs, DHT_BIT_1_DURATION_US) ){
                readByte |= 1;
            }
            else{
                STOP_READ_AND_RETURN(DHTDRV_ERR_HIGH_STATE_TIMEOUT);
            }
        }

        // Last byte is checksum
        if( byte != DHT_DATA_BYTES-1 ){
            buffer[byte] = readByte;
        }
        else{
            char checksum = 0;
            for(int i=0; i<DHT_DATA_BYTES-1; i++){
                checksum += buffer[i];
            }
            if(readByte != checksum){
                STOP_READ_AND_RETURN(DHTDRV_ERR_CHECKSUM);
            }
        }
    }

    STOP_READ_AND_RETURN(DHTDRV_ERR_SUCCESS);
}

int Dht22Driver::write(const char* buffer, uint32_t address, uint32_t bytes)
{
    return DHTDRV_ERR_OPERATION_NOT_SUPPORTED;
}

int Dht22Driver::ioctl(int request, void* value)
{
    return DHTDRV_ERR_UNKNOWN_REQUEST;
}

void Dht22Driver::setToIdleState()
{
    dataLine_.input();
    dataLine_.mode(OpenDrainNoPull);
}

inline int Dht22Driver::waitForLineValue(int value, int timeout)
{
    timer_.reset();
    while(dataLine_.read() != value){
        if(timer_.read_us() > timeout){
            return -1;
        }
    }
    int ret = timer_.read_us();
    return ret;
}
