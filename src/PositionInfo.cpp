
#include "PositionInfo.h"


CPositionInfo::CPositionInfo(void)
{
}


CPositionInfo::~CPositionInfo(void)
{
}

bool CPositionInfo::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		XPos = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		YPos = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		ZPos = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		XRotate = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		YRotate = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		ZRotate = m_obDataTransform.GetFloat(buffer, index, reverse);
		index += 4;
		RotateType = buffer[index++];
		HasName = buffer[index++] == 1;
	}catch(...)
	{
		return false;
	}
	return true;
}
