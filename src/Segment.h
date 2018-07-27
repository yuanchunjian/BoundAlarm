#pragma once
#include "SegmentInfo.h"
#include "AlgorithmInfo.h"
#include "CustomType.h"
#include <algorithm>
using namespace std;

class CSegment
{
public:
	CSegment(void);
	virtual ~CSegment(void);
	static bool CalActualData(const CAlgorithmInfo* info, std::vector<CPOINT>& points, int offset, std::vector<ushort>& vecResult);
	static CSegment* CreateSegment(CSegmentInfo* info);
	bool GetMiddlePoint(double resolution, CPOINT& retPoint);
	bool IsEmpty();
	 void Normalize(const CAlgorithmInfo* info);
	 virtual void ParseData();
	 int ToIndex(const CAlgorithmInfo* info, double degree);
	 double GetAdjustedLength();
	 virtual bool SetParameter();
	 virtual void SaveConfig();

	std::vector<ushort> Data;
	double AdjustedLength;

public:
	CSegmentInfo* SegInfo;
	CPOINT First;
	CPOINT Last;
	double Length;
	CPOINT Middle;
};

class CFrame : 
	public CSegment
{
public:
	CFrame();
	~CFrame();
	virtual bool SetParameter();
	double Acceleration;
	double FirstDistance;
	double LastDistance;
	double Speed;
	ushort TeleNum;
};

class CDoubleSegment
{
public:
	static CDoubleSegment* CreateDoubleSegment(CDoubleSegmentInfo* info);

	bool IsEmpty();

	virtual bool IsInArea(int index, double value);

	void Normalize(const CAlgorithmInfo* info);

	virtual void ParseData();

	virtual void SaveConfig();
	int GetDataLength();

	CSegment* Far;

	CSegment* Near;

	double GetResolution();

	double GetStartAngle();
protected:
	CDoubleSegmentInfo* _info;
};

class CProtectArea : 
	public CDoubleSegment
{
public:
	CProtectArea(){}
	CProtectArea(CAreaInfo* info)
	{
		_info = info;
	}
	~CProtectArea(){}
	static CProtectArea* CreateProtectArea(CAreaInfo* info)
	{
		CProtectArea* area = new CProtectArea(info);
		area->ParseData();
		return area;
	}
};

class CUnDetectedArea : 
	public CSegment
{
public:
	static CUnDetectedArea* CreateArea(CAlgorithmInfo* info)
	{
		CUnDetectedArea* area = new CUnDetectedArea();
		area->ReCalArea(info);
		return area;
	}

	bool IsInArea(int index, double value)
	{
		if ((index < 0) || (index >= (int)Data.size()))
		{
			return false;
		}
		ushort num = Data[index];
		if (num < 100)
		{
			return false;
		}
		return (value > num);
	}

	void ReCalArea(CAlgorithmInfo* info)
	{
		Data.clear();
		std::vector<CPolarArea> nonDetectedArea = info->NonDetectedArea;
		if (nonDetectedArea.size() != 0)
		{
			double num = 9999999;
			double num2 = 0.0;
			for (int j = 0; j < (int)nonDetectedArea.size(); j++)
			{
				if (nonDetectedArea.at(j).A1 < num)
				{
					num = nonDetectedArea.at(j).A1;
				}
				if (nonDetectedArea.at(j).A2 > num2)
				{
					num2 = nonDetectedArea.at(j).A2;
				}
			}
	
			SegInfo->StartAngle = num;
			SegInfo->Resolution = info->Resolution;
			int num3 = ((int) ((num2 - num) / SegInfo->Resolution)) + 1;
			Data.resize(num3);
			for (uint i = 0; i< nonDetectedArea.size(); i++)
			{
				CPolarArea area = nonDetectedArea.at(i);
				int index = (int) ((area.A1 - SegInfo->StartAngle) / SegInfo->Resolution);
				
				ushort num5 = (ushort)(min(area.R1, area.R2));
				Data[index] = num5;
				int num6 = (int) ((area.A2 - SegInfo->StartAngle) / SegInfo->Resolution);
				
				for (int i = index; i <= num6; i++)
				{
					Data[i] = num5;
				}
			}
		}
	}
};
