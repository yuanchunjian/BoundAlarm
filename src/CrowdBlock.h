#pragma once
#include "Block.h"
class CCrowdBlock :
	public CBlock
{
public:
	CCrowdBlock(void);
	CCrowdBlock(CBlock* b1, CBlock* b2);
	virtual ~CCrowdBlock(void);
	void CalCurrentPos();
	virtual void Add(CFrame* frame);
	void AfterAddFrame();
	virtual double Membership(const CFrame* frame);
	virtual bool IsCover(const CPOINT* p);
	virtual bool Discard(std::list<CBlock*>& blockList);
	virtual void Combine(CBlock* block);
	virtual bool GetCurArea(CPolarArea& area2);
	virtual const char* GetObjectName();
	virtual double GetLastAdjustedLength();
	int GetBlocksCounts();
	std::list<CBlock*> _blocks;
	CPOINT FirstPos;
	void* Tag;
protected:
	void CalSpeed();
	void RemoveExpiredBlocks(ushort teleNum);
	CPOINT _lastPos;
	CPOINT _firstPos;
};

