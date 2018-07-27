#pragma once
#include "ParseInterface.h"
#include "Frequencys.h"

class CScanDevice : 
	public CParseInterface
{
public:
	CScanDevice(void);
	virtual ~CScanDevice(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);

	unsigned short DeviceNumber;
	unsigned char DeviceStatus;
	CFrequencys Frequencys;
	unsigned int SerialNumber;
};

