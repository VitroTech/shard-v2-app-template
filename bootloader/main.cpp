#include <mbed.h>

#include <vitroio-sdk/bootloader/bootloader.h>
#include <vitroio-sdk/types.h>

#include <watchdog.h>
#include <global_consts.h>

using namespace vitroio::sdk;
using namespace vitroio::dht_demo;

Watchdog wdt;
Ticker wdtKicker;

Serial pc(PA_2, PA_3, 115200);
DigitalOut statusLed(VITROIO_DHT_DEMO_STATUS_LED_PIN);

int main()
{
	statusLed = true;

	wdt.Configure(10.0);
	wdtKicker.attach(callback(&wdt, &Watchdog::Service), 2.0);

	vitroioBoot(
		FlashSpace(
			VITROIO_DHT_DEMO_ENVIRONMENT_FLASH_OFFSET, 
			VITROIO_DHT_DEMO_ENVIRONMENT_FLASH_SECTORS),
		FlashSpace(
			VITROIO_DHT_DEMO_REGION_A_FLASH_OFFSET, 
			VITROIO_DHT_DEMO_REGION_FLASH_SECTORS),
		FlashSpace(
			VITROIO_DHT_DEMO_REGION_B_FLASH_OFFSET, 
			VITROIO_DHT_DEMO_REGION_FLASH_SECTORS),
		wdt.WatchdogCausedReset() ? VITROIO_RESTART_REASON_WDT : VITROIO_RESTART_REASON_SW);
}
