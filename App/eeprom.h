/*
 * eeprom.h
 *
 *  Created on: Jun 24, 2019
 *      Author: dhuwiler
 */

#ifndef EEPROM_H_
#define EEPROM_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "config.h"

#if ENABLE_SHELL
#include "McuShell.h"
#endif

void eeprom_init(void);
void eeprom_format(void);
bool eeprom_write8(uint16_t address, uint8_t value);
bool eeprom_read8(uint16_t address, uint8_t* value);
#if ENABLE_SHELL
uint8_t EEPROM_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);
#endif

#ifdef __cplusplus
}
#endif


#endif /* EEPROM_H_ */
