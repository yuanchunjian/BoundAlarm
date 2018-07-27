
#include "Outputchannel16.h"


COutputchannel16::COutputchannel16(void)
{
}


COutputchannel16::~COutputchannel16(void)
{
}

bool COutputchannel16::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		Content = m_obDataTransform.GetString(buffer, index, 5, reverse);
		index += 5;
		ScaleFactor = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		ScaleFactorOffset = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		StartAngle = ((float)m_obDataTransform.GetInt(buffer, index, reverse)) / 10000.0;
		index += 4;
		Steps = ((float)m_obDataTransform.GetUShort(buffer, index, reverse)) / 10000.0;
		index += 2;
		int num = m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
		if (num > 0)
		{
			Data.resize(num);
			for (int i = 0; i < num; i++)
			{
				Data[i] = m_obDataTransform.GetUShort(buffer, index, reverse);
				index += 2;
			}
		}
		return true;
	}
	catch (...)
	{
		return false;
	}
	return true;
}
