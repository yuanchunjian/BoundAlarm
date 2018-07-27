/*
 * CControlGPIO.cpp
 *
 *  Created on: May 19, 2016
 *      Author: yuan
 */

#include "CControlGPIO.h"
#include "em335x_drivers.h"
#include <liblogger/liblogger.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

CControlGPIO::CControlGPIO() {
	// TODO Auto-generated constructor stub
	numOfGPIOs = 32;
}

CControlGPIO::~CControlGPIO() {
	// TODO Auto-generated destructor stub
}

int CControlGPIO::Open(unsigned int _dwDisBits)
{
	dwDisBits = _dwDisBits;
	LogInfo("Driver em335x_gpio open, dwDisBits=%d", dwDisBits);

	fd = open("/dev/em335x_gpio", O_RDWR);
	LogInfo("open file = %d", fd);

	rc = GPIO_OutEnable();
	if(rc < 0)
	{
		LogError("GPIO_OutEnable::failed %d\n", rc);
		return rc;
	}
	return rc;
}

int CControlGPIO::Close()
{
	int rc = GPIO_OutDisable();
	close(fd);
	LogInfo("close file");
	return rc;
}

int CControlGPIO::TurnOn()
{
	rc = GPIO_OutClear();
	if(rc < 0)
	{
		printf("GPIO_OutClear::failed %d", rc);
		return rc;
	}
	dwPinState = 0xffffffff;				// read states of all bits
	rc = GPIO_PinState(&dwPinState);
	if(rc < 0)
	{
		printf("GPIO_PinState::failed %d", rc);
		return rc;
	}
	printf("PinState = 0x%08x", dwPinState);
	return rc;
}

int CControlGPIO::TurnOff()
{
	rc = GPIO_OutSet();
	if(rc < 0)
	{
		LogError("GPIO_OutSet::failed %d", rc);
		return rc;
	}
	dwPinState = 0xffffffff;				// read states of all bits
	rc = GPIO_PinState(&dwPinState);
	if(rc < 0)
	{
		LogError("GPIO_PinState::failed %d", rc);
		return rc;
	}
	LogInfo("PinState = 0x%08x", dwPinState);
	return rc;
}

int CControlGPIO::GPIO_OutEnable()
{
	int 				rc;
	struct double_pars	dpars;

	dpars.par1 = EM335X_GPIO_OUTPUT_ENABLE;		// 0
	dpars.par2 = dwDisBits;

	rc = write(fd, &dpars, sizeof(struct double_pars));
	return rc;
}

int CControlGPIO::GPIO_OutDisable()
{
	int 				rc;
	struct double_pars	dpars;

	dpars.par1 = EM335X_GPIO_OUTPUT_DISABLE;	// 1
	dpars.par2 = dwDisBits;

	rc = write(fd, &dpars, sizeof(struct double_pars));
	return rc;
}

int CControlGPIO::GPIO_OutSet()
{
	int 				rc;
	struct double_pars	dpars;

	dpars.par1 = EM335X_GPIO_OUTPUT_SET;	// 2
	dpars.par2 = dwDisBits;

	rc = write(fd, &dpars, sizeof(struct double_pars));
	return rc;
}

int CControlGPIO::GPIO_OutClear()
{
	int 				rc;
	struct double_pars	dpars;

	dpars.par1 = EM335X_GPIO_OUTPUT_CLEAR;	// 3
	dpars.par2 = dwDisBits;

	rc = write(fd, &dpars, sizeof(struct double_pars));
	return rc;
}

int CControlGPIO::GPIO_PinState(unsigned int* pPinState)
{
	int 				rc;
	struct double_pars	dpars;

	dpars.par1 = EM335X_GPIO_INPUT_STATE;	// 5
	dpars.par2 = *pPinState;

	rc = read(fd, &dpars, sizeof(struct double_pars));
	if(!rc)
	{
		*pPinState = dpars.par2;
	}
	return rc;
}


