/*
 * CControlGPIO.h
 *
 *  Created on: May 19, 2016
 *      Author: yuan
 */

#ifndef CCONTROLGPIO_H_
#define CCONTROLGPIO_H_

class CControlGPIO {
public:
	CControlGPIO();
	virtual ~CControlGPIO();
	int Open(unsigned int _dwDisBits);
	int Close();
    int TurnOn();
    int TurnOff();
private:
	int GPIO_OutEnable();
	int GPIO_OutDisable();
	int GPIO_OutSet();
	int GPIO_OutClear();
	int GPIO_PinState(unsigned int* pPinState);
private:
	int				fd;
    int				rc;
	int				i1;
	unsigned int	dwPinState;
	int             numOfGPIOs;		//there are 32 GPIOs in EM335X
	unsigned int dwDisBits;
};

#endif /* CCONTROLGPIO_H_ */
