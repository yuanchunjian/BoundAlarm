
#include "ScanDevice.h"


CScanDevice::CScanDevice(void)
{
}


CScanDevice::~CScanDevice(void)
{
}

bool CScanDevice::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		DeviceNumber = m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
		SerialNumber = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		DeviceStatus = buffer[index++];
		index++;
	}
	catch(...)
	{
		return false;
	}
	return true;
}
