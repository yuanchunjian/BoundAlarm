
#include "Frequencys.h"


CFrequencys::CFrequencys(void)
{
}


CFrequencys::~CFrequencys(void)
{
}

bool CFrequencys::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		ScanFrequency = ((float)m_obDataTransform.GetUInt(buffer, index, reverse)) / 100.0;
		index += 4;
		MeasurementFrequency = m_obDataTransform.GetUInt(buffer, index, reverse);
		index += 4;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

