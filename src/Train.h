#pragma once
#include "CrowdBlock.h"
#include "Track.h"
class CTrain : 
	public CCrowdBlock
{
public:
	CTrain(void);
	virtual ~CTrain(void);
	CTrain(CCrowdBlock* block);
	virtual void BeforeRemoved();
	static double TrainProbability(CBlock* block);
	virtual bool CheckAlarm();
	virtual double Membership(const CFrame* frame);
	virtual bool IsCover(const CPOINT* p);
	virtual void Add(CFrame* frame);
	virtual const char* GetObjectName();
	virtual bool CanCombine(const CBlock* block);
private:
	TrainPosition _trainPosition;
	uchar _runningDirection;
};

