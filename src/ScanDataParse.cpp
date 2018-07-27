
#include "ScanDataParse.h"


CScanDataParse::CScanDataParse(void)
{

}


CScanDataParse::~CScanDataParse(void)
{
}

bool CScanDataParse::Parse(unsigned char buffer[], int& index, bool reverse)
{
	try
	{
		Version = m_obDataTransform.GetUShort(buffer, index, reverse);
		index += 2;
	
		if (!Device.Parse(buffer, index, reverse))
		{
			return false;
		}
		
		if (!StatusInfo.Parse(buffer, index, reverse))
		{
			return false;
		}
	
		if (!Frequencys.Parse(buffer, index, reverse))
		{
			return false;
		}

		short num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (num > 0)
		{
			if (num > 0)
				Encoders.resize(num);
			for (int i = 0; i < num; i++)
			{
				if (!Encoders.at(i).Parse(buffer, index, reverse))
				{
					return false;
				}
			}
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;

		if (index > 0)
		{
			Channel16s.clear();
			if (num > 0)
			   Channel16s.resize(num);
			for (int j = 0; j < num; j++)
			{
				if (!Channel16s.at(j).Parse(buffer, index, reverse))
				{
					return false;
				}
			}
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (index > 0)
		{
			Channel8s.clear();
			if (num > 0)
				Channel8s.resize(num);
			for (int k = 0; k < num; k++)
			{
				if (!Channel8s.at(k).Parse(buffer, index, reverse))
				{
					return false;
				}
			}
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (num == 1)
		{
			if (!PositionInfo.Parse(buffer, index, reverse))
			{
				return false;
			}
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (num == 1)
		{
			int count = buffer[index++];
			DeviceName = m_obDataTransform.GetString(buffer, index, count,reverse);
			index += count;
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (num == 1)
		{
			int num6 = buffer[index++];
			Comment = m_obDataTransform.GetString(buffer, index, num6, reverse);
			index += num6;
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (num == 1)
		{
			if (!TimeInfo.Parse(buffer, index, reverse))
			{
				return false;
			}
		}
		num = m_obDataTransform.GetShort(buffer, index, reverse);
		index += 2;
		if (num == 1)
		{
			if (!EventInfo.Parse(buffer, index, reverse))
			{
				return false;
			}
		}
		return true;
	}
	catch (...)
	{
		return false;
	}
}
