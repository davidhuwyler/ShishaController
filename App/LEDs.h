/*!
 * \class LEDs
 * \brief LEDs controls the WS2812B LEDs of the ShishaController
 *
 *  Class to control the leds on the ShishaController
 *  There are 16 WS2812b leds placed on the pcb.
 *  They can be individually addressed.
 *
 *   LED Addresses:
 *	 <pre>
 *                 8      7
 *
 *		      9                  6
 *
 *		  10                          5
 *
 *		 11                            4
 *
 *		 12                            3
 *
 *		  13                          2
 *
 *		      14                 1
 *
 *		           15     0
 *   </pre>
 *
 *
 *  \author David Huwiler
 *  \version 1.0
 *  \date 02.07.2019
 */

#ifndef LEDS_H_
#define LEDS_H_

class ButtonMenu;
#include "WS2812B.h"

#include "config.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"
}

#define LED_IDLE_COLOR {255, 255, 255}
#define LED_MODE_1_NOF_LEVELS 6
#define LED_MODE_2_INCREMENTS_PER_STEP 16
#define LED_MODE_2_NOF_LEVELS 6
#define LED_MODE_3_STEPS_PER_LEVEL 16
#define LED_MODE_3_NOF_LEVELS 7
#define LED_MODE_4_NOF_LEVELS 7
#define LED_MODE_6_COLOR_INCREMENTS 30

class LEDs {
public:
	LEDs();
	void init(void);
	void setBrightness(uint8_t brightness);
	void setAll(uint32_t color, uint16_t waitDurationBetweenLEDsMs);
	void setLEDs(uint32_t color,uint8_t firstLEDtoApply, uint8_t lastLEDtoApply);
	uint32_t color(uint8_t r, uint8_t g, uint8_t b);

	void ledMode_1(bool ongoingSmoke);
	void ledMode_2(bool ongoingSmoke);
	void ledMode_3(bool ongoingSmoke);
	void ledMode_4(bool ongoingSmoke);
	void ledMode_5();
	void ledMode_6(ButtonMenu* buttonMenu);
	void ledMode_6_setColor(uint8_t r, uint8_t g, uint8_t b);
	void ledMode_6_getColor(uint8_t* r, uint8_t* g, uint8_t* b);
	void ledMode_7(uint8_t temp);
	void ledMode_8(uint16_t remainingBatteryCapacityPercentage);

	virtual ~LEDs();

private:
	WS2812B leds = WS2812B(NOF_LEDS);
	uint8_t ledMode_1_levelColors[LED_MODE_1_NOF_LEVELS][3] =
	{
			LED_IDLE_COLOR,
			{0, 0, 255},
			{255, 0, 100},
			{244, 241, 0},
			{244,100,0},
			{255,0,0}
	};
	uint8_t ledMode_2_levelColors[LED_MODE_2_NOF_LEVELS][3] =
	{
			LED_IDLE_COLOR,
			{0, 0, 255},
			{255, 0, 100},
			{244, 241, 0},
			{244,100,0},
			{255,0,0}
	};
	uint8_t ledMode_3_levelColors[LED_MODE_3_NOF_LEVELS][3] =
	{
			{0,0,255},
			{0,255,255},
			{0,255,0},
			{255,255,0},
			{255,0,0},
			{255,0,255},
			{0,0,255},
	};
	uint8_t ledMode_4_levelColors[LED_MODE_4_NOF_LEVELS][3] =
	{
			{0,0,255},
			{0,255,255},
			{0,255,0},
			{255,255,0},
			{255,0,0},
			{255,0,255},
			{0,0,255},
	};
	uint8_t ledMode_5_staticColor[3] ={255,255,255};
	uint8_t ledMode_6_color[3] ={255,255,255};
};


#endif /* LEDS_H_ */
