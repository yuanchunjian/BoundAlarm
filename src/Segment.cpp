
#include "Segment.h"
#include "AlgorithmTool.h"
#include <algorithm>


CSegment::CSegment(void)
{
	Length = 0.0;
	SegInfo = new CSegmentInfo;
}


CSegment::~CSegment(void)
{
	if (SegInfo != NULL)
	{
		delete SegInfo;
		SegInfo = NULL;
	}
}

bool Cmp(CPOINT a, CPOINT b)
{
	if (a.X <= b.X)
	{
		return true;
	}
	return false;
}

bool CSegment::CalActualData(const CAlgorithmInfo* info, std::vector<CPOINT>& points, int offset, std::vector<ushort>& vecResult)
{
	if (points.size() < 2)
	{
		return false;
	}
	int num = (int) ((info->EndAngle - info->StartAngle) / info->Resolution);
	vecResult.resize(num);
	std::sort(points.begin(), points.end(), Cmp);
	
	int num2 = 0;
	double direction = 0.0;
	double num4 = 0.0;
	for (int i = 0; i < points.size(); i++)
	{
		CPOINT point = points.at(i);
		if (i == (points.size() - 1))
		{
			double num6 = (direction > 0.0) ? direction : (360.0 + direction);
			for (int j = num2; j < num; j++)
			{
				double num8 = info->StartAngle + (j * info->Resolution);
				if (num8 > num6)
				{
					vecResult.at(j) = 0xffff;
				}
				else
				{
					double num10 = sin(CAlgorithmTool::GetInstance()->ToArc((num8 + 180.0) - direction));
					if (fabs(num10) < 0.01)
					{
						vecResult.at(j) = 0xffff;
					}
					else
					{
						double d = fabs((double) (num4 / num10));
						if ( d > 65535.0)
						{
							vecResult.at(j) = 0xffff;
						}
						else
						{
							vecResult.at(j) = (ushort) d;
						}
					}
				}
				num2 = j;
			}
		}
		else
		{
			CPOINT point2 = points.at(i+1);
			direction = CAlgorithmTool::GetInstance()->GetDirection(point, point2);
			double num12 = (direction > 0.0) ? (direction - 180.0) : (180.0 + direction);
			num4 = fabs((double) (point.Y * sin(CAlgorithmTool::GetInstance()->ToArc((180.0 + point.X) - direction)))) + offset;
			double x = point2.X;
			if ((offset != 0) && (i < (points.size() - 2)))
			{
				CPOINT point3 = points[i + 2];
				double num14 = CAlgorithmTool::GetInstance()->GetDirection(point2, point3);
				CPOINT point4 = CAlgorithmTool::GetInstance()->GetTargetPos(point2, direction - 90.0, 1.0, offset);
				CPOINT point5 = CAlgorithmTool::GetInstance()->GetTargetPos(point2, num14 - 90.0, 1.0, offset);
				x = (point4.X + point5.X) / 2.0;
			}
			int index = num2;
			while (true)
			{
				if (index >= num)
				{
					break;
				}
				double num16 = info->StartAngle + (index * info->Resolution);
				if (num16 > x)
				{
					break;
				}
				if (num16 < num12)
				{
					vecResult.at(index) = 0xffff;
				}
				else
				{
					double num18 = sin(CAlgorithmTool::GetInstance()->ToArc((num16 + 180.0) - direction));
					if (fabs(num18) < 0.01)
					{
						vecResult.at(index) = 0xffff;
					}
					else
					{
						double num19 = fabs((double) (num4 / num18));
						if ((num19 > 65535.0))
						{
							vecResult.at(index) = 0xffff;
						}
						else
						{
							vecResult.at(index) = (ushort) num19;
						}
					}
				}
				num2 = index;
				index++;
			}
		}
	}
	return true;
}

CSegment* CSegment::CreateSegment(CSegmentInfo* info)
{
	CSegment* segment = new CSegment;
	segment->SegInfo = info;
	segment->ParseData();
	/*
	*/
	return segment;
}

bool CSegment::SetParameter()
{
	if (Data.size() < 2)
	{
		return false;
	}
	First = CPOINT(SegInfo->StartAngle, Data[0]);
	Last = CPOINT(SegInfo->StartAngle + ((Data.size() - 1) * SegInfo->Resolution),(double)Data.at(Data.size()-1));
	if (Length == 0.0)
	{

		CPOINT point1(SegInfo->StartAngle, (double)Data.at(0));
		CPOINT point2(SegInfo->StartAngle+ (Data.size() - 1)*SegInfo->Resolution, (double)Data.at(Data.size()-1));
		Length = CAlgorithmTool::GetInstance()->GetDistance(point1, point2);
	}

	if(Data.size() > 0)
	{
		Middle = CPOINT(CAlgorithmTool::GetInstance()->GetMiddlePoint(First, Last));
	}
	return true;
}


