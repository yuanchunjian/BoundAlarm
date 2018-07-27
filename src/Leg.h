#pragma once
#include "Segment.h"
#include "Block.h"
#include "CrowdBlock.h"

const int StandLength = 20;

class CLeg
{
public:
	CLeg(void);
	CLeg(int legType);
	virtual ~CLeg(void);
	void Add(CFrame* frame);
	double GetStepLength();
	int GetSteps();
	ushort LastTelNum;
	ushort StandTelNum;
	double TotalDistance;
private:
	CPOINT _firstPos;
	int _legType;
	CPOINT _standPos;
	bool _standPosInitFlag;
	std::list<double> _steps;
};

//-------------------------------------CLegAlgorithm--------------------------------------------
const int SingleStepLength = 300;

class CLegAlgorithm
{
public:
	CLegAlgorithm();
	CLegAlgorithm(CBlock* block);
	virtual ~CLegAlgorithm();
	double GetChance(double chance);
private:
	CBlock* _block;
	ushort _lastTelNum;
	CLeg _leftLeg;
	CLeg _rightLeg;
	
};

//-----------------------------CDoubleLegAlgorithm---------------------------------------
typedef struct
{
	CBlock* block;
	CLeg* leg;
}LEG_BLOCK;

class CDoubleLegAlgorithm
{
public:
	CDoubleLegAlgorithm(CCrowdBlock* block);
	~CDoubleLegAlgorithm();
	double GetChance(double chance);
	static ushort _TeleNum;
private:
	//inline bool CmpLastTeleNum(LEG_BLOCK legBlock1, LEG_BLOCK legBlock2);
	std::vector<LEG_BLOCK*> _vecLegBlock;
	bool _isAlarm;
};

