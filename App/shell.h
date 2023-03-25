/*
 * shell.h
 *
 *  Created on: Jun 26, 2019
 *      Author: dhuwiler
 */
#ifndef SHELL_H_
#define SHELL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"

void shell_init();
void shell_run();
uint8_t shell_PutIntoRecBuf(uint8_t* data, uint8_t len);
uint8_t shell_getInputBufferLength(void);
uint8_t shell_SendCharUSBCDC(uint8_t ch);
uint8_t shell_RecvCharUSBCDC(uint8_t *c);

#ifdef __cplusplus
}
#endif

#endif /* SHELL_H_ */
