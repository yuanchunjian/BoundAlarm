#pragma once
#include "ParseInterface.h"

class CPositionInfo : 
	public CParseInterface
{
public:
	CPositionInfo(void);
	virtual ~CPositionInfo(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);

	bool HasName;
	unsigned char RotateType;
	float XPos;
	float XRotate;
	float YPos;
	float YRotate;
	float ZPos;
	float ZRotate;
};

