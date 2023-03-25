/*
 * cubeAppInterface.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: dave
 */
#include "cubeAppInterface.h"
#include "mainTask.h"
#include "ButtonMenu.h"

void cubeInterface_mainInit(void)
{
	mainTaskInit();
}

void cubeInterface_mainTask(void)
{
	mainTask();
}

void cubeInterface_buttonMenu_task(void)
{
	mainTask_getButtonMenu()->task();
}
