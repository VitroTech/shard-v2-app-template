#include <j16_driver.h>
#include <j16_defines.h>

using namespace vitroio;
using namespace vitroio::dht_demo;

J16Driver::J16Driver(PinName dataPinName) :
    vitroio::sdk::AbstractSensorDriver(),
    dataLine_(dataPinName)
{
    setInputMode();
    mode = J16_INPUT_MODE;
}

J16Driver::~J16Driver()
{}

int J16Driver::open()
{
    return J16_SUCCESS;
}

void J16Driver::close()
{}

int J16Driver::read(char* buffer, uint32_t address, uint32_t bytes)
{
    if(J16_INPUT_MODE!=mode && J16_PULLUP_MODE!=mode)
        return J16_ERR_READ;
    if(1!=bytes)
        return J16_INVALID_DATA_SIZE;
    
    int signal = dataLine_.read();
    *(int*)(buffer) = signal;
    
    return J16_SUCCESS;
}

int J16Driver::write(const char* buffer, uint32_t address, uint32_t bytes)
{
    if(J16_OUTPUT_MODE!=mode)
        return J16_ERR_WRITE;
    if(1!=bytes)
        return J16_INVALID_DATA_SIZE;
    
    dataLine_=(int)(buffer);
    
    return J16_SUCCESS;
    
}

int J16Driver::ioctl(int request, void* value)
{
    switch(request)
    {
        case J16_INPUT_MODE:
            setInputMode();
            return J16_SUCCESS;
        case J16_OUTPUT_MODE:
            setOutputMode();
            return J16_SUCCESS;
        case J16_PULLUP_MODE:
            setPullupMode();
            return J16_SUCCESS;
        default:
            return J16_UNKNOWN_REQUEST;
    }
}

void J16Driver::setInputMode()
{
    dataLine_.input();
    mode=J16_INPUT_MODE;
}

void J16Driver::setOutputMode()
{
    dataLine_.output();
    mode=J16_OUTPUT_MODE;
}

void J16Driver::setPullupMode()
{
    dataLine_.mode(PullUp);
    mode=J16_PULLUP_MODE;
}
