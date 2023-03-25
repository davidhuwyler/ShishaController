#include "LEDs.h"
#include "ButtonMenu.h"
#include "cmsis_os.h"

LEDs::LEDs(){}

LEDs::~LEDs(){}

/*! \fn LEDs::init
 *  \brief Initializes the LEDs
*/
void LEDs::init(void)
{
	leds.begin();// Sets up the SPI
	leds.show();// Clears the strip, as by default the strip data is set to all LED's off.
}

/*! \fn LEDs::setBrightness
 *  \brief Sets the LED brightness
 *  \param brightness 0(lowest)..255(higest) brightness to set
*/
void LEDs::setBrightness(uint8_t brightness)
{
	leds.setBrightness(brightness);
}

/*! \fn LEDs::setAll
 *  \brief Sets all LEDs to a specific Color (LEDs::color)
 *  \param color LEDs::color
 *  \param waitDurationBetweenLEDsMs if not 0, the leds are not all switched immediately to the color
*/
void LEDs::setAll(uint32_t color, uint16_t waitDurationBetweenLEDsMs)
{
	for(uint16_t i=0; i<leds.numPixels(); i++)
	{
	  leds.setPixelColor(i, color);
	  leds.show();
	  if(waitDurationBetweenLEDsMs)
		  osDelay(waitDurationBetweenLEDsMs);
	}
}

/*! \fn LEDs::setLEDs
 *  \brief Sets the specified LEDs to a new Color (LEDs::color)
 *
 *  The leds are addressed from 0 to NOF_LEDS-1
 *
 *  \param color LEDs::color
 *  \param firstLEDtoApply led address of the first LED to apply the color
 *  \param lastLEDtoApply led address of the last LED to apply the color
*/
void LEDs::setLEDs(uint32_t color,uint8_t firstLEDtoApply, uint8_t lastLEDtoApply)
{
	if(lastLEDtoApply>leds.numPixels()-1)
	{
		lastLEDtoApply = leds.numPixels()-1;
	}
	for(uint16_t i=firstLEDtoApply ; i< lastLEDtoApply+1; i++)
	{
	  leds.setPixelColor(i, color);
	}
	leds.show();
}

/*! \fn LEDs::color
 *  \brief Used to create a 32bit representation of 3 uint8_t (r,g,b)
 *  \param r red color part (0..255)
 *  \param g green color part (0..255)
 *  \param b blue color part (0..255)
*/
uint32_t LEDs::color(uint8_t r, uint8_t g, uint8_t b)
{
	return leds.Color(r, g, b);
}

/*! \fn LEDs::ledMode_1
 *  \brief LED Mode 1: Color circle as long as smoke is present
 *
 *  Every time this function is called, the LEDs are updated with
 *  the next step in this color Mode
 *
 *  \param ongoingSmoke weather or not the user currently is smoking
*/
void LEDs::ledMode_1(bool ongoingSmoke)
{
	static uint8_t nofLedsInLevel = 0;
	static uint8_t currentLevel = 0;

	if(ongoingSmoke)
	{

		if(nofLedsInLevel<NOF_LEDS)
		{
			nofLedsInLevel++;
		}

		if(nofLedsInLevel >= NOF_LEDS)
		{
			if(currentLevel<LED_MODE_1_NOF_LEVELS-2)
			{
				nofLedsInLevel = 0;
				currentLevel++;
			}
		}
	}
	else
	{
		if(nofLedsInLevel)
			nofLedsInLevel--;
		if(nofLedsInLevel)
			nofLedsInLevel--;

		else if(currentLevel)
		{
			nofLedsInLevel = NOF_LEDS-1;
			if(currentLevel)
			{
				currentLevel--;
			}
		}
	}
	setLEDs(color(ledMode_1_levelColors[currentLevel][0], ledMode_1_levelColors[currentLevel][1], ledMode_1_levelColors[currentLevel][2]), nofLedsInLevel+1, NOF_LEDS-1);
	setLEDs(color(ledMode_1_levelColors[currentLevel+1][0], ledMode_1_levelColors[currentLevel+1][1], ledMode_1_levelColors[currentLevel+1][2]), 0, nofLedsInLevel);
}

