#pragma once
#include "ParseInterface.h"

class CEventInfo : 
	public CParseInterface
{
public:
	CEventInfo(void);
	virtual ~CEventInfo(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);

	int AngleofEvent;

	unsigned int  EncoderPosition;

	string EventTye;

	unsigned int TimeofEvent;
};

