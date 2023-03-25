/*!
 * \class ButtonMenu
 * \brief ButtonMenu handles the Menu UserInterface
 *
 *  The task() member function needs to be called by the FreeRTOS task
 *
 *  The Menu can be activated by pressing the MenuButton and the ParamButton
 *  for one second simultaneously.
 *
 *  Now by pressing the MenuButton you can cycle through the MenuOptions.
 *  By pressing the ParamButton you can change the setting for the selected MenuOption
 *
 *  The LEDs Show the current Menu (Left LEDs) and the current Parameter (Right LEDs)
 *  in red color:
 *
 *
 *                 Menu1    Param1
 *
 *		     Menu2                 Param2
 *
 *		   Menu3                      Param3
 *
 *		 Menu4                         Param4
 *
 *		 Menu5                         Param5
 *
 *		  Menu6                       Param6
 *
 *		     Menu7                 Param7
 *
 *		           Menu8     Param8
 *
 *
 *  The Menu options are these:
 *
 *  -#	Set the LED mode
 *  -#	Set the LED brightness
 *  -#	Set the LED speed
 *
 *
 *  LED Modes:
 *
 *  -# Color circle as long as smoke is present
 *  -# Color fade as long as smoke is present
 *  -# Color fade
 *  -# Color circle
 *  -# Static white color
 *  -# Static color of choice (choose with buttons: menu to select color (r,g or b) param to change selected color)
 *  -# Thermometer (count the red LEDs to get the Temperature over 20°C)
 *  -# Battery gauge (remaining battery capacity)
 *
 *  \author David Huwiler
 *  \version 1.0
 *  \date 02.07.2019
 */

#ifndef BUTTONMENU_H_
#define BUTTONMENU_H_

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

#define NOF_SETTINGS 3
#define NOF_MODES	8 //1...NOF_MODES
#define NOF_BRIGHTNESS_LEVELS 8 //1...NOF_BRIGHTNESS_LEVELS
#define NOF_SPEED_LEVELS 8 //1...NOF_SPEED_LEVELS

#define DEFAULT_MODE 1
#define DEFAULT_BRIGHTNESS 1
#define DEFAULT_SPEED 3

class LEDs;
class Battery;

class ButtonMenu {
public:
	ButtonMenu();

	void init(LEDs* ledsParam, Battery* battParam);
	void task(void);
	uint8_t isActive(void);
	uint8_t getLedMode(void);
	uint8_t getLedBrightness(void);
	uint8_t getLedSpeed(void);

	void buttonEdgeDetection(bool* menuEdge,bool* paramEdge);
	void setActive(bool setActive);
	void setLedMode(uint8_t mode);
	void setLedBrightness(uint8_t brightness);
	void setLedSpeed(uint8_t speed);
	virtual ~ButtonMenu();


private:
	typedef enum
	{
		setting_led_mode = 1,
		setting_led_bringhtness = 2,
		setting_led_speed = 3

	}menuSettings_e;

	void displayMenu(uint8_t menuValue, uint8_t paramValue);
	LEDs* leds;
	Battery* battery;
	bool menuIsActivated = false;
	bool menuWasActivated = false;
	uint8_t currentMenu = 1;
	uint8_t currentParams[NOF_MODES] = {DEFAULT_MODE,DEFAULT_BRIGHTNESS,DEFAULT_SPEED};
	uint8_t maxParams[NOF_MODES] = {NOF_MODES, NOF_BRIGHTNESS_LEVELS, NOF_SPEED_LEVELS};
};


#if ENABLE_SHELL
uint8_t buttonMenu_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);
#endif

#endif /* BUTTONMENU_H_ */
