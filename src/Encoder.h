#pragma once
#include "ParseInterface.h"

class CEncoder : 
	public CParseInterface
{
public:
	CEncoder(void);
	virtual ~CEncoder(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);
	unsigned short Position;
	unsigned short Speed;
};

