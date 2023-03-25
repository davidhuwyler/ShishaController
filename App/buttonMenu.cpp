/*
 * ButtonMenu.cpp
 *
 *  Created on: 02.07.2019
 *      Author: dhuwiler
 */

#include "ButtonMenu.h"
#include "LEDs.h"
#include "Battery.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "config.h"
#if ENABLE_EEPROM
#include "eeprom.h"
#endif
#if ENABLE_SHELL
#include "shell.h"
#endif

#define BTN_MENU_IS_PRESSED !(bool)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)
#define BTN_PARAM_IS_PRESSED !(bool)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)

#define TASK_DELAY_MS 100

static void buttonMenu_initShell(ButtonMenu* bM);

ButtonMenu::ButtonMenu() {}
ButtonMenu::~ButtonMenu() {}

/*! \fn ButtonMenu::init
 *  \brief Initializes the ButtonMenu
 *
 *  \param ledsParam the LEDs object to display the menu on
*/
void ButtonMenu::init(LEDs* ledsParam, Battery* battParam)
{
	leds = ledsParam;
	battery = battParam;
	bool eepromReadNotSuccessful = false;

#if ENABLE_EEPROM
	eeprom_init();
	//Format EEPROM if both buttons are Pressed at startup
	if(BTN_MENU_IS_PRESSED && BTN_PARAM_IS_PRESSED)
	{
		eeprom_format();
	}
	eepromReadNotSuccessful|=eeprom_read8(EEPROM_ADDRESS_LED_MODE, &currentParams[setting_led_mode-1]);
	eepromReadNotSuccessful|=eeprom_read8(EEPROM_ADDRESS_LED_BRIGHTNESS, &currentParams[setting_led_bringhtness-1]);
	eepromReadNotSuccessful|=eeprom_read8(EEPROM_ADDRESS_LED_SPEED, &currentParams[setting_led_speed-1]);

	uint8_t r, g, b;
	eepromReadNotSuccessful|=eeprom_read8(EEPROM_ADDRESS_LED_MODE_6_R, &r);
	eepromReadNotSuccessful|=eeprom_read8(EEPROM_ADDRESS_LED_MODE_6_G, &g);
	eepromReadNotSuccessful|=eeprom_read8(EEPROM_ADDRESS_LED_MODE_6_B, &b);
	leds->ledMode_6_setColor(r, g, b);
#endif //ENABLE_EEPROM

	//EEPROM read failed or EEPROM is disabled... use default values
	if(eepromReadNotSuccessful)
	{
		currentParams[setting_led_mode-1] = DEFAULT_MODE;
		currentParams[setting_led_bringhtness-1] = DEFAULT_BRIGHTNESS;
		currentParams[setting_led_speed-1] = DEFAULT_SPEED;
	}

#if ENABLE_SHELL
	buttonMenu_initShell(this);
#endif
}


