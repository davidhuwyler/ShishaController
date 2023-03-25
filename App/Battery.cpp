/*
 * Battery.cpp
 *
 *  Created on: Jul 3, 2019
 *      Author: dhuwiler
 */

#include "Battery.h"
#include "adc.h"

static void battery_initShell(Battery* batt);

Battery::Battery() {}

Battery::~Battery() {}


/*! \fn Battery::init
 *  \brief Initializes the Battery
*/
void Battery::init(void)
{
#if ENABLE_SHELL
	battery_initShell(this);
#endif
}

/*! \fn Battery::getVoltage_mV
 *  \brief Samples the Batteryvoltage
 *
 *  \return Battery voltage in mV
*/
uint16_t Battery::getVoltage_mV(void)
{
	HAL_ADC_Start(&hadc1);
	while(HAL_ADC_PollForConversion(&hadc1, 20) != HAL_OK){}
	return (uint16_t)(((uint32_t)(HAL_ADC_GetValue(&hadc1)*3300*2))/4095);
}


uint8_t Battery::getRemainingCapacityPercentage(void)
{
	uint16_t battVolt_mV = getVoltage_mV();
	uint8_t battVoltRemainder;
	uint16_t voltageIndex;
	uint8_t percentage;
	uint8_t percentageRangeInVoltageRange;
	if(battVolt_mV>=capacityLookupTable[10][0]) //Voltage > 4200mV
	{
		return 100;
	}
	if(battVolt_mV<=capacityLookupTable[0][0]) //Voltage > 3300mV
	{
		return 0;
	}

	voltageIndex = (battVolt_mV-capacityLookupTable[0][0])/100;
	percentage = capacityLookupTable[voltageIndex][1];
	battVoltRemainder = battVolt_mV - capacityLookupTable[voltageIndex][0];
	percentageRangeInVoltageRange = capacityLookupTable[voltageIndex+1][1]-percentage;

	return percentage +( battVoltRemainder * percentageRangeInVoltageRange ) / 100;
}

/*! \fn Battery::isGoodToRun
 *  \brief Decides if the battery ha enough charge to run the device
 *
 *  \return true if there is enough charge to run the device
*/
bool Battery::isGoodToRun(void)
{
	return (bool)(BATTERY_VOLTAGE_LOW_THRESHOLD_mV <= getVoltage_mV());
}

//---------------------------------------------------------------------------------------------------
//Shell..
#if ENABLE_SHELL
// The following functions provide the shell with data (c interface for the shell)
Battery* battery;

static void battery_initShell(Battery* batt)
{
	battery = batt;
}
static uint16_t battery_getVoltage_mV(void)
{
	return battery->getVoltage_mV();
}
static bool battery_isGoodToRun(void)
{
	return battery->isGoodToRun();
}


static uint8_t PrintStatus(McuShell_ConstStdIOType *io)
{
  McuShell_SendStatusStr((unsigned char*)"Battery", (const unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t battery_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io)
{
	  uint8_t res = ERR_OK;

	  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "Battery help")==0)
	  {
	    McuShell_SendHelpStr((unsigned char*)"Battery", (const unsigned char*)"Group of ButtonMenu commands\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  help|status", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  getVoltage", (const unsigned char*)"Battery Voltage in mV\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  isGoodToRun", (const unsigned char*)"Enough Voltage to run the Device?\r\n", io->stdOut);
	    *handled = TRUE;
	    return ERR_OK;
	  }
	  else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "RTC status")==0))
	  {
	    *handled = TRUE;
	    return PrintStatus(io);
	  }
	  else if (McuUtility_strncmp((char*)cmd, "Battery getVoltage", sizeof("Battery getVoltage")-1)==0)
	  {
		  McuShell_SendStr((uint8_t*)"Battery Voltage: ",io->stdOut);
		  McuShell_SendNum16u(battery_getVoltage_mV(),io->stdOut);
		  McuShell_SendStr((uint8_t*)" mV\r\n",io->stdOut);
	      *handled = TRUE;
	  }
	  else if (McuUtility_strncmp((char*)cmd, "Battery isGoodToRun", sizeof("Battery isGoodToRun")-1)==0)
	  {
		  if(battery_isGoodToRun())
		  {
			  McuShell_SendStr((uint8_t*)"Battery Voltage is ok",io->stdOut);
		  }
		  else
		  {
			  McuShell_SendStr((uint8_t*)"Battery Voltage is too low",io->stdOut);
		  }
	  }
	  return res;
}
#endif
