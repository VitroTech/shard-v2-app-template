#include <mbed.h>
#include <shard-sdk.h>
#include <global_consts.h>
/**
 * Place for including application dependent drivers and interfaces
 */



using namespace vitroio::sdk;

#define MAIN_MODULE_NAME "MAIN"
#define MAIN_INFO(format, ...) VITROIO_DEBUG_INFO(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_WARNING(format, ...) VITROIO_DEBUG_WARNING(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_ERROR(format, ...) VITROIO_DEBUG_ERROR(MAIN_MODULE_NAME, format, ##__VA_ARGS__);

#define SERIAL_BAUDRATE 115200

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

ShardEdge shardEdgeConfig(false, false, false, false, 16, 16, 16, 16);

int main()
{
    MAIN_INFO("Application started; (id: %d) v%d.%d.%d.%d; vitroio-sdk v%s",
        VITRIOIO_TEMPLATE_FIRMWARE_ID,
        VITROIO_TEMPLATE_VERSION_MAJOR, VITROIO_TEMPLATE_VERSION_MINOR, VITROIO_TEMPLATE_VERSION_PATCH, VITROIO_TEMPLATE_VERSION_RC,
        VITROIO_SDK_VERSION);

    wdtKicker.attach(callback(&wdt, &Watchdog::Service), 2.0);

    if (shardEdgeConfig.configureShardEdge() != 0){
        MAIN_ERROR("Failed to configure Shard Edge");
    }

    while(1) {
        statusLed = 1;
        ThisThread::sleep_for(500);
        statusLed = 0;
        ThisThread::sleep_for(500);
    }

    return 0;
}
