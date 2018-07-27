#pragma once
#include "Block.h"
//#include "SickDataAlgorithm.h"
#include "CrowdBlock.h"
#include "Person.h"

class CPartitionInfo
{
public:
	CPartitionInfo()
	{
		PartitionRatio = 0.1;
		NewBlockGrade = 0.5;
	}
	~CPartitionInfo(){}
	double NewBlockGrade;
	double PartitionRatio;
};

class CPartitionAlgorithm
{
public:
	CPartitionAlgorithm(void);
	CPartitionAlgorithm(CPartitionInfo info);
	virtual ~CPartitionAlgorithm(void);
	void CheckAlarm();
	void CombineBlocks();
	void Reset(bool bResume = true);
	void RemoveExpiredBlocks(ushort teleNum);
	void Partition(const std::vector<ushort>& matter, double startAngle, ushort teleNum);

	int ToFrames(const std::vector<ushort>& matter, double startAngle, ushort teleNum, std::vector<CFrame*>& result);
	CSickDataAlgorithm* SickAlgorithm;
private:
	
	void Decompose();
	void DistributeFrames(std::vector<CFrame*>& frames);
	void Distinguish(const std::vector<CFrame*>& frames, ushort teleNum);
	std::list<CBlock*> _blocks;
	//std::list<CCrowdBlock*> _crowdBlocks;
    bool _clearFlag;
	CPartitionInfo _info;
	//CPerson _obPerson;
};

