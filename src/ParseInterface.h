#pragma once
#include <string>
#include <vector>
#include "DataTransform.h"
using namespace std;
class CParseInterface
{
public:
	CParseInterface(void);
	virtual ~CParseInterface(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true) = 0;
protected:
	CDataTransform m_obDataTransform;
};

