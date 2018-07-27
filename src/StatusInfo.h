#pragma once
#include "ParseInterface.h"
class CStatusInfo :
	public CParseInterface
{
public:
	CStatusInfo(void);
	~CStatusInfo(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);

	unsigned short ScanCounter;
	unsigned char StatusOfDigitalInputs;
	unsigned char StatusofDigitalOutputs;
	unsigned short TelegramCounter;
	unsigned int TimeofTransmission;
	unsigned int TimeSinceStartup;
};

