
#include "Track.h"
#include "AlgorithmTool.h"


CTrack::CTrack(CAlgorithmInfo* info)
{
	_algorithmInfo = info;
	Direction = 0.0;
	//IsSingle = false;
	InnerFar = NULL;
	InnerNear = NULL;
}

CTrack::~CTrack(void)
{
}

void CTrack::CalculateTrack()
{
	if (_algorithmInfo != NULL)	
	{
		InnerFar = NULL;
		InnerNear = NULL;
		Near = CreateSegment();
		bool bRet = CSegment::CalActualData(_algorithmInfo, _algorithmInfo->TrackInfo2.Near, 0, Near->Data);
		Far = CreateSegment();
		CSegment::CalActualData(_algorithmInfo, _algorithmInfo->TrackInfo2.Far, 0, Far->Data);
		if (Near->Data.size() == 0)
		{
			CSegment::CalActualData(_algorithmInfo, _algorithmInfo->TrackInfo2.Far, -1435, Near->Data);
		}
		else if (Far->Data.size() == 0)
		{
			CSegment::CalActualData(_algorithmInfo, _algorithmInfo->TrackInfo2.Near, 0x59b, Far->Data);
		}
		else
		{
			int iNearSize = _algorithmInfo->TrackInfo2.Near.size();	
			int iFarSize = _algorithmInfo->TrackInfo2.Far.size();	
			if (iNearSize > 1)
			{
				Direction = CAlgorithmTool::GetInstance()->GetDirection(_algorithmInfo->TrackInfo2.Near.at(0), _algorithmInfo->TrackInfo2.Near.at(iNearSize-1));
			}
			else if (iFarSize > 1)
			{
				Direction = CAlgorithmTool::GetInstance()->GetDirection(_algorithmInfo->TrackInfo2.Far.at(0), _algorithmInfo->TrackInfo2.Far.at(iFarSize-1));
			}
			InnerNear = CreateSegment();
			CSegment::CalActualData(_algorithmInfo, _algorithmInfo->TrackInfo2.Near, 0x59b, InnerNear->Data);
			InnerFar = CreateSegment();
			CSegment::CalActualData(_algorithmInfo, _algorithmInfo->TrackInfo2.Far, -1435, InnerFar->Data);
			for (int i = 0; i < Near->Data.size(); i++)
			{
				if (InnerFar->Data[i] < InnerNear->Data[i])
				{
					ushort num2;
					InnerFar->Data[i] = (ushort)(num2 = 0xffff);
					InnerNear->Data[i] = num2;
				}
			}
		}
	}
}

CSegment* CTrack::CreateSegment()
{
	double num1 = (_algorithmInfo->EndAngle - _algorithmInfo->StartAngle) / _algorithmInfo->Resolution;
	CSegment* Seg = new CSegment;
	Seg->SegInfo->StartAngle = _algorithmInfo->StartAngle;
	Seg->SegInfo->Resolution = _algorithmInfo->Resolution;
	return Seg;
}

CTrack* CTrack::CreateTrack(CAlgorithmInfo* info)
{
	CTrack* track = new CTrack(info);
	track->CalculateTrack();
	return track;
}

TrainPosition CTrack::GetTrainPosition(const CPOINT* p)
{
	enum TrainPosition tmpPos;
	if ((!JudgeIsSingle() && (InnerNear != NULL)) && (InnerNear->Data.size() != 0))
	{
		int index = static_cast<int>((double)((p->X - Near->SegInfo->StartAngle) / Near->SegInfo->Resolution));
		if (((index > 0) && (index < InnerNear->Data.size())) && (p->Y < (InnerNear->Data[index] + 500)))
		{
			return TrainUnknown;
		}
		if (((index > 0) && (index < InnerFar->Data.size())) && (p->Y > (InnerFar->Data[index] - 500)))
		{
			return TrainFar;
		}
	}
	return TrainUnknown;
}

bool CTrack::IsInArea(int index, double value)
{
	return CDoubleSegment::IsInArea(index, value);
}

bool CTrack::IsInTrack(const CFrame* frame)
{
	int num = (int)((frame->SegInfo->StartAngle - Near->SegInfo->StartAngle) / frame->SegInfo->Resolution);
	if (num >= 0)
	{
		for (int i = 0; i < frame->Data.size(); i++)
		{
			if (IsInArea(num + i, (double)frame->Data[i]))
			{
				return true;
			}
		}
	}
	return false;
}

bool CTrack::JudgeIsSingle()
{
	return (InnerNear == NULL);
}