/*! \fn ButtonMenu::task
 *  \brief FreeRTOS task of the ButtonMenu
*/
void ButtonMenu::task(void)
{
#if ENABLE_SHELL
	shell_run();
#endif //ENABLE_SHELL
#if ENABLE_BATTERY_CHECK
	if(!battery->isGoodToRun())
	{
		leds->setAll(leds->color(0, 0, 0), 0);
		while(!battery->isGoodToRun())
		{
			osDelay(10000);
		}
	}
#endif //ENABLE_BATTERY_CHECK



	if(BTN_MENU_IS_PRESSED && BTN_PARAM_IS_PRESSED)
	{
		osDelay(1000);
		if(BTN_MENU_IS_PRESSED && BTN_PARAM_IS_PRESSED)
		{
			menuIsActivated = !menuIsActivated;
		}

	}
	if(menuIsActivated)
	{
		bool menuButtonPosEdge, paramButtonPosEdge;
		buttonEdgeDetection(&menuButtonPosEdge,&paramButtonPosEdge);
		if(menuButtonPosEdge)
		{
			currentMenu++;
			if(currentMenu>NOF_SETTINGS)
			{
				currentMenu = 1;
			}
		}
		if(paramButtonPosEdge)
		{
			currentParams[currentMenu-1]++;
			if(currentParams[currentMenu-1]>maxParams[currentMenu-1])
			{
				currentParams[currentMenu-1] = 1;
			}
		}

		displayMenu(currentMenu,currentParams[currentMenu-1]);
	}

	//Edge on menuIsActivated
	if(menuIsActivated!=menuWasActivated)
	{
		if(menuIsActivated) //Pos Edge
		{
			leds->setBrightness(30);
		}
		else
		{
#if ENABLE_EEPROM
			eeprom_write8(EEPROM_ADDRESS_LED_MODE, currentParams[setting_led_mode-1]);
			eeprom_write8(EEPROM_ADDRESS_LED_BRIGHTNESS, currentParams[setting_led_bringhtness-1]);
			eeprom_write8(EEPROM_ADDRESS_LED_SPEED, currentParams[setting_led_speed-1]);

			uint8_t r, g, b;
			leds->ledMode_6_getColor(&r,&g,&b);
			eeprom_write8(EEPROM_ADDRESS_LED_MODE_6_R, r);
			eeprom_write8(EEPROM_ADDRESS_LED_MODE_6_G, g);
			eeprom_write8(EEPROM_ADDRESS_LED_MODE_6_B, b);
#endif
		}
	}

	menuWasActivated = menuIsActivated;
	osDelay(TASK_DELAY_MS);
}

/*! \fn ButtonMenu::isActive
 *  \brief Checks if the Menu is currently active
 *  \return 1 if the menu is acivated, 0 otherwise
*/
uint8_t ButtonMenu::isActive(void)
{
	return menuIsActivated;
}

/*! \fn ButtonMenu::setActive
 *  \brief Sets the Menu active/deactive
 *  \param setActive 1 to activate the menu, 0 to deactivate the menu
*/
void ButtonMenu::setActive(bool setActive)
{
	menuIsActivated = setActive;
}

/*! \fn ButtonMenu::getLedMode
 *  \brief Returns the current LED Mode
 *
 *   LED Modes:
 *
 *  -# Color circle as long as smoke is present
 *  -# Color fade as long as smoke is present
 *  -# Color fade
 *  -# Color circle
 *  -# Static white color
 *  -# Static color of choice (choose with buttons)
 *  -# Thermometer (count the red LEDs to get the Temperature over 20°C)
 *  -# Battery gauge (remaining battery capacity)
 *
 *  \return number of the current LED mode
*/
uint8_t ButtonMenu::getLedMode(void)
{
	return currentParams[setting_led_mode-1];
}

/*! \fn ButtonMenu::getLedBrightness
 *  \brief Returns the current LED Brightness
 *
 *   The LED brightness is represented by a number from
 *   1 (lowest intensity) to 8 (highest intensity)
 *
 *  \return LED brightness (1..8)
*/
uint8_t ButtonMenu::getLedBrightness(void)
{
	return currentParams[setting_led_bringhtness-1];
}

/*! \fn ButtonMenu::getLedSpeed
 *  \brief Returns the current LED Speed
 *
 *   The LED Speed is used to control the change in LED patterns
 *   Its represented by a number from 1 (lowest speed) to 8 (higest speed)
 *
 *  \return LED speed (1..8)
*/
uint8_t ButtonMenu::getLedSpeed(void)
{
	return currentParams[setting_led_speed-1];
}

