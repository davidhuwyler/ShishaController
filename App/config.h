/*
 * config.h
 *
 *  Created on: Jun 21, 2019
 *      Author: dave
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


#define NOF_LEDS 16

#define ENABLE_SHELL 1  		//Disabling saves 4k of Flash
#define ENABLE_IMU 1
#define ENABLE_FAKE_IMU 0
#define ENABLE_EEPROM 1 		//If enabled, the device needs to be erased with ST-Link Utility to be programmed again
#define ENABLE_BATTERY_CHECK 1	//IF enabled, the LEDs only light up if the battery is good
#define USE_SW_SPI 1			//PINs PB10 / PB11 need to be configured as GPIO Output for SW I2C in CubeMX
#define ENABLE_BATTERY_SAVE_FEATURE 0

//---------------- EEPROM -----------------
// the EEPROM has space for 100 8bit values
#define EEPROM_ADDRESS_LED_MODE  0
#define EEPROM_ADDRESS_LED_BRIGHTNESS 1
#define EEPROM_ADDRESS_LED_SPEED 2
#define EEPROM_ADDRESS_LED_MODE_6_R 10
#define EEPROM_ADDRESS_LED_MODE_6_G 11
#define EEPROM_ADDRESS_LED_MODE_6_B 12

//---------------- Battery -----------------
#define BATTERY_VOLTAGE_LOW_THRESHOLD_mV 3000	//If the BatteryVolgage is lower than this Threshold, no LEDs will light up


#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H_ */
