
#include "Block.h"
#include "AlgorithmTool.h"
#include <liblogger/liblogger.h>
#include "PublicHead.h"

extern int RF_flag;
CBlock::CBlock(void)
{
	IsAlarm = false;
	Removed = false;
	IsStand = false;
	Speed = 0.0;
	 ValidNum1 = 0;
	 Direction  = -1.0;
	 BeShield = false;
	 Confirmed = false;
	 SickAlgorithm = NULL;
	 LargBlockInfo = NULL;
	 bSendOneLevelAlarmFlag = false;
	// AlarmLevel = TWO_LEVEL_ALARM;
	 CurFrame = NULL;
	 StandLength = 0;
	 EnterTelNum = 0;
	 //mod by xxf2
	 LastAlarmStateType = false;
	 LastAlarmTelNum = 0;
}

CBlock::~CBlock(void)
{
	for (uint i = 0; i < Frames.size(); i++)
	{
		if (Frames.at(i) != NULL)
		{
			delete Frames.at(i);
			Frames.at(i) = NULL;
		}
	}
	Frames.clear();
	std::vector<CFrame*>().swap(Frames);
	if (LargBlockInfo != NULL)
	{
	   delete LargBlockInfo;
	   LargBlockInfo = NULL;
	}
}

CBlock::CBlock(CFrame* frame, CSickDataAlgorithm* algorithm)
{
	try
	{
		 BeShield = false;
		IsAlarm = false;
		Removed = false;
		IsStand = false;
		Confirmed = false;
		bSendOneLevelAlarmFlag = false;
		//AlarmLevel = TWO_LEVEL_ALARM;
		SickAlgorithm = NULL;
		LargBlockInfo = NULL;
		CurFrame = NULL;
		StandLength = 0;
		Speed = 0.0;
		ValidNum1 = 0; 
		EnterTelNum = 0;
		SickAlgorithm = algorithm;
		Direction = -1.0;
		//mod by xxf2
		LastAlarmStateType = false;
		LastAlarmTelNum = 0;
		
		Frames.push_back(frame);
		Name = algorithm->NameService->GetNewName("Block");
		Length = frame->Length;
		FirstTeleNum = LastTeleNum = frame->TeleNum;
		
		CurrentPos = frame->Middle;
	
		CurFrame = frame;
		
		LeftPos = frame->First;
		RightPos = frame->Last;
		LogDebug("Find block, name: %s, length: %.2f, teleNum: %d, leftPos: x->%.2f y->%.2f, rigthPos: x->%.2f y->%.2f",
				Name.c_str(), Length, FirstTeleNum, LeftPos.X, LeftPos.Y, RightPos.X, RightPos.Y);
	}
	catch (...)
	{
		LogFatal("build block abnomal!");
		exit(ABNORMAL_EXIT);
	}
}

