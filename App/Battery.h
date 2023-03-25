/*!
 * \class Battery
 * \brief The Battery class samples the Battery
 *
 *  To the ADC1 (PA1) Port is the following Voltage divider connected
 *  <pre>
 *
 *  U_battery
 *  --------
 *     |
 *     |
 *    ---
 *    | | R6=51k
 *    | |
 *    ---
 *     |
 *     |----------> U_BAT/2 -->PA1
 *     |
 *    ---
 *    | | R7=51k
 *    | |
 *    ---
 *     |
 *     |
 *  --------
 *    GND
 *
 * </pre>
 *
 * The Voltage is sampled and compared to a threshold (BATTERY_VOLTAGE_LOW_THRESHOLD_mV)
 *
 *  \author David Huwiler
 *  \version 1.0
 *  \date 03.07.2019
 */


#ifndef BATTERY_H_
#define BATTERY_H_

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"
#if ENABLE_SHELL
#include "McuShell.h"
#endif
#ifdef __cplusplus
}
#endif


class Battery {
public:
	Battery();
	void init(void);
	uint16_t getVoltage_mV(void);
	bool isGoodToRun(void);
	uint8_t getRemainingCapacityPercentage(void);
	virtual ~Battery();

private:
	uint16_t capacityLookupTable[11][2] = //Lookuptable [BatteryVoltage mV][Capacity %]
	{
			{3300, 0}, //Index 0
			{3400, 6},
			{3500, 15},
			{3600, 33},
			{3700, 50},
			{3800, 59},
			{3900, 72},
			{4000, 83},
			{4100, 94},
			{4200, 100}, //Index 9
			{4200, 100} //Index 10
	};
};

uint8_t battery_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);

#endif /* BATTERY_H_ */
