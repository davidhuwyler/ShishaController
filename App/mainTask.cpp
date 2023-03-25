/*
 * mainTask.cpp
 *
 *  Created on: Jun 21, 2019
 *      Author: dave
 */
#include "ButtonMenu.h"
#include "LEDs.h"
#if ENABLE_IMU
#include "IMU.h"
#endif
#include "mainTask.h"
#include "cmsis_os.h"
#include "config.h"
#if ENABLE_SHELL
#include "McuShell.h"
#include "shell.h"
#endif //ENABLE_SHELL
#include "Battery.h"
#include "StatusLEDs.h"

static ButtonMenu buttonMenu; 	//!< ButtonMenu object of the ShishaController
static LEDs leds;				//!< LEDs object of the ShishaController
#if ENABLE_IMU
static IMU imu;					//!< IMU object of the ShishaController
#endif
static	Battery battery;		//!< Battery object of the ShishaController
static StatusLEDs statusLEDs;	//!< Status LED object of the ShishaController

//Is called before the scheduler runns
void mainTaskInit(void)
{
 	  battery.init();
	  leds.init();
	  buttonMenu.init(&leds,&battery);
#if ENABLE_IMU
	  imu.init();
#endif
#if ENABLE_SHELL
	  McuShell_Init();
	  shell_init();
#endif //ENABLE_SHELL
	  statusLEDs.init();
	  statusLEDs.blinkStart(StatusLEDs::statusLED_green, 1); //Start Heartbeat
}

#if ENABLE_IMU
static void mainTask_goIntoSleepAfter20sNoMotion(bool motionPresent)
{
	static uint32_t timeStamp_lastMotion;
	if(motionPresent)
	{
		timeStamp_lastMotion = osKernelSysTick();
	}

	else if((osKernelSysTick()-timeStamp_lastMotion) > 20000)
	{
		imu.setLowPowerSamplingMode(true);
		leds.setAll(leds.color(0, 0, 0), 0);
		statusLEDs.blinkStop(StatusLEDs::statusLED_red);
		statusLEDs.blinkStop(StatusLEDs::statusLED_green);
		statusLEDs.off(StatusLEDs::statusLED_green);
		statusLEDs.on(StatusLEDs::statusLED_red);
		osDelay(500);
		//HAL_PWR_EnterSTANDBYMode(); //ToDo test function and Interrupt before enable Standby...
		for(;;); //Dummy for standby mode...
		//After Wakeup -> Reset
	}

}
#endif

void mainTask(void)
{
#if ENABLE_IMU
	bool ongoingSmoke = imu.sensedActivity();

	imu.getTemp();

#if ENABLE_BATTERY_SAVE_FEATURE
	mainTask_goIntoSleepAfter20sNoMotion(ongoingSmoke);
#endif

#elif ENABLE_FAKE_IMU
	bool ongoingSmoke = true;
#else
	bool ongoingSmoke = false;
#endif


#if ENABLE_BATTERY_CHECK
	if(!battery.isGoodToRun())
	{
		statusLEDs.blinkStart(StatusLEDs::statusLED_red, 2);
		leds.setAll(leds.color(0, 0, 0), 0);
		while(!battery.isGoodToRun())
		{
			osDelay(10000);
		}
		statusLEDs.blinkStop(StatusLEDs::statusLED_red);
	}
#endif //ENABLE_BATTERY_CHECK

	if(!buttonMenu.isActive())
	{
		leds.setBrightness(buttonMenu.getLedBrightness()*31);

		switch(buttonMenu.getLedMode())
		{
		case 1:
			leds.ledMode_1(ongoingSmoke);
			break;
		case 2:
			leds.ledMode_2(ongoingSmoke);
			break;
		case 3:
			leds.ledMode_3(ongoingSmoke);
			break;
		case 4:
			leds.ledMode_4(ongoingSmoke);
			break;
		case 5:
			leds.ledMode_5();
			break;
		case 6:
			leds.ledMode_6(&buttonMenu);
			break;
#if ENABLE_IMU
		case 7:
			leds.ledMode_7(imu.getTemp());
#endif
			break;
		case 8:
			leds.ledMode_8(battery.getRemainingCapacityPercentage());
			break;
		default:
			leds.setAll(leds.color(255, 00, 00), 200);
			leds.setAll(leds.color(255, 255, 255), 200);
		}
	}
#if ENABLE_FAKE_IMU
	osDelay((9-buttonMenu.getLedSpeed())*200);
#else
	osDelay((9-buttonMenu.getLedSpeed())*20);
#endif
}

ButtonMenu* mainTask_getButtonMenu(void)
{
	return &buttonMenu;
}
