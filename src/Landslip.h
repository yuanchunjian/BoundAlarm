#pragma once
#include "Block.h"


class CLandslip :
	public CBlock
{
public:
	CLandslip(void);
	virtual ~CLandslip(void);
	CLandslip(CBlock* block);
	virtual void BeforeRemoved();
	virtual bool CanCombine(const CBlock* block);
	virtual bool CheckAlarm();
	virtual const char* GetObjectName();
	static double LandslipProbability(CBlock* block);
	static double GetMaxSingleLength(CBlock* block);
	static void CalDistanceAndDirection(const CBlock* block, CLargBlockInfo* largBlockInfo);
};

