/* @file Watchdog.cpp provides the interface to the Watchdog module
 *
 * This provides basic Watchdog service for the mbed. You can configure
 * various timeout intervals that meet your system needs. Additionally,
 * it is possible to identify if the Watchdog was the cause of any
 * system restart.
 *
 * Adapted from Simon's Watchdog code from http://mbed.org/forum/mbed/topic/508/
 *
 * @note Copyright &copy; 2011 by Smartware Computing, all rights reserved.
 *	This software may be used to derive new software, as long as
 *	this copyright statement remains in the source file.
 * @author David Smart
 *
 * @note Copyright &copy; 2015 by NBRemond, all rights reserved.
 *	This software may be used to derive new software, as long as
 *	this copyright statement remains in the source file.
 *
 *	Added support for STM32 Nucleo platforms
 *
 * @author Bernaérd Remond
 */

#include "watchdog.h"

#include <mbed.h>

#include <vitroio-sdk/debug.h>

using namespace vitroio::dht_demo;

#define WATCHDOG_MODULE_NAME "WATCHDOG"
#define WATCHDOG_INFO(format, ...) VITROIO_DEBUG_INFO(WATCHDOG_MODULE_NAME, format, ##__VA_ARGS__);
#define WATCHDOG_WARNING(format, ...) VITROIO_DEBUG_WARNING(WATCHDOG_MODULE_NAME, format, ##__VA_ARGS__);
#define WATCHDOG_ERROR(format, ...) VITROIO_DEBUG_ERROR(WATCHDOG_MODULE_NAME, format, ##__VA_ARGS__);

// Watchdog gets instantiated at the module level
Watchdog::Watchdog() {
	/* capture the cause of the previous reset
	 * Check if the system has resumed from IWDG reset */

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
		wdreset = true;
		/* Reset reason flags persist across reboots. Clear them after
		 * reding it */
		SET_BIT(RCC->CSR, RCC_CSR_RMVF);
	}
	else {
		wdreset = false;
	}
}

// Load timeout value in watchdog timer and enable
void Watchdog::Configure(float timeout) {
	// see http://embedded-lab.com/blog/?p=9662
	const unsigned int lsiFreq = 45000;
	uint16_t prescalerCode;
	uint16_t prescaler;
	uint16_t reloadValue;
	float calculatedTimeout;

	if ((timeout * (lsiFreq / 4)) < 0x7FF) {
		prescalerCode = IWDG_PRESCALER_4;
		prescaler = 4;
	}
	else if ((timeout * (lsiFreq / 8)) < 0xFF0) {
		prescalerCode = IWDG_PRESCALER_8;
		prescaler = 8;
	}
	else if ((timeout * (lsiFreq / 16)) < 0xFF0) {
		prescalerCode = IWDG_PRESCALER_16;
		prescaler = 16;
	}
	else if ((timeout * (lsiFreq / 32)) < 0xFF0) {
		prescalerCode = IWDG_PRESCALER_32;
		prescaler = 32;
	}
	else if ((timeout * (lsiFreq / 64)) < 0xFF0) {
		prescalerCode = IWDG_PRESCALER_64;
		prescaler = 64;
	}
	else if ((timeout * (lsiFreq / 128)) < 0xFF0) {
		prescalerCode = IWDG_PRESCALER_128;
		prescaler = 128;
	}
	else {
		prescalerCode = IWDG_PRESCALER_256;
		prescaler = 256;
	}

	/* Specifies the IWDG Reload value.
	 * This parameter must be a number between 0 and 0x0FFF. */
	reloadValue = (uint32_t)(timeout * (lsiFreq / prescaler));

	calculatedTimeout = ((float)(prescaler * reloadValue)) / lsiFreq;
	WATCHDOG_INFO("WATCHDOG set with prescaler:%d reload value: 0x%X - timeout:%f"
				,prescaler, reloadValue, calculatedTimeout);

	// Disable write protection of IWDG registers
	IWDG->KR = 0x5555;
	// Set PR value
	IWDG->PR = prescalerCode;
	// Set RLR value
	IWDG->RLR = reloadValue;
	// Reload IWDG
	IWDG->KR = 0xAAAA;
	/* Start IWDG. See more at:
	 * http://embedded-lab.com/blog/?p=9662#sthash.6VNxVSn0.dpuf */
	IWDG->KR = 0xCCCC;

	Service();
}

/* "Service", "kick" or "feed" the dog - reset the watchdog timer
 * by writing this required bit pattern */
void Watchdog::Service() {
	/* Reload IWDG. See more at:
	 * http://embedded-lab.com/blog/?p=9662#sthash.6VNxVSn0.dpuf */
	IWDG->KR = 0xAAAA;
}

// Get the flag to indicate if the watchdog causes the reset
bool Watchdog::WatchdogCausedReset() {
	return wdreset;
}