void CBlock::Add(CFrame* frame)
{
	try
	{
		if ((LastTeleNum != frame->TeleNum) && ((Frames.size() <= 0) || (CAlgorithmTool::GetInstance()->GetFrameDiff(LastTeleNum, frame->TeleNum) <= 0x3e8)))
		{
			Frames.push_back(frame);
			LastTeleNum = frame->TeleNum;
			Length = max(Length, frame->Length);
		
			CurrentPos = frame->Middle;
			if (Frames.size() > 1)
			{
				int num = Frames.size() - 5;
				if (num < 0)
				{
					num = 0;
				}
				CFrame* frame2 = Frames.at(num);
				CFrame* frame3 = Frames.at(Frames.size() - 1);
				double distance = CAlgorithmTool::GetInstance()->GetDistance(frame2->Middle, frame3->Middle);
				double num3 = ((double) (CAlgorithmTool::GetInstance()->GetFrameDiff(frame2->TeleNum, frame3->TeleNum) * 0x3e8)) / SickAlgorithm->Info->Frequency;
				Speed = distance / num3;
				//JudgeStand(Speed);
				JudgeValidFrameNum();

				Direction = CAlgorithmTool::GetInstance()->GetDirection(frame2->Middle, frame3->Middle);
				frame2 = Frames.at(Frames.size() - 2);
				distance = CAlgorithmTool::GetInstance()->GetDistance(frame2->Middle, frame3->Middle);
				num3 = ((double) (CAlgorithmTool::GetInstance()->GetFrameDiff(frame2->TeleNum, frame3->TeleNum) * 0x3e8)) / SickAlgorithm->Info->Frequency;
				frame2->Speed = distance / num3;
				frame2->FirstDistance = CAlgorithmTool::GetInstance()->GetDistance(frame2->First, frame3->First);
				frame2->LastDistance = CAlgorithmTool::GetInstance()->GetDistance(frame2->Last, frame3->Last);
				if (Frames.size() > 2)
				{
					frame3 = Frames.at(Frames.size() - 3);
					frame3->Acceleration = frame2->Speed- frame3->Speed; //���ٶȼ��㣬������Ҫ����
				}
			}
			if (Frames.size() > 0x7d0)//0x7d0
			{
				for (int i = 0; i < 0x3e8; i++)//
				{
					CFrame* tmpFrame = *(Frames.begin());
					Frames.erase(Frames.begin());
					delete tmpFrame;
					tmpFrame = NULL;
				}
			}
			CurFrame = frame;
			
			LeftPos = frame->First;
		
			RightPos = frame->Last;
			LogDebug("Add frame to block, blockname: %s, first tele num: %d, last tele num: %d, speed: %.2f,leftPos: x->%.2f y->%.2f, rigthPos: x->%.2f y->%.2f",
							Name.c_str(), FirstTeleNum, LastTeleNum, Speed, LeftPos.X, LeftPos.Y, RightPos.X, RightPos.Y);
		}
	}
	catch (...)
	{
		exit(ABNORMAL_EXIT);
	}
}

void CBlock::BeforeRemoved()
{
	SetIsAlarm(false);
}

bool CBlock::CanCombine(const CBlock* block)
{
	try
	{
		double num2;
		
		double standSpeed = SickAlgorithm->Info->StandSpeed;
			if (Speed >= standSpeed)
			{
				double speed = block->Speed;
				double num8 = standSpeed;
				if (!(speed < num8 ))
				{
					goto Label_00B2;
				}
			}
		
		return false;
Label_00B2:
		num2 = 0.6 * (1.0 - (fabs((double) (Speed - block->Speed)) / Speed));
		if (num2 < 0.0)
		{
			return false;
		}
		if (num2 < 0.0)
		{
			num2 = 0.0;
		}
		double num3 = CAlgorithmTool::GetInstance()->IntersectAngle(Direction, block->Direction);
		if (num3 > 180.0)
		{
			num3 = 360.0 - num3;
		}
		double num4 = 0.7 * (1.0 - (num3 / 30.0));
		if (num4 < 0.0)
		{
			return false;
		}
		if (num4 < 0.0)
		{
			num4 = 0.0;
		}
		num2 = CAlgorithmTool::GetInstance()->MembershipAdd(num2, num4);
		double distance = CAlgorithmTool::GetInstance()->GetDistance(CurrentPos, block->CurrentPos);
		double num6 = Speed * 1000.0;
		if (distance > num6)
		{
			return false;
		}
		double num7 = 0.99 * (1.0 - (distance / num6));
		return (CAlgorithmTool::GetInstance()->MembershipAdd(num2, num7) > 0.8);
	}
	catch (...)
	{
		LogError("Find error, default return false");
		return false;
	}
}

bool CBlock::CheckAlarm()
{
	try{
		    bool bAlarm = IsAlarm;
			/*if (BeShield)
			{
				EnterTelNum = LastTeleNum;
			}*/
		   // mod by xxf
		    if (EnterTelNum > 0)
		    {
		    	double num = (CAlgorithmTool::GetInstance()->GetFrameDiff(EnterTelNum, LastTeleNum) * 1000.0) / SickAlgorithm->Info->Frequency;
		    	bAlarm= num > SickAlgorithm->Info->StandAlarmTime;
		    	return bAlarm;
		    }
		    else
		    {
		    	return false;
		    }
	}catch(...)
	{
		LogError("catch abnomal");
		return false;
	}
}

bool CBlock::GetCurArea(CPolarArea& polarArea)
{
	if (Frames.size() == 0)
	{
        LogError("Frames size = 0, CurArea is NULL");
        return false;
	}
	CFrame* frame = Frames.at(Frames.size() - 1);
	CPOINT point = frame->First;
	CPOINT point2 = frame->Last;

	polarArea.A1 = point.X;
	polarArea.A2 = point2.X;
	polarArea.R1 = min(point.Y, point2.Y);
	polarArea.R2 = max(point.Y, point2.Y);
	return true;
}

