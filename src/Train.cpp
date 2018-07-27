
#include "Train.h"
#include "AlgorithmTool.h"
#include <liblogger/liblogger.h>

CTrain::CTrain(void)
{
}


CTrain::~CTrain(void)
{
}

CTrain::CTrain(CCrowdBlock* block)
{
	SickAlgorithm = block->SickAlgorithm;
	Name = SickAlgorithm->NameService->GetNewName("Train");
	Confirmed = true;
	_blocks.insert(_blocks.end(), block->_blocks.begin(), block->_blocks.end());
	Speed = block->Speed;
	JudgeStand(Speed);
	Direction = block->Direction;
	FirstTeleNum = block->FirstTeleNum;
	LastTeleNum = block->LastTeleNum;

	bool flag = (Direction > -90.0) && (Direction < 90.0);
	if (SickAlgorithm->Info->TrainIdentifyInfo.UpReverse)
	{
		flag = !flag;
	}
	LogDebug("Find Train, name: %s, speed: %.2f, direction: %.2f, first teleNum: %d, last teleNum: %d",
					Name.c_str(), Speed*3.6, Direction, FirstTeleNum, LastTeleNum);

	SickAlgorithm->EventInfo = new S_EventInfo;
	if (SickAlgorithm->EventInfo == NULL)
	{
       LogFatal("new event info abnomal, exit(0)");
       exit(0);
	}
	S_PassProtectionAreaPar* passAreaPar = new S_PassProtectionAreaPar;
	SickAlgorithm->EventInfo->eventPar = passAreaPar;

	SickAlgorithm->EventInfo->blockNameID = TRAIN;
	SickAlgorithm->EventInfo->eventTypeID = PASS_PROTECTION_AREA;

	passAreaPar->eventType = ENTER_PROTECTION_AREA;
	passAreaPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	passAreaPar->fSpeed = Speed*3.6;
	if (flag)
	{
		passAreaPar->unionDirection.ucDirection = TRAIN_LEFT;
		_runningDirection = TRAIN_LEFT;
	}
	else
	{
		passAreaPar->unionDirection.ucDirection = TRAIN_RIGHT;
		_runningDirection = TRAIN_RIGHT;
	}
	passAreaPar->fAngle = CurrentPos.X;
	passAreaPar->fRadialLen = CurrentPos.Y;
	passAreaPar->fRemainTime = 0.0;
	passAreaPar->enterFrameNO = LastTeleNum;
	passAreaPar->leaveFrameNO = 0;

	SickAlgorithm->m_pfEventInfoCallback(SickAlgorithm->EventInfo, SickAlgorithm->RetPar);
	if (SickAlgorithm->EventInfo != NULL)
	{
		if (SickAlgorithm->EventInfo->eventPar != NULL)
		{
			delete SickAlgorithm->EventInfo->eventPar;
			SickAlgorithm->EventInfo->eventPar = NULL;
		}
		delete SickAlgorithm->EventInfo;
		SickAlgorithm->EventInfo = NULL;
	}


	if (!SickAlgorithm->Track->JudgeIsSingle())
	{
		_trainPosition = SickAlgorithm->Track->GetTrainPosition(&block->CurrentPos);
	}
	SickAlgorithm->AlterTrainFlag(true);
}

void CTrain::BeforeRemoved()
{
	SickAlgorithm->EventInfo = new S_EventInfo;
	S_PassProtectionAreaPar* passAreaPar = new S_PassProtectionAreaPar;
	SickAlgorithm->EventInfo->eventPar = passAreaPar;

	SickAlgorithm->EventInfo->blockNameID = TRAIN;
	SickAlgorithm->EventInfo->eventTypeID = PASS_PROTECTION_AREA;

	passAreaPar->eventType = LEAVE_PROTECTION_AREA;
	passAreaPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	passAreaPar->fSpeed = Speed * 3.6;
	passAreaPar->unionDirection.ucDirection = _runningDirection;
	passAreaPar->fAngle = CurrentPos.X;
	passAreaPar->fRadialLen = CurrentPos.Y;
	passAreaPar->fRemainTime =  (CAlgorithmTool::GetInstance()->GetFrameDiff(FirstTeleNum, LastTeleNum) * 1000) / SickAlgorithm->Info->Frequency;
	passAreaPar->enterFrameNO = FirstTeleNum;
	passAreaPar->leaveFrameNO = LastTeleNum;
	SickAlgorithm->m_pfEventInfoCallback(SickAlgorithm->EventInfo, SickAlgorithm->RetPar);
	if (SickAlgorithm->EventInfo != NULL)
	{
		if (SickAlgorithm->EventInfo->eventPar != NULL)
		{
			delete SickAlgorithm->EventInfo->eventPar;
			SickAlgorithm->EventInfo->eventPar = NULL;
		}
		delete SickAlgorithm->EventInfo;
		SickAlgorithm->EventInfo = NULL;
	}

	//SickAlgorithm->TrainFlag = false;
	SickAlgorithm->AlterTrainFlag(false);

	CCrowdBlock::BeforeRemoved();
}

