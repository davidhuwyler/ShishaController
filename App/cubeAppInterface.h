/*
 * cubeAppInterface.h
 *
 *  Created on: Jun 26, 2019
 *      Author: dave
 */

#ifndef CUBEAPPINTERFACE_H_
#define CUBEAPPINTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

void cubeInterface_mainInit(void);
void cubeInterface_mainTask(void);
void cubeInterface_buttonMenu_task(void);

#ifdef __cplusplus
}
#endif

#endif /* CUBEAPPINTERFACE_H_ */
