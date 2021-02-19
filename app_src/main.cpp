#include <mbed.h>
#include <shard-sdk.h>
#include <global_consts.h>
/**
 * Place for including application dependent drivers and interfaces
 */



using namespace vitroio::sdk;
//If your drivers and sensors use namespaces, remember to use them.


#define MAIN_MODULE_NAME "MAIN"
#define MAIN_INFO(format, ...) VITROIO_DEBUG_INFO(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_WARNING(format, ...) VITROIO_DEBUG_WARNING(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_ERROR(format, ...) VITROIO_DEBUG_ERROR(MAIN_MODULE_NAME, format, ##__VA_ARGS__);

#define SERIAL_BAUDRATE 115200

#define TIMESTAMP_REQUEST_PARAMETER_ID 0x40

/**
 * Watchdog
 */
Watchdog wdt;
Ticker wdtKicker;

/**
 * Peripherals
 */
Serial pc(UART_DEBUG_TX, UART_DEBUG_RX, SERIAL_BAUDRATE);
DigitalOut statusLed(VITROIO_TEMPLATE_STATUS_LED_PIN);

/**
 * Event queues and threads
 */
Thread normalPriorityThread(osPriorityNormal, 0x1800);
EventQueue normalPriorityEventQueue(32*EVENTS_EVENT_SIZE);
Thread highPriorityThread(osPriorityHigh, 0x1800);
EventQueue highPriorityEventQueue(32*EVENTS_EVENT_SIZE);

ShardEdge shardEdgeConfig(false, false, false, false, 16, 16, 16, 16);

/**
 * Functions declaration
 */
void updateTimestamp();
void measurementsHandler(SensorParameterValue* values, int);
void frameReceivedCallback(const CanbusFrame& frame);

/**
 * vitroio-sdk objects
 */
IoTBlock iotBlock;
Canbus canbus(CAN_RX, CAN_TX, &highPriorityEventQueue);

NodeController node(
    &canbus,
    FirmwareId((uint16_t)VITRIOIO_TEMPLATE_FIRMWARE_ID),
    Version(
        VITROIO_TEMPLATE_VERSION_MAJOR, 
        VITROIO_TEMPLATE_VERSION_MINOR, 
        VITROIO_TEMPLATE_VERSION_PATCH,
        VITROIO_TEMPLATE_VERSION_RC), 
    FlashSpace(
        VITROIO_TEMPLATE_ENVIRONMENT_FLASH_OFFSET, 
        VITROIO_TEMPLATE_ENVIRONMENT_FLASH_SECTORS), 
    FlashSpace(
        VITROIO_TEMPLATE_REGION_A_FLASH_OFFSET, 
        VITROIO_TEMPLATE_REGION_FLASH_SECTORS),
    FlashSpace(
        VITROIO_TEMPLATE_REGION_B_FLASH_OFFSET, 
        VITROIO_TEMPLATE_REGION_FLASH_SECTORS),
    &highPriorityEventQueue
);

//MeasurementApi template parameter is maximum number of registered sensors.
//If you want to register more then 4 change this number.
MeasurementApi<4> measurementApi(&normalPriorityEventQueue, 32);

/**
 * global variable for timestamps from Gateway
 */
volatile uint32_t timestampGlobal = 0;

/**
 * @brief Function called by Canbus when frame is received
 * 
 * @param frame Received frame
 */
void frameReceivedCallback(const CanbusFrame& frame)
{
    if (frame.parameterId() == TIMESTAMP_REQUEST_PARAMETER_ID)
    {
        timestampGlobal = *((uint32_t*)frame.data());
    }
}

/**
 * Place for sensors drivers declaration and initialisation
 */



/**
 * Place for sensors interfaces declaration and initialisation
 */



int main()
{
    MAIN_INFO("Application started; (id: %d) v%d.%d.%d.%d; vitroio-sdk v%s",
        VITRIOIO_TEMPLATE_FIRMWARE_ID,
        VITROIO_TEMPLATE_VERSION_MAJOR, VITROIO_TEMPLATE_VERSION_MINOR, VITROIO_TEMPLATE_VERSION_PATCH, VITROIO_TEMPLATE_VERSION_RC,
        VITROIO_SDK_VERSION);

    wdtKicker.attach(callback(&wdt, &Watchdog::Service), 2.0);

    highPriorityThread.start(callback(&highPriorityEventQueue, &EventQueue::dispatch_forever));
    normalPriorityThread.start(callback(&normalPriorityEventQueue, &EventQueue::dispatch_forever));

    if(!canbus.isValid() || !node.isValid() || !measurementApi.isValid()){
        MAIN_ERROR("Failed to initialize vitroio-sdk objects");
        NVIC_SystemReset();
    }

    int frameReceivedHandler = 0;
    frameReceivedHandler = canbus.setOnFrameReceivedCallback(callback(&frameReceivedCallback));

    if(frameReceivedHandler < 0){
        MAIN_ERROR("Failed to set callback for received frames");
    }

    if (shardEdgeConfig.configureShardEdge() != 0){
        MAIN_ERROR("Failed to configure Shard Edge");
    }

    int err = node.initCommunication();
    if(err != VITROIO_ERR_SUCCESS){
        MAIN_ERROR("Failed to initialize communication");
    }

    /**
     * Measurement api initialization
     */
    measurementApi.setPollTime(10);
    //Uncomment the line below and provide the sensor interface variable
    //measurementApi.registerSensor(&/*your_sensor_interface*/);
    measurementApi.setOnNewValuesCallback(measurementsHandler);
    measurementApi.startMeasurements();

    while(1) {
        ThisThread::sleep_for(0xFFFFFFFF);
    }

    return 0;
}

/**
 * @brief Function called by measurementApi
 * 
 * @details This function is called with a selected period. An array of sensors
 * values and its size are passed as arguments. The function prepares IoTBlocks
 * containing sensors values and sends them by CAN bus
 * 
 * @param values Pointer to a sensors values array
 * @param quantity Number of sensors values in the array
 */
void measurementsHandler(SensorParameterValue* values, int quantity)
{
    updateTimestamp();

    for(int i=0; i<quantity; i++){
        iotBlock.make_iot(values[i].value, values[i].parameter, timestampGlobal);
        iotBlock.CAN_iot(&canbus, node.nodeId());
        ThisThread::sleep_for(10);
    }
}

/**
 * @brief Sends timestamp request to Gateway and waits for response
 * 
 * @note Uses CAN bus to send timestamp request. Canbus object required
 */
void updateTimestamp()
{
    int err = -1;
    CanbusFrame canFrameTimestamp;

    canFrameTimestamp.setNodeId(node.nodeId());
    canFrameTimestamp.setParameterId(TIMESTAMP_REQUEST_PARAMETER_ID);
    memset(canFrameTimestamp.data(), 0x00, 1);
    canFrameTimestamp.data()[0] = 0x00;

    err = canbus.sendFrame(canFrameTimestamp);

    if(err != VITROIO_ERR_SUCCESS){
        MAIN_ERROR("Failed to send timestamp request over CAN bus: %d", err);
    }
    /**
     * Give some time for response
     */
    ThisThread::sleep_for(10);
    MAIN_INFO("Timestamp = %lu", (timestampGlobal));
}
