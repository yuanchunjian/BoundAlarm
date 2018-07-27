
#include "AlgorithmTool.h"
#include <math.h>

 CAlgorithmTool* CAlgorithmTool::m_pInstance = new CAlgorithmTool;

 CAlgorithmTool* CAlgorithmTool::GetInstance()
 {
	/* if (m_pInstance == NULL)
	 {
		 m_pInstance = new CAlgorithmTool;
	 }*/
	 return m_pInstance;
 }

CAlgorithmTool::CAlgorithmTool(void)
{
}


CAlgorithmTool::~CAlgorithmTool(void)
{
}

bool CAlgorithmTool::Chebyshev(CPOINT* data, int length, int m, double* result)
{
	int num3;
	if (m > 20 || length < m)
	{
		return false;
	}
	//result = new double[m + 1];
	int* numArray = new int[0x15];
	double* numArray2 = new double[0x15];
	int num2 = m + 1;
	double num9 = 0.0;
	numArray[0] = 0;
	numArray[m] = length - 1;
	int num5 = (length - 1) / m;
	int index = num5;
	for (num3 = 1; num3 <= (m - 1); num3++)
	{
		numArray[num3] = index;
		index += num5;
	}
	while (true)
	{
		int num6;
		int num7;
		double num11;
		double num12;
		double num13;
		double num14;
		double x;
		double num10 = 1.0;
		num3 = 0;
		while (num3 <= m)
		{
			result[num3] = data[numArray[num3]].Y;
			numArray2[num3] = -num10;
			num10 = -num10;
			num3++;
		}
		for (index = 1; index <= m; index++)
		{
			num6 = num2;
			num12 = result[num6 - 1];
			num14 = numArray2[num6 - 1];
			num3 = index;
			while (num3 <= m)
			{
				x = data[numArray[num6 - 1]].X - data[numArray[(num2 - num3) - 1]].X;
				num11 = result[((m - num3) + index) - 1];
				num13 = numArray2[((m - num3) + index) - 1];
				result[num6 - 1] = (num12 - num11) / x;
				numArray2[num6 - 1] = (num14 - num13) / x;
				num6 = (m - num3) + index;
				num12 = num11;
				num14 = num13;
				num3++;
			}
		}
		num10 = -result[m] / numArray2[m];
		for (num3 = 0; num3 <= m; num3++)
		{
			result[num3] += numArray2[num3] * num10;
		}
		for (index = 1; index <= (m - 1); index++)
		{
			num6 = m - index;
			x = data[numArray[num6 - 1]].X;
			num12 = result[num6 - 1];
			num7 = num2 - index;
			while (num7 <= m)
			{
				num11 = result[num7 - 1];
				result[num6 - 1] = num12 - (x * num11);
				num12 = num11;
				num6 = num7;
				num7++;
			}
		}
		double num16 = fabs(num10);
		if (num16 <= num9)
		{
			result[m] = -num16;
			return true;
		}
		result[m] = num16;
		num9 = num16;
		int num8 = numArray[0];
		num13 = num10;
		index = 0;
		for (num3 = 0; num3 <= (length - 1); num3++)
		{
			if (num3 == numArray[index])
			{
				if (index < m)
				{
					index++;
				}
			}
			else
			{
				num14 = result[m - 1];
				for (num7 = m - 2; num7 >= 0; num7--)
				{
					num14 = (num14 * data[num3].X) + result[num7];
				}
				num14 -= data[num3].Y;
				if (fabs(num14) > num16)
				{
					num16 = fabs(num14);
					num13 = num14;
					num8 = num3;
				}
			}
		}
		if (num8 == numArray[0])
		{
			return true;
		}
		num3 = 0;
		num5 = 1;
		while (num5 == 1)
		{
			num5 = 0;
			if (num8 >= numArray[num3])
			{
				num3++;
				if (num3 <= m)
				{
					num5 = 1;
				}
			}
		}
		if (num3 > m)
		{
			num3 = m;
		}
		if (num3 == ((num3 / 2) * 2))
		{
			num14 = -num10;
		}
		else
		{
			num14 = num10;
		}
		if ((num13 * num14) >= 0.0)
		{
			numArray[num3] = num8;
		}
		else if (num8 < numArray[0])
		{
			for (index = m - 1; index >= 0; index--)
			{
				numArray[index + 1] = numArray[index];
			}
			numArray[0] = num8;
		}
		else if (num8 > numArray[m])
		{
			for (index = 1; index <= m; index++)
			{
				numArray[index - 1] = numArray[index];
			}
			numArray[m] = num8;
		}
		else
		{
			numArray[num3 - 1] = num8;
		}
	}
	delete[] numArray;
	delete[] numArray2;
	return true;
}

