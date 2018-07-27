#pragma once
#include "Segment.h"

enum TrainPosition
{
	TrainUnknown,
	TrainNear,
	TrainFar
};

class CTrack :
	public CDoubleSegment
{
private:
	CTrack(CAlgorithmInfo* info);
	CSegment* CreateSegment();
	CAlgorithmInfo* _algorithmInfo;
public:
	virtual ~CTrack(void);
	void CalculateTrack();
	TrainPosition GetTrainPosition(const CPOINT* p);
	virtual bool IsInArea(int index, double value);
	bool IsInTrack(const CFrame* frame);
	static CTrack* CreateTrack(CAlgorithmInfo* info);

	double Direction;
	CSegment* InnerFar;
	CSegment* InnerNear;
	bool JudgeIsSingle();
};

