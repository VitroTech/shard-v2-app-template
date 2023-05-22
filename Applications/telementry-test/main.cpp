#include <mbed.h>
#include <shard-sdk.h>
#include <vitroio-sdk/communication/can_layer.h>
#include <global_consts.h>

#include "telementry_packet.h"

#define STANDARD_BLOCK      0
#define DISCHARGE_BLOCK     1

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
    standard_telementry *tmp = (standard_telementry *)block;
    for(int i = 0; i < TELEMENTRY_PER_BLOCK; i++){
        tmp[i] = {
            .battery_voltage = 1,
            .temperature_pole_1 = 2,
            .temperature_pole_2 = 3,
            .resistance = 4,
            .current = 5,
        };
    }
}

void fillDischargeModeBlock(uint8_t *block){
    /* read some values here and copy them to block */
    discharge_telementry *tmp = (discharge_telementry *)block;
    for(int i = 0; i < TELEMENTRY_PER_BLOCK; i++){
        tmp[i] = {
            .battery_voltage = 1,
            .temperature_pole_1 = 2,
            .temperature_pole_2 = 3,
        };
    }
}

void sendBlock(IoTBlock &iotBlock, uint8_t blockType){
    uint8_t block[128] = {0};

    switch (blockType)
    {
    case STANDARD_BLOCK:
        fillPoolingModeBlock(block);
        break;
    case DISCHARGE_BLOCK:
        fillDischargeModeBlock(block);
        break;
    default:
        break;
    }

    iotBlock.make(block, sizeof(block), 0x132);
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

    IoTBlock iotBlock = IoTBlock(&can_layer);

    int type = DISCHARGE_BLOCK;
    int blockCounter = 0;

    for(;;) {
        switch (type){
        case DISCHARGE_BLOCK:
            sendBlock(iotBlock, type);
            ThisThread::sleep_for(1s);
            if(++blockCounter == 60){
                type = STANDARD_BLOCK;
                blockCounter = 0;
            }
            break;
        case STANDARD_BLOCK:
            sendBlock(iotBlock, type);
            ThisThread::sleep_for(60s);
            if(++blockCounter == 5){
                type = DISCHARGE_BLOCK;
                blockCounter = 0;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
