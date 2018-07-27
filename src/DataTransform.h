#pragma once
#include <string>
class CDataTransform
{
public:
	CDataTransform(void);
	virtual ~CDataTransform(void);

	int GetInt(unsigned char buffer[], int index, bool reverse);
	unsigned int GetUInt(unsigned char buffer[], int index, bool reverse);
	short GetShort(unsigned char buffer[], int index, bool reverse);
	unsigned short GetUShort(unsigned char buffer[], int index, bool reverse);
	float GetFloat(unsigned char buffer[], int index, bool reverse);
	std::string GetString(unsigned char buffer[], int index, int count, bool reverse);
};