bool CSegment::GetMiddlePoint(double resolution, CPOINT& midPoint)
{
	if (Data.size() == 0)
	{
		return false;
	}
	int index = Data.size() / 2;
	midPoint.X = SegInfo->StartAngle + (index * resolution);
	midPoint.Y = (double) Data.at(index);
	return true;
}

bool CSegment::IsEmpty()
{
	if (Data.size() == 0)
	{
		return true;
	}
	return false;
}

 void CSegment::Normalize(const CAlgorithmInfo* info)
{
	int num = CAlgorithmTool::GetInstance()->FirstValidDataIndex(info, Data, 0);
	int num2 = CAlgorithmTool::GetInstance()->LastValidDataIndex(info, Data);
	if (((num != -1) && (num2 != -1)) && (num < num2))
	{
		for (int i = num; i < num2; i++)
		{
			if ((Data.at(i) < info->MinValue) || (Data.at(i) >= info->MaxValue))
			{
				double num4 = CAlgorithmTool::GetInstance()->GetInnerData(Data, i - 1, CAlgorithmTool::GetInstance()->FirstValidDataIndex(info, Data, i), i);
				Data.at(i) = (ushort)(num4);
			}
		}
	}
}

 void CSegment::ParseData()
{
	Data = CAlgorithmTool::GetInstance()->ParseData(SegInfo->Data);
}

void CSegment::SaveConfig()
{
	//SegInfo->Data = AlgorithmHelper.DataToString(this.Data);
}

int CSegment::ToIndex(const CAlgorithmInfo* info, double degree)
{
	if (Data.size() == 0)
	{
		return -1;
	}
	if ((degree < info->StartAngle) || (degree >= (info->StartAngle + (Data.size() * info->Resolution))))
	{
		return -1;
	}
	return (int)((degree - info->StartAngle) / info->Resolution);
}

double CSegment::GetAdjustedLength()
{
	double num2 = (min(First.Y, Last.Y) * (174.0 * SegInfo->Resolution)) / 10000.0;
	return double(max((double) (Length - num2), (double) 0.0));

}
//----------------------------CFrameʵ��-----------------------------------------------------------
CFrame::CFrame() : CSegment()
{
	Acceleration = 0.0;
	FirstDistance = 0;
	LastDistance = 0;
	Speed = 0.0;
	TeleNum = 0;
}

CFrame::~CFrame()
{

}

bool CFrame::SetParameter()
{
	return CSegment::SetParameter();
}

//----------------------------CDoubleSegmentʵ��-------------------------------------------------------

CDoubleSegment* CDoubleSegment::CreateDoubleSegment(CDoubleSegmentInfo* info)
{
	CDoubleSegment* segment = new CDoubleSegment;
	segment->_info = info;
	segment->ParseData();
	return segment;
}

bool CDoubleSegment::IsEmpty()
{
	if (!Near->IsEmpty() && !Far->IsEmpty())
	{
		return false;
	}
	return true;
}

bool CDoubleSegment::IsInArea(int index, double value)
{
	if (((index < 0) && (index >= Near->Data.size())))
	{
		return false;
	}
	return ((value > Near->Data[index]) && (value < Far->Data[index]));
}

void CDoubleSegment::Normalize(const CAlgorithmInfo* info)
{
	if ((Near->IsEmpty() && !Far->IsEmpty()))
	{
		Near->Normalize(info);
		Far->Normalize(info);
	}
}

void CDoubleSegment::ParseData()
{
	Near = CSegment::CreateSegment(_info->Near);
	Far = CSegment::CreateSegment(_info->Far);
}

void CDoubleSegment:: SaveConfig()
{
	Near->SaveConfig();
	Far->SaveConfig();
}

int CDoubleSegment::GetDataLength()
{
	if (Near == NULL)
	{
		return 0;
	}
	return Near->Data.size();
}

double CDoubleSegment::GetResolution()
{
	if (Near == NULL)
	{
		return 0.0;
	}
	return Near->SegInfo->Resolution;
}

double CDoubleSegment::GetStartAngle()
{
	if (Near == NULL)
	{
		return 0.0;
	}
	return Near->SegInfo->StartAngle;
}
