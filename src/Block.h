#pragma once
#ifndef _BLOCK_H
#define _BLOCK_H
#include "Segment.h"
#include "SickDataAlgorithm.h"
#include "LargBlockInfo.h"

class CBlock
{
public:
	CBlock(void);
	virtual ~CBlock(void);
	CBlock(CFrame* frame, CSickDataAlgorithm* algorithm);
	virtual void Add(CFrame* frame);
	virtual void BeforeRemoved();
	virtual bool CanCombine(const CBlock* block);
	virtual bool CheckAlarm();
	virtual bool GetCurArea(CPolarArea &polarArea);
	virtual bool IsCover(const CPOINT* p);
	virtual double Membership(const CFrame* frame);
	virtual const char* GetObjectName();

	void JudgeStand(double dbSpeed);
	void JudgeValidFrameNum();
	void SetIsAlarm(bool bVal);
	void AlarmInfoChange();
	virtual double GetLastAdjustedLength();
	void SetRemoved(bool bVal);
	CSickDataAlgorithm* SickAlgorithm;
	CPOINT GetExpectedPos(int deltaTelNum);
public:
	std::string Name;
	bool Removed;
	CPOINT AlarmPos;
	bool BeShield;
	bool Confirmed;
	CFrame* CurFrame;
	CPOINT CurrentPos;
	ushort FirstTeleNum;
	ushort LastTeleNum;
	CPOINT LeftPos;
	CPOINT RightPos;

	ushort AlarmTeleNum;
	double Direction;
	std::vector<CFrame*> Frames;
	int InvalidFrameNum;
	bool IsAlarm;
	//mod by xxf2
	bool LastAlarmStateType;
	ushort LastAlarmTelNum;
	
	double Speed;
	int ValidNum1;
	double Length;
	bool IsStand;
	double StandLength;
	CPOINT StandPos;
	ushort StandTelNum;
	ushort EnterTelNum;

	//uchar  AlarmLevel;
	bool bSendOneLevelAlarmFlag;
	CLargBlockInfo* LargBlockInfo;
};

#endif

