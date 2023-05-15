#include <mbed.h>
#include <shard-sdk.h>
#include <vitroio-sdk/communication/can_layer.h>
#include <global_consts.h>

using namespace vitroio::sdk;

#define MAIN_MODULE_NAME "MAIN"
#define MAIN_INFO(format, ...) VITROIO_DEBUG_INFO(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_WARNING(format, ...) VITROIO_DEBUG_WARNING(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_ERROR(format, ...) VITROIO_DEBUG_ERROR(MAIN_MODULE_NAME, format, ##__VA_ARGS__);

#define SERIAL_BAUDRATE 115200

#define CAN_HANDLER_INFO(format, ...) VITROIO_DEBUG_INFO("CAN HANDLER", format, ##__VA_ARGS__);

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

void handler(const CanbusFrame& frame){
    CAN_HANDLER_INFO(
        "Got frame id: %x, param id: %x, data size: %d",
        frame.nodeId(),
        frame.parameterId(),
        frame.dataLength()
    )
    for(int i = 0; i < frame.dataLength(); i++){
        /* 
         * NOTE: printf might not be the best functions, it works,
         * but very poorly
         */
        printf("%x ", frame.data()[i]);
    }
    printf("\n");
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

    node.setExternalFrameCallback(&handler);

    IoTBlock iotBlock = IoTBlock(&can_layer);
    uint8_t block[256] = {0};
    static int i = 0;

    while(1) {
        block[i] = i;
        iotBlock.make(block, i + 1, 0x132);
        iotBlock.print();

        iotBlock.send();
        i++;
        if(i >= 256) i = 0;
        ThisThread::sleep_for(5s);
    }

    return 0;
}
