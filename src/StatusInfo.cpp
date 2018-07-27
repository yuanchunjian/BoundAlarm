
#include "StatusInfo.h"


CStatusInfo::CStatusInfo(void)
{
}


CStatusInfo::~CStatusInfo(void)
{
}

bool CStatusInfo::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		TelegramCounter = m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
		ScanCounter = m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
		TimeSinceStartup = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		TimeofTransmission = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		StatusOfDigitalInputs = buffer[index++];
		index++;
		StatusofDigitalOutputs = buffer[index++];
		index++;
		index += 2;
	}catch(...)
	{
		return false;
	}
	return true;
}