bool CTrain::CheckAlarm()
{
	IsAlarm = false;
	return false;
}

double CTrain::TrainProbability(CBlock* block)
{
	if (strcmp(block->GetObjectName(), "CCrowdBlock") != 0)
	{
		return 0.0;
	}
	
	if (strcmp(block->GetObjectName(), "CTrain") == 0)
	{
		return 0.99;
	}
	// block->SickAlgorithm->Info->TrainIdentifyInfo
	CCrowdBlock* block2 = dynamic_cast<CCrowdBlock*>(block);
	if (block2 == NULL || block2->_blocks.size() < block->SickAlgorithm->Info->TrainIdentifyInfo.MinSubCount)
	{
		return 0.0;
	}
	CPOINT middle = (*(block2->_blocks.begin()))->Frames[0]->Middle;
	if (((middle.X > 45.0)) && (middle.X < 135.0))
	{
		return 0.0;
	}
	double num = 0.0;
	if (block2->Speed > 0.0)
	{
		num = ((block2->Speed * CAlgorithmTool::GetInstance()->GetFrameDiff(block2->FirstTeleNum, block2->LastTeleNum)) * 1000.0) / block->SickAlgorithm->Info->Frequency;
	}
	if ((block->Length < 5000.0) || (num < 5000.0))
	{
		return 0.0;
	}
	double num2 = CAlgorithmTool::GetInstance()->NormalIntersect(CAlgorithmTool::GetInstance()->IntersectAngle(block->Direction, block->SickAlgorithm->Track->Direction));
	if (num2 > 15.0)
	{
		return 0.0;
	}
	double num3 = (0.6 * (90.0 - num2)) / 90.0;
	double valueOrDefault = block2->Speed;
	if (valueOrDefault > block->SickAlgorithm->Info->TrainIdentifyInfo.NormalSpeed)
	{
		valueOrDefault = block->SickAlgorithm->Info->TrainIdentifyInfo.NormalSpeed;
	}
	double num5 = 0.5 * (1.0 - (fabs((double)(valueOrDefault - block->SickAlgorithm->Info->TrainIdentifyInfo.NormalSpeed)) / block->SickAlgorithm->Info->TrainIdentifyInfo.NormalSpeed));
	if (num5 < 0.0)
	{
		num5 = 0.0;
	}
	num3 = CAlgorithmTool::GetInstance()->MembershipAdd(num3, num5);
	double num6 = ((((block2->_blocks.size() - block->SickAlgorithm->Info->TrainIdentifyInfo.MinSubCount) + 2) * 0.9) / 8.0) + 0.1;
	if (num6 > 0.9)
	{
		num6 = 0.9;
	}
	if (num6 < 0.0)
	{
		num6 = 0.0;
	}
	num3 = CAlgorithmTool::GetInstance()->MembershipAdd(num3, num6);
	if (num > 12000.0)
	{
		num3 = CAlgorithmTool::GetInstance()->MembershipAdd(num3, 0.5);
	}
	return num3;
}

double CTrain::Membership(const CFrame* frame)
{
	LogFuncEntry();
	try
	{
		if (SickAlgorithm->Track->JudgeIsSingle())
		{
			return 0.99;
		}
		if (Speed == 0.0)
		{
			return 0.0;
		}

		double standSpeed = SickAlgorithm->Info->StandSpeed;
		if (Speed > standSpeed)
		{
			return 0.99;
		}
		return CCrowdBlock::Membership(frame);
	}
	catch (...)
	{
		return 0.0;
	}
	LogFuncExit();
}

bool CTrain::IsCover(const CPOINT* p)
{
	return (SickAlgorithm->Track->JudgeIsSingle() || CCrowdBlock::IsCover(p));
}

void CTrain::Add(CFrame* frame)
{
	try
	{
		if (SickAlgorithm->Track->JudgeIsSingle())
		{
			Frames.push_back(frame);
			if (((SickAlgorithm->Track->Near->Data.size() == 0) || SickAlgorithm->Track->IsInTrack(frame)) && (frame->Length > 200.0))
			{
				LastTeleNum = frame->TeleNum;
			}
			if (Frames.size() > 0x7d0)
			{
				for (int i = 0; i < 0x3e8; i++)
				{
					CFrame* tmpFrame = *(Frames.begin());
					Frames.erase(Frames.begin());
					delete tmpFrame;
					tmpFrame = NULL;
				}
			}
		}
		else
		{
			CCrowdBlock::Add(frame);
		}
	}
	catch (...)
	{
	}
	LogTrace("Add frame to train, name: %s, speed: %.2f, direction: %.2f, first teleNum: %d, last teleNum: %d",
					Name.c_str(), Speed, Direction, FirstTeleNum, LastTeleNum);
}

bool CTrain::CanCombine(const CBlock* block)
{
	if (!SickAlgorithm->Track->JudgeIsSingle())
	{
		double standSpeed = SickAlgorithm->Info->StandSpeed;
		if (Speed < standSpeed)
		{
			return false;
		}
	}
	return true;
}

const char* CTrain::GetObjectName()
{
	return "Train";
}