/*! \fn ButtonMenu::setLedMode
 *  \brief Sets the led Mode
 *
 *   LED Modes:
 *
 *  -# Color circle as long as smoke is present
 *  -# Color fade as long as smoke is present
 *  -# Color fade
 *  -# Color circle
 *  -# Static white color
 *  -# Static color of choice (choose with buttons)
 *  -# Thermometer (count the red LEDs to get the Temperature over 20°C)
 *  -# Battery gauge (remaining battery capacity)
 *
 *  \param mode LED Mode (1..8)
*/
void ButtonMenu::setLedMode(uint8_t mode)
{
	currentMenu = setting_led_mode;
	currentParams[setting_led_mode-1] = mode;
}

/*! \fn ButtonMenu::setLedBrightness
 *  \brief Sets the current LED Brightness
 *
 *   The LED brightness is represented by a number from
 *   1 (lowest intensity) to 8 (highest intensity)
 *
 *  \param brightness LED brightness (1..8)
*/
void ButtonMenu::setLedBrightness(uint8_t brightness)
{
	currentMenu = setting_led_bringhtness;
	currentParams[setting_led_bringhtness-1] = brightness;
}

/*! \fn ButtonMenu::setLedSpeed
 *  \brief Sets the current LED Speed
 *
 *   The LED Speed is used to control the change in LED patterns
 *   Its represented by a number from 1 (lowest speed) to 8 (higest speed)
 *
 *  \param speed LED speed (1..8)
*/
void ButtonMenu::setLedSpeed(uint8_t speed)
{
	currentMenu = setting_led_speed;
	currentParams[setting_led_speed-1] = speed;
}

/*! \fn ButtonMenu::displayMenu
 *  \brief Shows the current menu on the LEDs
 *
 *  \param menuValue the current value of the menuoption to display
 *  \param paramValue the current parameter of the selected menuoption to display
*/
void ButtonMenu::displayMenu(uint8_t menuValue, uint8_t paramValue)
{
	if(paramValue < 8)
	{
		leds->setLEDs(leds->color(0xf4,0xe2,0x42),0,7-paramValue);
	}
	leds->setLEDs(leds->color(0xf4,0xe2,0x42),7+menuValue,NOF_LEDS-1);
	leds->setLEDs(leds->color(0xff,0,0),8-paramValue, 7+menuValue);
}


/*! \fn ButtonMenu::buttonEdgeDetection
 *  \brief Detects positive Edges on the buttons Menu & Param
 *
 * 	The buttons need to be pressed at least TASK_DELAY_MS to be registred
 *	This function updates the private variables menuButtonPosEdge and paramButtonPosEdge
 *
 *  \param menuEdge output, true if menu-button has pos edge
 *  \param paramEdge output, true if param-button has pos edge
*/
void ButtonMenu::buttonEdgeDetection(bool* menuEdge,bool* paramEdge)
{
	static bool oldMenuState = false;
	static bool oldParamState = false;
	static bool internalMenuPosEdge = false;
	static bool internalParamPosEdge = false;

	//MenuButton
	if(oldMenuState && internalMenuPosEdge && BTN_MENU_IS_PRESSED && !BTN_PARAM_IS_PRESSED)
	{
		*menuEdge = true;
	}
	else
	{
		*menuEdge = false;
	}

	if(BTN_MENU_IS_PRESSED && !oldMenuState && !BTN_PARAM_IS_PRESSED)
	{
		internalMenuPosEdge = true;
	}
	else
	{
		internalMenuPosEdge = false;
	}

	//ParamButton
	if(oldParamState && internalParamPosEdge && BTN_PARAM_IS_PRESSED && !BTN_MENU_IS_PRESSED)
	{
		*paramEdge = true;
	}
	else
	{
		*paramEdge = false;
	}

	if(BTN_PARAM_IS_PRESSED && !oldParamState && !BTN_MENU_IS_PRESSED)
	{
		internalParamPosEdge = true;
	}
	else
	{
		internalParamPosEdge = false;
	}

	oldMenuState = BTN_MENU_IS_PRESSED;
	oldParamState = BTN_PARAM_IS_PRESSED;
}



