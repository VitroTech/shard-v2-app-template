#include <mbed.h>
#include <shard-sdk.h>
#include <vitroio-sdk/communication/can_layer.h>
#include <global_consts.h>

//#include "telementry_packet.h"
#define TELEMENTRY_PER_BLOCK            4

/* standard telementry */
#define STANDARD_TELEMENTRY_SIZE        20

#define STANDARD_BATTERY_VOLTAGE_SIZE   4
#define STANDARD_BATTERY_VOLTAGE_OFFSET 0

#define STANDARD_TEMP_POLE_SIZE         4
#define STANDARD_TEMP_POLE_1_OFFSET     4
#define STANDARD_TEMP_POLE_2_OFFSET     8

#define STANDARD_RESISTANCE_SIZE        4
#define STANDARD_RESISTANCE_OFFSET      12

#define STANDARD_CURRENT_SIZE           4
#define STANDARD_CURRENT_OFFSET         16

/* discharge telementry */
#define DISCHARGE_TELEMENTRY_SIZE           12

#define DISCHARGE_BATTERY_VOLTAGE_SIZE      4
#define DISCHARGE_BATTERY_VOLTAGE_OFFSET    0

#define DISCHARGE_TEMP_POLE_SIZE            4
#define DISCHARGE_TEMP_POLE_1_OFFSET        4
#define DISCHARGE_TEMP_POLE_2_OFFSET        8

using namespace vitroio::sdk;

#define MAIN_MODULE_NAME "MAIN"
#define MAIN_INFO(format, ...) VITROIO_DEBUG_INFO(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_WARNING(format, ...) VITROIO_DEBUG_WARNING(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_ERROR(format, ...) VITROIO_DEBUG_ERROR(MAIN_MODULE_NAME, format, ##__VA_ARGS__);

#define SERIAL_BAUDRATE 115200

/**
 * Watchdog ticker
 */
Ticker wdtKicker;

/**
 * Peripherals
 */
BufferedSerial pc(UART_DEBUG_TX, UART_DEBUG_RX, SERIAL_BAUDRATE);
DigitalOut statusLed(VITROIO_TEMPLATE_STATUS_LED_PIN);

/**
 * Event queues and threads
 */
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
Canbus canbus(CAN_RX, CAN_TX, &highPriorityEventQueue);

NodeController node(
    &canbus,
    FirmwareId((uint16_t)VITRIOIO_TEMPLATE_FIRMWARE_ID),
    Version(
        VITROIO_TEMPLATE_VERSION_MAJOR,
        VITROIO_TEMPLATE_VERSION_MINOR,
        VITROIO_TEMPLATE_VERSION_PATCH,
        VITROIO_TEMPLATE_VERSION_RC),
    &highPriorityEventQueue
);

Can_layer can_layer(&canbus, node.nodeId());

FileHandle *mbed::mbed_override_console(int fd)
{
    return &pc;
}

void kickWatchdog(){
    Watchdog::get_instance().kick();
}

void fillPoolingModeBlock(uint8_t *block){
    /* read some values here and copy them to block */
    for(int i = 0; i < TELEMENTRY_PER_BLOCK; i++){
        *(uint32_t *)(block + i * STANDARD_TELEMENTRY_SIZE + STANDARD_BATTERY_VOLTAGE_OFFSET) = (uint32_t)1;
        *(uint32_t *)(block + i * STANDARD_TELEMENTRY_SIZE + STANDARD_TEMP_POLE_1_OFFSET) = (uint32_t)2;
        *(uint32_t *)(block + i * STANDARD_TELEMENTRY_SIZE + STANDARD_TEMP_POLE_2_OFFSET) = (uint32_t)3;
        *(uint32_t *)(block + i * STANDARD_TELEMENTRY_SIZE + STANDARD_RESISTANCE_OFFSET) = (uint32_t)4;
    }
}

void sendPoolingModeBlock(IoTBlock &iotBlock){
    uint8_t block[128] = {0};
    fillPoolingModeBlock(block);
    iotBlock.make(block, 1, 0x132);
    iotBlock.print();
    iotBlock.send();
}

void fillDischargeModeBlock(uint8_t *block){
    /* read some values here and copy them to block */
    for(int i = 0; i < TELEMENTRY_PER_BLOCK; i++){
        *(uint32_t *)(block + i * DISCHARGE_TELEMENTRY_SIZE + DISCHARGE_BATTERY_VOLTAGE_OFFSET) = (uint32_t)1;
        *(uint32_t *)(block + i * DISCHARGE_TELEMENTRY_SIZE + DISCHARGE_TEMP_POLE_1_OFFSET) = (uint32_t)2;
        *(uint32_t *)(block + i * DISCHARGE_TELEMENTRY_SIZE + DISCHARGE_TEMP_POLE_2_OFFSET) = (uint32_t)3;
    }
}

void sendDischargeModeBlock(IoTBlock &iotBlock){
    uint8_t block[128] = {0};
    fillDischargeModeBlock(block);
    iotBlock.make(block, 1, 0x132);
    iotBlock.print();
    iotBlock.send();
}

int main()
{
    MAIN_INFO("Application started; (id: %d) v%d.%d.%d.%d; vitroio-sdk v%s",
        VITRIOIO_TEMPLATE_FIRMWARE_ID,
        VITROIO_TEMPLATE_VERSION_MAJOR, VITROIO_TEMPLATE_VERSION_MINOR, VITROIO_TEMPLATE_VERSION_PATCH, VITROIO_TEMPLATE_VERSION_RC,
        VITROIO_SDK_VERSION);

    Watchdog &watchdog = Watchdog::get_instance();
    watchdog.start(10000);
    wdtKicker.attach(&kickWatchdog, 2.0);

    highPriorityThread.start(callback(&highPriorityEventQueue, &EventQueue::dispatch_forever));

    if(!canbus.isValid() || !node.isValid()){
        MAIN_ERROR("Failed to initialize vitroio-sdk objects");
        NVIC_SystemReset();
    }

    if (shardEdgeConfig.configureShardEdge() != 0){
        MAIN_ERROR("Failed to configure Shard Edge");
    }

    int err = node.initCommunication(&can_layer);
    if(err != VITROIO_ERR_SUCCESS){
        MAIN_ERROR("Failed to initialize communication");
    }

    for(;;) {
    }

    return 0;
}
