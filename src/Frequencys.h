#pragma once
#include "ParseInterface.h"
class CFrequencys  : 
	public CParseInterface
{
public:
	CFrequencys(void);
	virtual ~CFrequencys(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);

	unsigned int MeasurementFrequency;

	float ScanFrequency;
};

