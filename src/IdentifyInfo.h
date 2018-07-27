#ifndef _IdentifyInfo_H
#define _IdentifyInfo_H

/*-----------------------------CAnimalIdentifyInfo---------------------------------------*/
class CAnimalIdentifyInfo
{
public:
	CAnimalIdentifyInfo()
	{
		MinIdentifyFrames = 200;
		MaxSpeed = 15.0;
		MaxLength = 300;
		Probability = 0.8;
		StandAlarmTime = 1.0;
	}
	~CAnimalIdentifyInfo()
	{

	}
	int MaxLength;
	double MaxSpeed;
	int MinIdentifyFrames;
	double Probability;
	double StandAlarmTime;
};
/*-----------------------------CLandslipIdentifyInfo---------------------------------------*/
class CLandslipIdentifyInfo
{
public:	
	CLandslipIdentifyInfo()
	{
		MinIdentifyFrames = 50;
		MinLength = 0x5dc;
		Probability = 0.8;
		MinDistance = 200;
		_identifyTime = 2;
	}
	~CLandslipIdentifyInfo()
	{

	}
	int GetIdentifyTime()
	{
		return _identifyTime;
	}
	void SetIdentifyTime(int val)
	{
		if (val < 2)
		{
			_identifyTime = 2;
		}
		else if (val > 15)
		{
			_identifyTime = 15;
		}
		else
		{
			_identifyTime = val;
		}
        
		MinIdentifyFrames = val * 0x19;
	}
	int MinDistance;
	int MinIdentifyFrames;
	int MinLength;
	double Probability;
private:
	int _identifyTime;
};
/*-----------------------------CPersonIdentifyInfo---------------------------------------*/
class CPersonIdentifyInfo
{
public:
	CPersonIdentifyInfo()
	{
		MinIdentifyFrames = 50;
		MaxSpeed = 3.0;
		MaxLength = 0x5dc;
		MinLength = 100;
		Probability = 0.8;
		StandAlarmTime = 1.0;
	}
	~CPersonIdentifyInfo()
	{

	}

	int MaxLength;
	double MaxSpeed;
	int MinIdentifyFrames;
	int MinLength;
	double Probability;
	double StandAlarmTime;
};
/*-----------------------------CTrainIdentifyInfo---------------------------------------*/
class CTrainIdentifyInfo
{
public:
	CTrainIdentifyInfo()
	{
		Probability = 0.8;
		MinSubCount = 5;
		UpReverse = false;
		NormalSpeed = 14.0;
	}
	~CTrainIdentifyInfo()
	{

	}
	int MinSubCount;
    double NormalSpeed;
    double Probability;
    bool UpReverse;
};

#endif