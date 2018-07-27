
#include "Encoder.h"


CEncoder::CEncoder(void)
{
}


CEncoder::~CEncoder(void)
{
}

bool CEncoder::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		Position = m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
		Speed =  m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
		return true;
	}
	catch (...)
	{
		return false;
	}
	return true;
}
