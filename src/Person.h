#pragma once
#include "CrowdBlock.h"
class CPerson :
	public CCrowdBlock
{
public:
	CPerson(void);
	virtual ~CPerson(void);
	CPerson(CBlock* block);
	virtual void Add(CFrame* frame);
	virtual void BeforeRemoved();
	virtual bool CanCombine(const CBlock* block);
	virtual bool CheckAlarm();
	virtual double Membership(const CFrame* frame);
	virtual const char* GetObjectName();
	virtual bool Discard(std::list<CBlock*>& blockList);

	static double DoubleBlockJuge(double chance, CCrowdBlock* crowd);
	static double SingleBlockJuge(CBlock* block, double chance);
	static double PersonProbability(CBlock* block);
	bool IsPerson(CBlock* b);
public:
	static inline bool CmpLastTeleNum(const CBlock* block1, const CBlock* block2);
	bool Uncertain;
};

