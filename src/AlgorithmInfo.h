#pragma once
#include "IdentifyInfo.h"
#include "SegmentInfo.h"
#include <list>

typedef enum Train_Direction
{
	Left,
	Right,
	Both
}Train_Direction;

class CAlgorithmInfo
{
public:
	CAlgorithmInfo(void)
	{
		MaxValue = 0x4e20;
		MinValue = 800;
		StartAngle = 0.0;
		EndAngle = 180.0;
		Resolution = 0.5;
		Frequency = 50.0;
		//mod by yuan
		StandSpeed = 0.1;
		ExpiredTime = 1000;   //
		StandAlarmTime = 1000;      //<15ms
		OneLevelAlarmTime = 0x61ab;  //<25ms
		TrainDirection = Both;
		MinDetectSize = 50;
		MajorVersion = 0;
	}
	~CAlgorithmInfo(){}

	CAnimalIdentifyInfo AnimalIdentifyInfo;
	CAreaInfo AreaInfo;
	double EndAngle;
	int ExpiredTime;
	double Frequency;
	CLandslipIdentifyInfo LandslipIdentifyInfo;
	std::vector<CPolarArea> LazyArea;
	int MajorVersion;
	unsigned short MaxValue;
	int MinDetectSize;
	unsigned short MinValue;
	std::vector<CPolarArea> NonDetectedArea;
	CPersonIdentifyInfo PersonIdentifyInfo;
	double Resolution;
	unsigned int SerialNumber;
	int StandAlarmTime;
	int OneLevelAlarmTime;
	double StandSpeed;
	double StartAngle;
	CTrackInfo TrackInfo;
	CTrackInfo2 TrackInfo2;
	Train_Direction TrainDirection;
	CTrainIdentifyInfo TrainIdentifyInfo;
	//CTrack Track;
};