CPOINT CBlock::GetExpectedPos(int deltaTelNum)
{
	if (Direction > 0 && Speed > 0)
	{
		return CAlgorithmTool::GetInstance()->GetTargetPos(CurrentPos, Direction, Speed, (int) (((double) (deltaTelNum * 0x3e8)) / SickAlgorithm->Info->Frequency));
	}
	return CurrentPos;
}

bool CBlock::IsCover(const CPOINT* p)
{
	if (CurFrame == NULL)
	{
		return false;
	}
	if ((p->X < CurFrame->First.X) || (p->X > CurFrame->Last.X))
	{
		return false;
	}
	if (p->Y <= CurFrame->First.Y)
	{
		return (p->Y > CurFrame->Last.Y);
	}
	return true;
}

double CBlock::Membership(const CFrame* frame)
{
	try
	{
		if ((LastTeleNum == frame->TeleNum) )
		{
			return 0.0;
		}
		CPOINT point = frame->Middle;
		int frameDiff = CAlgorithmTool::GetInstance()->GetFrameDiff(LastTeleNum, frame->TeleNum);
		
		CPOINT expectedPos = GetExpectedPos(frameDiff);
		double num2 = 50000.0 / SickAlgorithm->Info->Frequency; //<12.5s
		
		if (Speed == 0)
		{
			num2 *= 2.0;
		}
	
		double distance = CAlgorithmTool::GetInstance()->GetDistance(point, expectedPos);
		if ((distance > num2))
		{
			return 0.0;
		}
		double num4 = (1.0 - (distance / num2)) * 0.9;
		double num5 = fabs((double) (frame->Length - Length)) / Length;
		num5 = (1.0 - num5) * 0.1;
		return CAlgorithmTool::GetInstance()->MembershipAdd(num4, num5);
	}
	catch (...)
	{
		LogError("Find error, default return 0.0");
		return 0.0;
	}
}

void CBlock::SetIsAlarm(bool bVal)
{
	//mod by xxf2
	//if (bVal != IsAlarm)
	if (bVal != IsAlarm || IsAlarm)
	{
		IsAlarm = bVal;
		LogDebug("Alarm change IsAlarm = %d", IsAlarm);
		if (IsAlarm /*&& AlarmLevel == TWO_LEVEL_ALARM*/)
		{
			LogDebug("赋值当前位置");
			AlarmPos = CurrentPos;
			LogDebug("赋值最后桢 LastTeleNum = %d", LastTeleNum);
			AlarmTeleNum = LastTeleNum;
		//	AlarmLevel = TWO_LEVEL_ALARM;
		}
		AlarmInfoChange();
	}

	/*if (IsAlarm && !bSendOneLevelAlarmFlag)
	{
		double num = (CAlgorithmTool::GetInstance()->GetFrameDiff(AlarmTeleNum, LastTeleNum) * 1000.0) / SickAlgorithm->Info->Frequency;
		bool flag = num > SickAlgorithm->Info->OneLevelAlarmTime;
		if (flag)
		{
			AlarmLevel = ONE_LEVEL_ALARM;
			AlarmInfoChange();
			bSendOneLevelAlarmFlag = true;
		}
	}*/

}

