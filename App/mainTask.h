/*! \mainpage
 *
<pre>
 _______  __   __  ___   _______  __   __  _______  _______  _______  __    _  _______  ______    _______  ___      ___      _______  ______
|       ||  | |  ||   | |       ||  | |  ||   _   ||       ||       ||  |  | ||       ||    _ |  |       ||   |    |   |    |       ||    _ |
|  _____||  |_|  ||   | |  _____||  |_|  ||  |_|  ||       ||   _   ||   |_| ||_     _||   | ||  |   _   ||   |    |   |    |    ___||   | ||
| |_____ |       ||   | | |_____ |       ||       ||       ||  | |  ||       |  |   |  |   |_||_ |  | |  ||   |    |   |    |   |___ |   |_||_
|_____  ||       ||   | |_____  ||       ||       ||      _||  |_|  ||  _    |  |   |  |    __  ||  |_|  ||   |___ |   |___ |    ___||    __  |
 _____| ||   _   ||   |  _____| ||   _   ||   _   ||     |_ |       || | |   |  |   |  |   |  | ||       ||       ||       ||   |___ |   |  | |
|_______||__| |__||___| |_______||__| |__||__| |__||_______||_______||_|  |__|  |___|  |___|  |_||_______||_______||_______||_______||___|  |_|

</pre>
 *
 *
 *
 *
 * Documentation of the ShishaController <br>
 * The ShishaController is a device to lightup Shishas. Different Modes can be
 * choosen to either illustrate the stroke or to enlight the Shisha.
 *
 *
 * \section ShishaController
 *
 * \subsection handling_subsec Usage
 * To start the ShishaController, turn on the PowerButton.
 * The Battery can be charged via the usb port of the Device.
 * As long as the battery has a high enough voltage (>3V) the device runns normally.
 * If the voltage is below 3V, the LEDs will not light up anymore.
 *
 * \subsection ChangeSettings_subsec Change Settings
 * To change settings, the MenuButton and the ParamButton need to be pressed simultaneously for one Second.
 * After one second the settings menu is entered and different settings can be altered.
 * A description of the Modes and Menus can be found in the ButtonMenu .h file
 *
 * \subsection SaveSettings_subsec Save Settings
 * To save settings permanently inside the EEPROM (Flash-Emulated-EEPROM) the settings menu has to be left.
 * To leave the settings menu , the MenuButton and the ParamButton need to be pressed simultaneously for one Second.
 *
 * \subsection USB_CDC_subsec USB CDC manintainance Shell
 * The Device implements a usb cfc shell. To access this shell, a usb cable needs to be connected to the
 * ShishaController. On the Host-PC a uart terminal programm can be used to connect to the ShishaController.
 * The windows device manager can be used to find the ShishaController-COM port.
 * Once the connection is established, type in "help". The shell displays now all possible shell-commands
 *
 *  \author David Huwiler
 *  \version 1.0
 *  \date 02.07.2019
 */

/*
 * mainTask.h
 *
 *  Created on: Jun 21, 2019
 *      Author: dave
 */

#ifndef MAINTASK_H_
#define MAINTASK_H_

class ButtonMenu;

void mainTaskInit(void);
void mainTask(void);
ButtonMenu* mainTask_getButtonMenu(void);

#endif /* MAINTASK_H_ */
