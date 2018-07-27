
#include "EventInfo.h"


CEventInfo::CEventInfo(void)
{
}


CEventInfo::~CEventInfo(void)
{
}

bool CEventInfo::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		EventTye = m_obDataTransform.GetString(buffer, index, 4, reverse);
		index += 4;
		EncoderPosition = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		TimeofEvent = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		AngleofEvent = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		return true;
	}
	catch (...)
	{
		return false;
	}
}