void CBlock::AlarmInfoChange()
{
	LogDebug("Enter AlarmInfoChange");
	CAlarmInfo *alarmInfo = NULL;
	alarmInfo = new CAlarmInfo();
	if (alarmInfo == NULL)
	{
		LogError("new CAlarmInfo fail, name = %s", Name.c_str());
		return;
	}
	switch(Name.at(0))
	{
	case 'B':
		alarmInfo->EventInfo->blockNameID = COMMON_BLOCK;
		break;
	case 'P':
		alarmInfo->EventInfo->blockNameID = PERSON;
		break;
	case 'L':
		alarmInfo->EventInfo->blockNameID = LANDSLIP;
		break;
	default:
		//mod by xxf
		alarmInfo->EventInfo->blockNameID = DEFAULT_BLOCK;
		break;
	}

	alarmInfo->EventInfo->eventTypeID = FIND_BLOCK_ALEAR;
	alarmInfo->blockPar->abnormalPoint = NORMAL_POINT;
	alarmInfo->blockPar->ID = -1;
	//StandPos = CurrentPos;
	//StandLength = GetLastAdjustedLength();
	if (Length == 0)
	{
		alarmInfo->blockPar->fBlockLen = SickAlgorithm->Info->MinDetectSize;
	}
	else
	{
		alarmInfo->blockPar->fBlockLen = Length;
	}
	alarmInfo->blockPar->frameNO = LastTeleNum;
	alarmInfo->blockPar->rfidFlag = RF_flag;
	if (IsAlarm)
	{
		alarmInfo->blockPar->fAngle = static_cast<float>(AlarmPos.X);
		alarmInfo->blockPar->fRadialLen = static_cast<float>(AlarmPos.Y);
		alarmInfo->blockPar->eventType = TWO_LEVEL_ALARM;
		alarmInfo->blockPar->alarmStateType = Alarm;
		alarmInfo->blockPar->fRemainTime = 0.0;
		//add RFID_flag
		alarmInfo->blockPar->rfidFlag = RF_flag;
		bool bRet = GetCurArea(alarmInfo->curArea);
		if (!bRet)
		{
			LogError("取当前面积失败");
		}
		LogInfo("Find alarm, name: %s", Name.c_str());
	}
	else
	{
		alarmInfo->blockPar->alarmStateType = Resume;
		alarmInfo->blockPar->fRemainTime = (float) ((CAlgorithmTool::GetInstance()->GetFrameDiff(AlarmTeleNum, LastTeleNum)*1000)/
			SickAlgorithm->Info->Frequency);
		LogInfo("alarm remove, name: %s", Name.c_str());

	}
	//mod by xxf2
	//SickAlgorithm->AlarmChange(Name, alarmInfo);
	if (LastAlarmStateType != IsAlarm || LastTeleNum - LastAlarmTelNum >= SickAlgorithm->Info->Frequency * 0.5)
	{
		LastAlarmTelNum = LastTeleNum;
		LastAlarmStateType = IsAlarm;
		SickAlgorithm->AlarmChange(Name, alarmInfo);
	}
    if (alarmInfo != NULL && alarmInfo->DeleteFlag)
    {
    	delete alarmInfo;
    	alarmInfo = NULL;
    }

}
double CBlock::GetLastAdjustedLength()
{
	   if (Frames.empty())
	   {
		  return 0.0;
	   }
		CFrame* frame = Frames.at(Frames.size() - 1);
		
		return frame->Length;
}

void CBlock::SetRemoved(bool bVal)
{
	if (bVal != Removed)
	{
		Removed = bVal;
		if (Removed)
		{
			BeforeRemoved();
		}
	}
}
void CBlock::JudgeStand(double dbSpeed)
{
	if (IsStand)
	{
		if ((CAlgorithmTool::GetInstance()->GetDistance(CurrentPos, StandPos) > 500.0))
		{
			if (InvalidFrameNum++ > 10)
			{
				IsStand = false;
				ValidNum1 = 0;
			}
		}
		else
		{
			InvalidFrameNum = 0;
		}
	}
	else
	{
		if (dbSpeed < SickAlgorithm->Info->StandSpeed)
		{
			if (GetLastAdjustedLength() > SickAlgorithm->Info->MinDetectSize)
			{
				ValidNum1++;
			}
			else
			{
				ValidNum1 = 0;
			}
			if (ValidNum1 > 20)
			{
				StandTelNum = LastTeleNum;
				StandPos = CurrentPos;
				StandLength = GetLastAdjustedLength();
				IsStand = true;
				InvalidFrameNum = 0;
			}
		}
	}
	
}

void CBlock::JudgeValidFrameNum()
{
#define VALID_NUM 10//mod by xxf2
	if (ValidNum1 <= VALID_NUM )
	{
		if (GetLastAdjustedLength() > SickAlgorithm->Info->MinDetectSize)
		{
			ValidNum1++;
			if (ValidNum1 > VALID_NUM)
			{
				StandPos = CurrentPos;
				StandLength = GetLastAdjustedLength();
				EnterTelNum = LastTeleNum;
			}
		}
		else
		{
			ValidNum1 = 0;
		}
	}
}

const char* CBlock::GetObjectName()
{
	return "CBlock";
}
