#pragma once
#include "ParseInterface.h"
#include "CustomType.h"
class COutputchannel16 :
	public CParseInterface
{
public:
	COutputchannel16(void);
	virtual ~COutputchannel16(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);
public:
	string Content;

	vector<ushort> Data;

	float ScaleFactor;

	float ScaleFactorOffset;

	float StartAngle;

	float Steps;
};

