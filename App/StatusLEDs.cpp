/*
 * StatusLEDs.cpp
 *
 *  Created on: Jul 3, 2019
 *      Author: dhuwiler
 */

#include <StatusLEDs.h>

#define STATUS_LED_RED_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
#define STATUS_LED_RED_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)

#define STATUS_LED_GREEN_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define STATUS_LED_GREEN_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)

#define STATUS_LED_RED_TOGGLE HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15)
#define STATUS_LED_GREEN_TOGGLE HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14)

static  bool blinkRedStatusLed = false;
static	bool blinkGreenStatusLed = false;
static	bool durationOnStatusRedLed = false;
static	bool durationOnStatusGreenLed = false;

StatusLEDs::StatusLEDs() {}
StatusLEDs::~StatusLEDs() {}

void statusLEDblinkTimerCallback(void const * argument)
{
	if(blinkRedStatusLed)
	{
		STATUS_LED_RED_TOGGLE;
	}

	if(blinkGreenStatusLed)
	{
		STATUS_LED_GREEN_TOGGLE;
	}
}

void statusLEDdurationTimerCallback(void const * argument)
{
	if(durationOnStatusRedLed)
	{
		STATUS_LED_RED_OFF;
		durationOnStatusRedLed = false;
	}

	if(durationOnStatusGreenLed)
	{
		STATUS_LED_GREEN_OFF;
		durationOnStatusGreenLed = false;
	}
}

/*! \fn StatusLEDs::init
 *  \brief Initializes the StatusLEDs
*/
void StatusLEDs::init(void)
{
	  osTimerDef(StatusLEDblinkTimer, statusLEDblinkTimerCallback);
	  StatusLEDblinkTimerHandle = osTimerCreate(osTimer(StatusLEDblinkTimer), osTimerPeriodic, NULL);
	  osTimerDef(StatusLEDdurationTimer, statusLEDdurationTimerCallback);
	  StatusLEDdurationTimerHandle = osTimerCreate(osTimer(StatusLEDdurationTimer), osTimerOnce, NULL);
}

/*! \fn StatusLEDs::blinkStart
 *  \brief Starts LED blinking
 *  \param ledColor led color to start blinking
 *  \param frequency_hz blink frequency
*/
void StatusLEDs::blinkStart(ledColor_e ledColor, uint8_t frequency_hz)
{
	osTimerStart(StatusLEDblinkTimerHandle, 500/frequency_hz);
	if(ledColor == statusLED_red)
	{
		blinkRedStatusLed = true;
	}
	else if(ledColor == statusLED_green)
	{
		blinkGreenStatusLed = true;
	}
}

/*! \fn StatusLEDs::blinkStop
 *  \brief Stops LED blinking
 *  \param ledColor led color to stop blinking
*/
void StatusLEDs::blinkStop(ledColor_e ledColor)
{
	osTimerStop(StatusLEDblinkTimerHandle);
	if(ledColor == statusLED_red)
	{
		STATUS_LED_RED_OFF;
		blinkRedStatusLed = false;
	}
	else if(ledColor == statusLED_green)
	{
		STATUS_LED_GREEN_OFF;
		blinkGreenStatusLed = false;
	}
}

/*! \fn StatusLEDs::on
 *  \brief Lights up the specified LED
 *  \param ledColor led color to light up
*/
void StatusLEDs::on(ledColor_e ledColor)
{
	if(ledColor == statusLED_red)
	{
		STATUS_LED_RED_ON;
	}
	else if(ledColor == statusLED_green)
	{
		STATUS_LED_GREEN_ON;
	}
}

/*! \fn StatusLEDs::on
 *  \brief Lights up the specified LED for a period of time
 *  \param ledColor led color to light up
 *  \param time period (ms) to light up the LED
*/
void StatusLEDs::onDuration(ledColor_e ledColor,uint16_t duration_ms)
{
	osTimerStart(StatusLEDdurationTimerHandle, duration_ms);
	if(ledColor == statusLED_red)
	{
		STATUS_LED_RED_ON;
		durationOnStatusRedLed = true;
	}
	else if(ledColor == statusLED_green)
	{
		STATUS_LED_GREEN_ON;
		durationOnStatusGreenLed = true;
	}
}

/*! \fn StatusLEDs::off
 *  \brief Switches a LED of
 *  \param ledColor led color to switch off
*/
void StatusLEDs::off(ledColor_e ledColor)
{
	if(ledColor == statusLED_red)
	{
		STATUS_LED_RED_OFF;
		durationOnStatusRedLed = false;
		blinkRedStatusLed = false;
	}
	else if(ledColor == statusLED_green)
	{
		STATUS_LED_GREEN_OFF;
		durationOnStatusGreenLed = false;
		blinkGreenStatusLed = false;
	}
}
