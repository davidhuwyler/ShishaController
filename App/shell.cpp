/*
 * shell.c
 *
 *  Created on: Jun 26, 2019
 *      Author: dhuwiler
 */
#include "shell.h"
#include "config.h"
#include "ButtonMenu.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "McuLib.h"
#include "McuShell.h"
#include "usbd_cdc_if.h"
#if ENABLE_EEPROM
#include "eeprom.h"
#endif //ENABLE_EEPROM
#ifdef __cplusplus
}
#endif //extern "C"

#if ENABLE_BATTERY_CHECK
#include "Battery.h"
#endif //ENABLE_BATTERY_CHECK

#define USB_CDC_BUF_SIZE 100
static uint8_t usbCDC_inBuf[USB_CDC_BUF_SIZE];
static uint8_t usbCDC_bufPtr_firstElemPtr;
static uint8_t usbCDC_bufPtr_nextFreeSpace;
static uint8_t usbCDC_bufnofElemInBuf;


static const McuShell_ParseCommandCallback CmdParserTable[] =
{
#if ENABLE_SHELL
	buttonMenu_ParseCommand,
#if ENABLE_EEPROM
	EEPROM_ParseCommand,
#endif
#if ENABLE_BATTERY_CHECK
	battery_ParseCommand,
#endif
#endif
  NULL /* sentinel */
};


void shell_init()
{
	usbCDC_bufnofElemInBuf = 0;
	usbCDC_bufPtr_firstElemPtr = 0;
	usbCDC_bufPtr_nextFreeSpace = 0;
}

void shell_run()
{
	McuShell_ReadAndParseWithCommandTable(McuShell_DefaultShellBuffer, sizeof(McuShell_DefaultShellBuffer), McuShell_GetStdio(), CmdParserTable);
}

uint8_t shell_PutIntoRecBuf(uint8_t* data, uint8_t len)
{
	if(len>(USB_CDC_BUF_SIZE-usbCDC_bufnofElemInBuf))
	{
		return ERR_PARAM_BUFFER_COUNT;
	}

	for(int i = 0 ; i<len ; i++)
	{
		usbCDC_inBuf[usbCDC_bufPtr_nextFreeSpace] = data[i];
		usbCDC_bufPtr_nextFreeSpace++;
		usbCDC_bufPtr_nextFreeSpace%=USB_CDC_BUF_SIZE;
		usbCDC_bufnofElemInBuf++;
	}

	return ERR_OK;
}

uint8_t shell_getInputBufferLength(void)
{
	return usbCDC_bufnofElemInBuf;
}

uint8_t shell_SendCharUSBCDC(uint8_t ch)
{
  int res;

  res = CDC_Transmit_FS(&ch, 1);
  if (res!=0) {
    return ERR_TXFULL; /* character not sent? */
  }
  return ERR_OK;
}

uint8_t shell_RecvCharUSBCDC(uint8_t *c)
{

  if(usbCDC_bufnofElemInBuf)
  {
	  usbCDC_bufnofElemInBuf--;
	  *c = usbCDC_inBuf[usbCDC_bufPtr_firstElemPtr];
	  usbCDC_bufPtr_firstElemPtr++;
	  usbCDC_bufPtr_firstElemPtr%=USB_CDC_BUF_SIZE;
	  return ERR_OK;
  }
  else
  {
	  return ERR_RXEMPTY;
  }
}