int CAlgorithmTool::FirstValidDataIndex(const CAlgorithmInfo* info, const std::vector<double>& data, int startIndex)
{
	//   System.Diagnostics.Contracts.__ContractsRuntime.Requires<ArgumentNullException>(data != null, null, "data != null");
	for (int i = startIndex; i < data.size(); i++)
	{
		if ((data[i] > info->MinValue) && (data[i] < info->MaxValue))
		{
			return i;
		}
	}
	return -1;
}

int CAlgorithmTool::FirstValidDataIndex(const CAlgorithmInfo* info, const std::vector<ushort>& data, int startIndex)
{
	for (int i = startIndex; i < data.size(); i++)
	{
		if ((data[i] > info->MinValue) && (data[i] < info->MaxValue))
		{
			return i;
		}
	}
	return -1;
}
//已知2点的极坐标，求向量P1P2相对于X轴夹角
double CAlgorithmTool::GetDirection(CPOINT p1, CPOINT p2)
{
	double a = ToArc(p1.X);
	double num2 = ToArc(p2.X);
	double y = (p2.Y * sin(num2)) - (p1.Y * sin(a));
	double x = (p2.Y * cos(num2)) - (p1.Y * cos(a));
	return ToDegree(atan2(y, x));
}
//已知三角形2边和夹角，求第三边距离
double CAlgorithmTool::GetDistance(CPOINT p1, CPOINT p2)
{
	double num = ToArc(p1.X);
	double num2 = ToArc(p2.X);
	double num3 = ((p1.Y * p1.Y) + (p2.Y * p2.Y)) - (((2.0 * p1.Y) * p2.Y) * cos(num - num2));
	return sqrt(fabs(num3));
}

int CAlgorithmTool::GetFrameDiff(ushort teleNum1, ushort teleNum2)
{
	int num = teleNum2 - teleNum1;
	if (num < 0)
	{
		num += 0x10000;
	}
	return num;
}

double CAlgorithmTool::ToArc(double value)
{
	return ((value * 3.1415926535897931) / 180.0);
}

CPOINT CAlgorithmTool::ToCartesian(double angle, double radius)
{
	double d = ToArc(angle);
	return CPOINT(radius * cos(d), radius * sin(d));
}

double CAlgorithmTool::ToDegree(double value)
{
	return ((value * 180.0) / 3.1415926535897931);
}

CPOINT CAlgorithmTool::ToPolar(CPOINT p)
{
	return CPOINT(ToDegree(atan2(p.Y, (double)p.X)), sqrt(((double)p.X * (double)p.X) + (p.Y * p.Y)));
}

double  CAlgorithmTool::MembershipAdd(double d1, double d2)
{
	if (d1 < 0.0)
	{
		d1 = 0.0;
	}
	if (d2 < 0.0)
	{
		d2 = 0.0;
	}
	if (d1 > 1.0)
	{
		d1 = 1.0;
	}
	if (d2 > 1.0)
	{
		d2 = 1.0;
	}
	return (1.0 - ((1.0 - d1) * (1.0 - d2)));
}

double CAlgorithmTool::GetInnerData(const std::vector<double>& data, int first, int last, int index)
{
	return (data[first] + (((index - first) * (data[last] - data[first])) / ((double) (last - first))));
}

double CAlgorithmTool::GetInnerData(const std::vector<ushort>& data, int first, int last, int index)
{
	return (data[first] + (((1.0 * (index - first)) * (data[last] - data[first])) / ((double) (last - first))));
}

CPOINT CAlgorithmTool::GetMiddlePoint(CPOINT p1, CPOINT p2)
{
	CPOINT point = ToCartesian(p1.X, p1.Y);
	CPOINT point2 = ToCartesian(p2.X, p2.Y);
	return ToPolar(CPOINT((point.X + point2.X) / 2.0, (point.Y + point2.Y) / 2.0));
}

