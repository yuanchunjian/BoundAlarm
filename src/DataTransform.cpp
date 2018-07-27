
#include "DataTransform.h"
#include "CustomType.h"


CDataTransform::CDataTransform(void)
{
}


CDataTransform::~CDataTransform(void)
{
}

int CDataTransform::GetInt(unsigned char buffer[], int index, bool reverse)
{
	int val = 0;
	int len = sizeof(int);
	if (reverse)
	{
		for (int i = len - 1; i >= 0; i--)
		{
			val += buffer[index+i] << ((len - 1 - i ) * 8);
		}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			val += buffer[index+i] << (i * 8);
		}
	}
	return val;
}

unsigned int CDataTransform::GetUInt(unsigned char buffer[], int index, bool reverse)
{
	unsigned int val = 0;
	int len = sizeof(int);
	if (reverse)
	{
		for (int i = len - 1; i >= 0; i--)
		{
			val += buffer[index+i] << ((len - 1 - i ) * 8);
		}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			val += buffer[index+i] << (i * 8);
		}
	}
	return val;
}

short CDataTransform::GetShort(unsigned char buffer[], int index, bool reverse)
{
	short val = 0;
	int len = sizeof(short);
	if (reverse)
	{
		for (int i = len - 1; i >= 0; i--)
		{
			val += buffer[index+i] << ((len - 1 - i ) * 8);
		}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			val += buffer[index+i] << (i * 8);
		}
	}
	return val;
}

unsigned short CDataTransform::GetUShort(unsigned char buffer[], int index, bool reverse)
{
	unsigned short val = 0;
	int len = sizeof(ushort);
	if (reverse)
	{
		for (int i = len - 1; i >= 0; i--)
		{
			val += buffer[index+i] << ((len - 1 - i ) * 8);
		}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			val += buffer[index+i] << (i * 8);
		}
	}
	return val;
}
float CDataTransform::GetFloat(unsigned char buffer[], int index, bool reverse)
{
	int len = sizeof(float);
	unsigned char* buf = new unsigned char[len];
	if (reverse)
	{
		for (int i = len - 1; i >= 0; i--)
		{
			buf[i] = buffer[index+i];
		}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			buf[i] = buffer[index+i];
		}
	}
	float fValue = *((float*)&buf[0]);
	delete[] buf;
	buf = NULL;
	return fValue;
}

std::string CDataTransform::GetString(unsigned char buffer[], int index, int count, bool reverse)
{
	std::string strTmp = "";
	if (reverse)
	{
		for (int i = count - 1; i >= 0; i--)
		{
			strTmp += (char)buffer[index+i];
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			strTmp += (char)buffer[index+i];
		}
	}
	return strTmp;
}
