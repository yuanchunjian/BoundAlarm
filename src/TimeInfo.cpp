
#include "TimeInfo.h"


CTimeInfo::CTimeInfo(void)
{
}


CTimeInfo::~CTimeInfo(void)
{
}

bool CTimeInfo::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		Year = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		Month = buffer[index++];
		Day = buffer[index++];
		Hour = buffer[index++];
		Minute = buffer[index++];
		Second = buffer[index++];
		MicroSecond = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		return true;
	}
	catch (...)
	{
		return false;
	}
}
