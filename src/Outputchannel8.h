#pragma once
#include "ParseInterface.h"
class COutputchannel8 :
	public CParseInterface
{
public:
	COutputchannel8(void);
	virtual ~COutputchannel8(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);
protected:
	string Content;

	vector<unsigned short> Data;

	float ScaleFactor;

	float ScaleFactorOffset;

	float StartAngle;
};