//---------------------------------------------------------------------------------------------------
//Shell..
#if ENABLE_SHELL
// The following functions provide the shell with data (c interface for the shell)
ButtonMenu* buttonMenu;

static void buttonMenu_initShell(ButtonMenu* bM)
{
	buttonMenu = bM;
}

static void buttonMenu_SetMenuActivity(bool activity)
{
	buttonMenu->setActive(activity);
}
static void buttonMenu_SetLEDMode(uint8_t mode)
{
	buttonMenu->setLedMode(mode);
}
static void buttonMenu_SetLEDBrightness(uint8_t brightness)
{
	buttonMenu->setLedBrightness(brightness);
}
static void buttonMenu_SetLEDSpeed(uint8_t speed)
{
	buttonMenu->setLedSpeed(speed);
}

static uint8_t PrintStatus(McuShell_ConstStdIOType *io)
{
  McuShell_SendStatusStr((unsigned char*)"ButtonMenu", (const unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t buttonMenu_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io)
{
	  uint8_t res = ERR_OK;
	  const uint8_t *p;
	  int32_t tmp;

	  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "ButtonMenu help")==0)
	  {

	    McuShell_SendHelpStr((unsigned char*)"ButtonMenu", (const unsigned char*)"Group of ButtonMenu commands\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  help|status", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  setEnable", (const unsigned char*)"Enables/Disables the Menu (1/0)\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  setLEDmode", (const unsigned char*)"Sets the  LedMode (1..8)\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  setLEDbrithgtness", (const unsigned char*)"Sets the LedBrightness (1..8)\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  setLEDspeed", (const unsigned char*)"Sets the LedSpeed (1..8)\r\n", io->stdOut);
	    *handled = TRUE;
	    return ERR_OK;
	  }
	  else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "RTC status")==0))
	  {
	    *handled = TRUE;
	    return PrintStatus(io);
	  }
	  else if (McuUtility_strncmp((char*)cmd, "ButtonMenu setEnable", sizeof("ButtonMenu setEnable")-1)==0)
	  {
		  p = cmd+sizeof("ButtonMenu setEnable")-1;
		  res = McuUtility_xatoi(&p, &tmp);
		  if (res==ERR_OK && tmp>=0 && tmp<=1)
		  {
			  buttonMenu_SetMenuActivity(tmp);
		      *handled = TRUE;
		  }
	  }
	  else if (McuUtility_strncmp((char*)cmd, "ButtonMenu setLEDmode", sizeof("ButtonMenu setLEDmode")-1)==0)
	  {
		  p = cmd+sizeof("ButtonMenu setLEDmode")-1;
		  res = McuUtility_xatoi(&p, &tmp);
		  if (res==ERR_OK && tmp>=1 && tmp<=8)
		  {
			  buttonMenu_SetLEDMode(tmp);
		      *handled = TRUE;
		  }
	  }
	  else if (McuUtility_strncmp((char*)cmd, "ButtonMenu setLEDbrithgtness", sizeof("ButtonMenu setLEDbrithgtness")-1)==0)
	  {
		  p = cmd+sizeof("ButtonMenu setLEDbrithgtness")-1;
		  res = McuUtility_xatoi(&p, &tmp);
		  if (res==ERR_OK && tmp>=1 && tmp<=8)
		  {
			  buttonMenu_SetLEDBrightness(tmp);
		      *handled = TRUE;
		  }
	  }
	  else if (McuUtility_strncmp((char*)cmd, "ButtonMenu setLEDspeed", sizeof("ButtonMenu setLEDspeed")-1)==0)
	  {
		  p = cmd+sizeof("ButtonMenu setLEDspeed")-1;
		  res = McuUtility_xatoi(&p, &tmp);
		  if (res==ERR_OK && tmp>=1 && tmp<=8)
		  {
			  buttonMenu_SetLEDSpeed(tmp);
		      *handled = TRUE;
		  }
	  }
	  return res;
}
#endif
