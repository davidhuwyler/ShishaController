/*!
 * \class StatusLED
 * \brief Controls the red/green StatusLED of the ShishaController
 *
 *
 *  \author David Huwiler
 *  \version 1.0
 *  \date 03.07.2019
 */

#ifndef STATUSLEDS_H_
#define STATUSLEDS_H_

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"
#if ENABLE_SHELL
#include "McuShell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#endif
#ifdef __cplusplus
}
#endif

class StatusLEDs {
public:
	StatusLEDs();

	typedef enum
	{
		statusLED_red = GPIO_PIN_15,
		statusLED_green = GPIO_PIN_14
	}ledColor_e;

	void init(void);
	void blinkStart(ledColor_e ledColor, uint8_t frequency_hz);
	void blinkStop(ledColor_e ledColor);
	void on(ledColor_e ledColor);
	void onDuration(ledColor_e ledColor,uint16_t duration_ms);
	void off(ledColor_e ledColor);

	virtual ~StatusLEDs();

private:
	osTimerId StatusLEDblinkTimerHandle;
	osTimerId StatusLEDdurationTimerHandle;
};

#endif /* STATUSLEDS_H_ */