/*! \fn LEDs::ledMode_2
 *  \brief LED Mode 2: Color fade as long as smoke is present
 *
 *  Every time this function is called, the LEDs are updated with
 *  the next step in this color Mode
 *
 *  \param ongoingSmoke weather or not the user currently is smoking
*/
void LEDs::ledMode_2(bool ongoingSmoke)
{
	static uint8_t currentLevel = 0;
	static uint8_t stepInLevel = 0;

	if(ongoingSmoke)
	{
		stepInLevel++;
		if(stepInLevel >= NOF_LEDS)
		{
			stepInLevel = 0;
			if(currentLevel<LED_MODE_2_NOF_LEVELS-2)
			{
				currentLevel++;
			}
		}
	}
	else
	{
		if(stepInLevel)
			stepInLevel--;
		if(stepInLevel)
			stepInLevel--;
		else if(currentLevel)
		{
			stepInLevel = LED_MODE_2_INCREMENTS_PER_STEP;
			if(currentLevel)
			{
				currentLevel--;
			}
		}
	}

	uint8_t r = ledMode_2_levelColors[currentLevel][0]+
			(stepInLevel * (ledMode_2_levelColors[currentLevel+1][0]-ledMode_2_levelColors[currentLevel][0]))
			/LED_MODE_2_INCREMENTS_PER_STEP;

	uint8_t g = ledMode_2_levelColors[currentLevel][1]+
			(stepInLevel * (ledMode_2_levelColors[currentLevel+1][1]-ledMode_2_levelColors[currentLevel][1]))
			/LED_MODE_2_INCREMENTS_PER_STEP;

	uint8_t b = ledMode_2_levelColors[currentLevel][2]+
			(stepInLevel * (ledMode_2_levelColors[currentLevel+1][2]-ledMode_2_levelColors[currentLevel][2]))
			/LED_MODE_2_INCREMENTS_PER_STEP;

	setLEDs(color(r,g,b), 0, NOF_LEDS-1);
}

/*! \fn LEDs::ledMode_3
 *  \brief LED Mode 3: Color fade
 *
 *  Every time this function is called, the LEDs are updated with
 *  the next step in this color Mode
 *
 *  \param ongoingSmoke weather or not the user currently is smoking
*/
void LEDs::ledMode_3(bool ongoingSmoke)
{
	static uint8_t currentLevel = 0;
	static uint8_t stepInLevel = 0;

	if(ongoingSmoke)
	{
		stepInLevel++;
		if(stepInLevel >= LED_MODE_3_STEPS_PER_LEVEL)
		{
			stepInLevel = 0;
			if(currentLevel<LED_MODE_3_NOF_LEVELS-2)
			{
				currentLevel++;
			}
			else
			{
				currentLevel = 0;
			}
		}
	}

	uint8_t r = ledMode_3_levelColors[currentLevel][0]+
			(stepInLevel * (ledMode_3_levelColors[currentLevel+1][0]-ledMode_3_levelColors[currentLevel][0]))
			/LED_MODE_3_STEPS_PER_LEVEL;

	uint8_t g = ledMode_3_levelColors[currentLevel][1]+
			(stepInLevel * (ledMode_3_levelColors[currentLevel+1][1]-ledMode_3_levelColors[currentLevel][1]))
			/LED_MODE_3_STEPS_PER_LEVEL;

	uint8_t b = ledMode_3_levelColors[currentLevel][2]+
			(stepInLevel * (ledMode_3_levelColors[currentLevel+1][2]-ledMode_3_levelColors[currentLevel][2]))
			/LED_MODE_3_STEPS_PER_LEVEL;

	setLEDs(color(r,g,b), 0, NOF_LEDS-1);
}

/*! \fn LEDs::ledMode_4
 *  \brief LED Mode 4: Color circle
 *
 *  Every time this function is called, the LEDs are updated with
 *  the next step in this color Mode
 *
 *  \param ongoingSmoke weather or not the user currently is smoking
*/
void LEDs::ledMode_4(bool ongoingSmoke)
{
	static uint8_t nofLedsInLevel = 0;
	static uint8_t currentLevel = 0;

	if(ongoingSmoke)
	{
		nofLedsInLevel++;
		if(nofLedsInLevel >= NOF_LEDS)
		{
			nofLedsInLevel = 0;
			if(currentLevel<LED_MODE_4_NOF_LEVELS-2)
			{
				currentLevel++;
			}
			else
			{
				currentLevel = 0;
			}
		}
	}
	setLEDs(color(ledMode_4_levelColors[currentLevel][0], ledMode_4_levelColors[currentLevel][1], ledMode_4_levelColors[currentLevel][2]), nofLedsInLevel+1, NOF_LEDS-1);
	setLEDs(color(ledMode_4_levelColors[currentLevel+1][0], ledMode_4_levelColors[currentLevel+1][1], ledMode_4_levelColors[currentLevel+1][2]), 0, nofLedsInLevel);

}

