#pragma once
#include "ParseInterface.h"
class CTimeInfo :
	public CParseInterface
{
public:
	CTimeInfo(void);
	virtual ~CTimeInfo(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);

	unsigned char Day;
	unsigned char  Hour;
	unsigned int MicroSecond;
	unsigned char  Minute;
	unsigned char  Month;
	unsigned char  Second;
	unsigned short Year;
};

