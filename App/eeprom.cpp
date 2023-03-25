/*
 * eeprom.cpp
 *
 *  Created on: Jun 24, 2019
 *      Author: dhuwiler
 */
#include "eeprom.h"
extern "C" { //without <extern "C"> the linker dosnt find the references to the EE functions
#include "eeprom_emul.h"
}

#if ENABLE_SHELL
#include "shell.h"
#include "McuLib.h"
#endif


static uint16_t VirtAddVarTab[NB_OF_VARIABLES];
static uint32_t ErasingOnGoing = 0;

void eeprom_init(void)
{
	for (uint16_t i = 0; i < NB_OF_VARIABLES; i++)
	{
		VirtAddVarTab[i] = (uint16_t)(10*i + 1);
	}
	HAL_FLASH_Unlock();
	EE_Init(VirtAddVarTab, EE_FORCED_ERASE);
	HAL_FLASH_Lock();
}

void eeprom_format(void)
{
	HAL_FLASH_Unlock();
	EE_Format(EE_FORCED_ERASE);
	HAL_FLASH_Lock();
}


bool eeprom_write8(uint16_t address, uint8_t value)
{
	EE_Status ee_status = EE_OK;
	if(address>NB_OF_VARIABLES-1)
	{
		return false;
	}
	while (ErasingOnGoing == 1) { }
	HAL_FLASH_Unlock();
	ee_status = EE_WriteVariable8bits(VirtAddVarTab[address], value);

    /* Start cleanup IT mode, if cleanup is needed */
    if ((ee_status & EE_STATUSMASK_CLEANUP) == EE_STATUSMASK_CLEANUP)
    {
    	ErasingOnGoing = 1;
    	ee_status= (EE_Status)((int)ee_status | (int)EE_CleanUp_IT());
    }
    else
    {
    	HAL_FLASH_Lock();
    }

    return true;
}

bool eeprom_read8(uint16_t address, uint8_t* value)
{
	return (bool)EE_ReadVariable8bits(VirtAddVarTab[address], value);
}


/**
  * @brief  FLASH end of operation interrupt callback.
  * @param  ReturnValue: The value saved in this parameter depends on the ongoing procedure
  *                  Mass Erase: Bank number which has been requested to erase
  *                  Page Erase: Page which has been erased
  *                    (if 0xFFFFFFFF, it means that all the selected pages have been erased)
  *                  Program: Address which was selected for data program
  * @retval None
  */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* Call CleanUp callback when all requested pages have been erased */
  if (ReturnValue == 0xFFFFFFFF)
  {
    EE_EndOfCleanup_UserCallback();
  }
}

/**
  * @brief  Clean Up end of operation interrupt callback.
  */
void EE_EndOfCleanup_UserCallback(void)
{
  ErasingOnGoing = 0;
  HAL_FLASH_Lock();
}



#if ENABLE_SHELL
static uint8_t PrintStatus(McuShell_ConstStdIOType *io) {
  McuShell_SendStatusStr((unsigned char*)"EEPROM", (const unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t EEPROM_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io)
{
	  uint8_t res = ERR_OK;
	  const uint8_t *p;
	  int32_t tmp;
	  uint8_t value;

	  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "EEPROM help")==0)
	  {

	    McuShell_SendHelpStr((unsigned char*)"EEPROM", (const unsigned char*)"Group of EEPROM commands\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  help|status", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  getValue", (const unsigned char*)"Returns value at address (1/1)\r\n", io->stdOut);
	    McuShell_SendHelpStr((unsigned char*)"  format", (const unsigned char*)"Formats the whole EEPROM\r\n", io->stdOut);
	    *handled = TRUE;
	    return ERR_OK;
	  }
	  else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "RTC status")==0))
	  {
	    *handled = TRUE;
	    return PrintStatus(io);
	  }
	  else if (McuUtility_strncmp((char*)cmd, "EEPROM getValue", sizeof("EEPROM getValue")-1)==0)
	  {
		  p = cmd+sizeof("EEPROM getValue")-1;
		  res = McuUtility_xatoi(&p, &tmp);
		  if (res==ERR_OK && tmp>=0 && tmp<(int32_t)NB_OF_VARIABLES)
		  {
			  eeprom_read8(tmp, &value);
			  McuShell_SendStr((uint8_t*)"EEPROM Value: ",io->stdOut);
			  McuShell_SendNum8u(value,io->stdOut);
			  McuShell_SendStr((uint8_t*)"\r\n",io->stdOut);
		      *handled = TRUE;
		  }
	  }
	  else if (McuUtility_strncmp((char*)cmd, "EEPROM format", sizeof("EEPROM format")-1)==0)
	  {
		  EE_Format(EE_FORCED_ERASE);
		  *handled = TRUE;
	  }
	  return res;
}
#endif