/*! \fn LEDs::ledMode_5
 *  \brief LED Mode 5: Static white color
 *
 *  Every time this function is called, the LEDs are updated with
 *  the next step in this color Mode
*/
void LEDs::ledMode_5()
{
	setLEDs(color(ledMode_5_staticColor[0], ledMode_5_staticColor[1], ledMode_5_staticColor[2]), 0, NOF_LEDS-1);
}

/*! \fn LEDs::ledMode_6
 *  \brief LED Mode 6: Static color of choice
 *
 *  Every time this function is called, the LEDs are updated with
 *  the next step in this color Mode
 *
 *  The buttons are used to change the color
 *  menuButton to select color (r,g or b) paramButton to change selected color
 *
 *  \param buttonMenu buttonmenu object to obtain the button states
*/
void LEDs::ledMode_6(ButtonMenu* buttonMenu)
{
	static uint8_t activeColor = 0; //0=r, 1=g, 2=b

	bool menuButtonPosEdge, paramButtonPosEdge;
	buttonMenu->buttonEdgeDetection(&menuButtonPosEdge,&paramButtonPosEdge);

	if(menuButtonPosEdge)
	{
		activeColor++;
		activeColor%=3;
	}

	if(paramButtonPosEdge)
	{
		if((uint16_t)ledMode_6_color[activeColor]+LED_MODE_6_COLOR_INCREMENTS <= 255)
		{
			ledMode_6_color[activeColor]+=LED_MODE_6_COLOR_INCREMENTS;
		}
		else
		{
			ledMode_6_color[activeColor] = 0;
		}
	}
	setLEDs(color(ledMode_6_color[0], ledMode_6_color[1], ledMode_6_color[2]), 0, NOF_LEDS-1);
}

void LEDs::ledMode_6_setColor(uint8_t r, uint8_t g, uint8_t b)
{
	ledMode_6_color[0] = r;
	ledMode_6_color[1] = g;
	ledMode_6_color[2] = b;
}

void LEDs::ledMode_6_getColor(uint8_t* r, uint8_t* g, uint8_t* b)
{
	*r = ledMode_6_color[0];
	*g = ledMode_6_color[1];
	*b = ledMode_6_color[2];
}

/*! \fn LEDs::ledMode_7
 *  \brief LED Mode 7: Thermometer
 *
 *  Mode displays the Temperature above 20°C
 *
 *  \param temp temperature in °C
*/
void LEDs::ledMode_7(uint8_t temp)
{
	uint8_t tempAbove20Degree = temp - 20;
	setLEDs(color(255,0,50), 0,tempAbove20Degree-1);
	setLEDs(color(0,0,0), tempAbove20Degree, NOF_LEDS-1);
}

/*! \fn LEDs::ledMode_8
 *  \brief LED Mode 8: Battery Gauge
 *
 *  Mode displays the remaining Battery Capacity
 *
 *  \param remainingBatteryCapacityPercentage remaining Battery capacity in percent
*/
void LEDs::ledMode_8(uint16_t remainingBatteryCapacityPercentage)
{
	static uint8_t ledPercentageOld1, ledPercentageOld2, ledPercentageOld3;
	uint8_t ledPercentage = ( remainingBatteryCapacityPercentage * (NOF_LEDS) ) / 100;

	uint8_t ledMeanPercentage = (ledPercentage+ledPercentageOld1+ledPercentageOld2+ledPercentageOld3)/4;

	setLEDs(color(255,0,25*(remainingBatteryCapacityPercentage/10)), 0,ledMeanPercentage);
	if(ledPercentage<NOF_LEDS-1)
	{
		setLEDs(color(0,0,0), ledPercentage+1, NOF_LEDS-1);
	}
	ledPercentageOld3 = ledPercentageOld2;
	ledPercentageOld2 = ledPercentageOld1;
	ledPercentageOld1 = ledPercentage;

}