ushort CAlgorithmTool::GetMinValue(ushort* data, int dataLen, int index, int n)
{
	int num = index - n;
	if (num < 0)
	{
		num = 0;
	}
	int num2 = index + n;
	if (num2 >= dataLen)
	{
		num2 = dataLen - 1;
	}
	ushort num3 = 0xffff;
	for (int i = num; i <= num2; i++)
	{
		if (data[i] < num3)
		{
			num3 = data[i];
		}
	}
	return num3;
}

CPOINT CAlgorithmTool::GetTargetPos(CPOINT curPos, double angle, double speed, int span)
{
	double d = ToArc(curPos.X);
	double num2 = ToArc(angle);
	double num3 = speed * span;
	double x = (curPos.Y * cos(d)) + (num3 * cos(num2));
	double y = (curPos.Y * sin(d)) + (num3 * sin(num2));
	return CPOINT(ToDegree(atan2(y, x)), sqrt((x * x) + (y * y)));
}

double CAlgorithmTool::Interpolate(CPOINT p1, CPOINT p2, double angle)
{
	if (angle == p1.X)
	{
		return p1.Y;
	}
	if (angle == p2.X)
	{
		return p2.Y;
	}
	double num = ToArc(p1.X);
	double num2 = ToArc(p2.X);
	double num3 = ToArc(angle);
	double num4 = (p1.Y * sin(num3 - num)) + (p2.Y * sin(num2 - num3));
	return (((p1.Y * p2.Y) * sin(num2 - num)) / num4);
}

double CAlgorithmTool::IntersectAngle(double d1, double d2)
{
	if (d1 < 0.0)
	{
		d1 += 360.0;
	}
	if (d2 < 0.0)
	{
		d2 += 360.0;
	}
	return fabs((double) (d1 - d2));
}

bool CAlgorithmTool::IsInArea(CPolarArea lazy, CPolarArea area)
{
	CPOINT p(area.A1, area.R1);
	CPOINT point2(area.A2, area.R2);
	return (IsInArea(lazy, p) && IsInArea(lazy, point2));
}

bool CAlgorithmTool::IsInArea(CPolarArea area, CPOINT p)
{
	if (area.A2 > area.A1)
	{
		return ((((p.X >= area.A1) && (p.X <= area.A2)) && (p.Y > area.R1)) && (p.Y < area.R2));
	}
	if ((p.Y > area.R1) && (p.Y < area.R2))
	{
		if (p.X < area.A1)
		{
			return (p.X <= area.A2);
		}
		return true;
	}
	return false;
}

int CAlgorithmTool::LastValidDataIndex(const CAlgorithmInfo* info, const std::vector<double>& data)
{
	for (int i = data.size() - 1; i > 0; i--)
	{
		if ((data[i] > info->MinValue) && (data[i] < info->MaxValue))
		{
			return i;
		}
	}
	return -1;
}

int CAlgorithmTool::LastValidDataIndex(const CAlgorithmInfo* info, const std::vector<ushort>& data)
{
	for (int i = data.size() - 1; i > 0; i--)
	{
		if ((data[i] > info->MinValue) && (data[i] < info->MaxValue))
		{
			return i;
		}
	}
	return -1;
}

//�ַ�ָ��
std::vector<std::string> CAlgorithmTool::split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//��չ�ַ��Է������
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}


std::vector<ushort> CAlgorithmTool::ParseData(std::string msg)
{
	std::vector<ushort> result;
	try
	{
		std::vector<std::string> strVec = split(msg, ",");
		for (int i = 0; i < strVec.size(); i++)
		{
			ushort u16 = (ushort)atoi(strVec.at(i).c_str());
			result.push_back(u16);
		}
		return result;
	}
	catch (...)
	{
		result.clear();
		return result;
	}
	return result;
}

double CAlgorithmTool::NormalIntersect(double intersect)
{
	if (intersect > 180.0)
	{
		intersect = 360.0 - intersect;
	}
	if (intersect > 90.0)
	{
		intersect = 180.0 - intersect;
	}
	return intersect;
}


