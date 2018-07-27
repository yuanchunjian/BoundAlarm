
#include "Person.h"
#include "AlgorithmTool.h"
#include "Leg.h"
#include <algorithm>
#include <liblogger/liblogger.h>


CPerson::CPerson(void)
{
}


CPerson::~CPerson(void)
{
}

CPerson::CPerson(CBlock* block)
{
	SickAlgorithm = block->SickAlgorithm;
	Name = SickAlgorithm->NameService->GetNewName("Person");
	Confirmed = true;
	if (strcmp(block->GetObjectName(), "CCrowdBlock") == 0)
	{
		CCrowdBlock* crowd = dynamic_cast<CCrowdBlock*>(block);
		if (crowd != NULL)
		{
		    _blocks.insert(_blocks.end(), crowd->_blocks.begin(), crowd->_blocks.end());
		}
	}
	else
	{
		_blocks.push_back(block);
	}
	LastTeleNum = block->LastTeleNum;
	FirstTeleNum = block->FirstTeleNum;
	Length = block->Length;
	CurrentPos = block->CurrentPos;
	FirstPos = _lastPos = CurrentPos;
	Speed = block->Speed;
	JudgeStand(Speed);
	Direction = block->Direction;

	LogDebug("Find person, name: %s, speed: %.2f, direction: %.2f, length: %.2f, first teleNum: %d, last teleNum: %d",
						Name.c_str(), Speed, Direction, Length, FirstTeleNum, LastTeleNum);

	SickAlgorithm->EventInfo = new S_EventInfo;
	S_PassProtectionAreaPar* passAreaPar = new S_PassProtectionAreaPar;
	SickAlgorithm->EventInfo->eventPar = passAreaPar;

	SickAlgorithm->EventInfo->blockNameID = PERSON;
	SickAlgorithm->EventInfo->eventTypeID = PASS_PROTECTION_AREA;

	passAreaPar->eventType = ENTER_PROTECTION_AREA;
	passAreaPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	passAreaPar->fSpeed = Speed;
	passAreaPar->unionDirection.fDirection = Direction;
	passAreaPar->fAngle = CurrentPos.X;
	passAreaPar->fRadialLen = CurrentPos.Y;
	passAreaPar->fRemainTime = 0.0;
	passAreaPar->enterFrameNO = FirstTeleNum;
	passAreaPar->leaveFrameNO = 0;

	SickAlgorithm->m_pfEventInfoCallback(SickAlgorithm->EventInfo, SickAlgorithm->RetPar);
	SickAlgorithm->ReleaseCallMemory();
	
	Uncertain = false;
}

void CPerson::Add(CFrame* frame)
{
	CCrowdBlock::Add(frame);
	try
	{
			if (IsStand)
			{
				double num = (SickAlgorithm->Info->PersonIdentifyInfo.StandAlarmTime * 60.0) * 1000.0;
				if (!Uncertain && (((CAlgorithmTool::GetInstance()->GetFrameDiff(StandTelNum, LastTeleNum) * 1000.0) / SickAlgorithm->Info->Frequency) > num))
				{
					Uncertain = true;
				}
			}
			else if (Uncertain && (PersonProbability(this) > SickAlgorithm->Info->PersonIdentifyInfo.Probability))
			{
				Uncertain = false;
			}
			if (BeShield)
			{
				Uncertain = true;
				StandTelNum = LastTeleNum;
			}

			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if ((*iter)->LastTeleNum == LastTeleNum)
				{
					Direction = (*iter)->Direction;
					break;
				}
			}
	}
	catch (...)
	{
        LogError("Catch abnomal");
	}
	LogTrace("Add frame to person, name: %s, speed: %.2f, direction: %.2f, first teleNum: %d, last teleNum: %d",
						Name.c_str(), Speed, Direction, FirstTeleNum, LastTeleNum);
}

void CPerson::BeforeRemoved()
{

	SickAlgorithm->EventInfo = new S_EventInfo;
	S_PassProtectionAreaPar* passAreaPar = new S_PassProtectionAreaPar;
	SickAlgorithm->EventInfo->eventPar = passAreaPar;

	SickAlgorithm->EventInfo->blockNameID = PERSON;
	SickAlgorithm->EventInfo->eventTypeID = PASS_PROTECTION_AREA;

	passAreaPar->eventType = LEAVE_PROTECTION_AREA;
	passAreaPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	passAreaPar->fSpeed = Speed;
	passAreaPar->unionDirection.fDirection = Direction;
	passAreaPar->fAngle = CurrentPos.X;
	passAreaPar->fRadialLen = CurrentPos.Y;
	passAreaPar->fRemainTime = (float)((CAlgorithmTool::GetInstance()->GetFrameDiff(FirstTeleNum, LastTeleNum) * 1000) /
		SickAlgorithm->Info->Frequency);
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

	CCrowdBlock::BeforeRemoved();
}

bool CPerson::CanCombine(const CBlock* block)
{
	CPersonIdentifyInfo personIdentifyInfo = SickAlgorithm->Info->PersonIdentifyInfo;
	if (block->Length > personIdentifyInfo.MaxLength)
	{
		return false;
	}
	double distance = CAlgorithmTool::GetInstance()->GetDistance(CurrentPos, block->CurrentPos);

	if ((distance < 500.0))
	{
		int iCount = 0;
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			if ((*iter)->LastTeleNum == LastTeleNum)
			{
				iCount++;
			}
		}
		if (iCount < 2)
		{
			return true;
		}
	}
	if (distance > 1500.0)
	{
		return false;
	}
	return CCrowdBlock::CanCombine(block);
}

bool CPerson::CheckAlarm()
{
	try{
		    LogTrace("检测行人是否报警");
		    bool bAlarm = IsAlarm;
			if (IsStand)
			{
				double num = (SickAlgorithm->Info->PersonIdentifyInfo.StandAlarmTime * 60.0) * 1000.0;
				if(((CAlgorithmTool::GetInstance()->GetFrameDiff(StandTelNum, LastTeleNum) * 1000.0) / SickAlgorithm->Info->Frequency) > num)
				{
					bAlarm = true;
					LogTrace("检测到行人报警");
				}
				else
				{
					bAlarm = false;
					LogTrace("检测行人没有报警");
				}
			}
			else
			{
				bAlarm = false;
				LogTrace("检测到行人行走");
			}
			return bAlarm;
	}catch(...)
	{
		LogError("catch abnomal");
		return false;
	}

}

ushort g_lastTeleNum = 0; 

bool CPerson::CmpLastTeleNum(const CBlock* block1, const CBlock* block2)
{
	if (CAlgorithmTool::GetInstance()->GetFrameDiff(g_lastTeleNum, block1->LastTeleNum) < CAlgorithmTool::GetInstance()->GetFrameDiff(g_lastTeleNum, block2->LastTeleNum))
	{
		return true;
	}
	return false;
}

double CPerson::Membership(const CFrame* frame)
{
	try
	{
		if ((frame->Length < SickAlgorithm->Info->PersonIdentifyInfo.MinLength) || (frame->Length > 800.0))
		{
			return 0.0;
		}
		return CCrowdBlock::Membership(frame);
	}
	catch (...)
	{
		return 0.0;
	}
}

double CPerson::DoubleBlockJuge(double chance, CCrowdBlock* crowd)
{
	if (crowd->_blocks.size() < 2)
	{
		return chance;
	}
	std::vector<CBlock*> blockArray;
	for (std::list<CBlock*>::const_iterator iter = crowd->_blocks.begin(); iter != crowd->_blocks.end(); iter++)
	{
		blockArray.push_back(*iter);
	}
	g_lastTeleNum = crowd->LastTeleNum;
	std::sort(blockArray.begin(), blockArray.end(), CmpLastTeleNum);

	int num = (int) ((crowd->SickAlgorithm->Info->PersonIdentifyInfo.MinIdentifyFrames * crowd->SickAlgorithm->Info->Frequency) / 50.0);
	CPersonIdentifyInfo personIdentifyInfo = crowd->SickAlgorithm->Info->PersonIdentifyInfo;
	CBlock *block = blockArray.at(0);
	CBlock *block2 = blockArray.at(1);
	if ((block->Length >= personIdentifyInfo.MinLength) && (block2->Length >= personIdentifyInfo.MinLength))
	{
		if ((block->Length > personIdentifyInfo.MaxLength) || (block2->Length > personIdentifyInfo.MaxLength))
		{
			return chance;
		}
		if (CAlgorithmTool::GetInstance()->GetDistance(block->CurrentPos, block2->CurrentPos) > 1000.0)
		{
			return chance;
		}
		if (CAlgorithmTool::GetInstance()->GetFrameDiff(crowd->FirstTeleNum, crowd->LastTeleNum) < num)
		{
			return chance;
		}
		if ((block->Frames.size() < num) || (block2->Frames.size() < num))
		{
			return chance;
		}
		double distance = 0.0;
		double num3 = 0.0;
		CFrame* frame = block->Frames.at(block->Frames.size() - num);
		
		distance = CAlgorithmTool::GetInstance()->GetDistance(block->CurrentPos, frame->Middle);
	
		frame = block2->Frames[block2->Frames.size() - num];
		
		num3 = CAlgorithmTool::GetInstance()->GetDistance(block2->CurrentPos, frame->Middle);
		
		if ((distance >= 1000.0) || (num3 >= 1000.0))
		{
		   /* CDoubleLegAlgorithm* tag;
			if (crowd->Tag == NULL)
		   {
		      tag = new CDoubleLegAlgorithm(crowd);
		      crowd->Tag = tag;
		  }*/
		  CDoubleLegAlgorithm tag(crowd);
		  chance = tag.GetChance(chance);
		  // chance = (static_cast<CDoubleLegAlgorithm* >(crowd->Tag))->GetChance(chance);
		}
	}
	return chance;
}

double CPerson::SingleBlockJuge(CBlock* block, double chance)
{
	int num = (int) ((block->SickAlgorithm->Info->PersonIdentifyInfo.MinIdentifyFrames * block->SickAlgorithm->Info->Frequency) / 50.0);
	if (block->Frames.size() >= num)
	{
		if (block->Speed < 0.3)
		{
			return chance;
		}
		CFrame* frame = block->Frames.at(block->Frames.size() - num);
		if (CAlgorithmTool::GetInstance()->GetDistance(block->CurrentPos, frame->Middle) < (num * 15))
		{
			return chance;
		}
		if ((block->Length < 400.0) || (block->Length > 1500.0))
		{
			return chance;
		}
		int iSize = block->Frames.size()  - num;
		if (iSize < 1)
		{
			return chance;
		}
		double num2 = 0.0;
		//<计算平均速度
	    for (int i = iSize; i < block->Frames.size(); i++)
		{
			num2 += block->Frames.at(i)->Speed;
		}
		num2 = num2 / (block->Frames.size() - iSize);

		if (num2 > block->SickAlgorithm->Info->StandSpeed)
		{
			int num3 = 0;
			double num4 = 0.0;
			for (int i = iSize; i < block->Frames.size(); i++)
			{
				if (block->Frames.at(i)->Speed > 0)
				{
					double num5 = fabs((double) (block->Frames.at(i)->Speed - num2));
					if (num5 <= (num2 * 2.0))
					{
						num3++;
						num4 += num5;
					}
				}
			}
			if (num3 > 0)
			{
				double num6 = (1.0 - ((num4 / ((double) num3)) / num2)) * 0.6;
				if (num6 < 0.0)
				{
					num6 = 0.0;
				}
				chance = CAlgorithmTool::GetInstance()->MembershipAdd(chance, num6);
			}
		}
		CLegAlgorithm leg(block);
	
		chance = leg.GetChance(chance);
	}
	return chance;
}

double CPerson::PersonProbability(CBlock* block)
{
	if (block->Speed > 0)
	{
		double speed = block->Speed;
		double standSpeed = block->SickAlgorithm->Info->StandSpeed;
		if (!(speed < standSpeed))
		{
			double maxSpeed = block->SickAlgorithm->Info->PersonIdentifyInfo.MaxSpeed;
			if (!((block->Speed > maxSpeed)))
			{
				if (strcmp(block->GetObjectName(), "CBlock") != 0)
				{
					if ((block->Length < block->SickAlgorithm->Info->PersonIdentifyInfo.MinLength) || (block->Length > block->SickAlgorithm->Info->PersonIdentifyInfo.MaxLength))
					{
						return 0.0;
					}
				}
				else if ((block->Length < block->SickAlgorithm->Info->PersonIdentifyInfo.MinLength) || (block->Length > 800.0))
				{
					return 0.0;
				}
				double chance = 0.0;
				if (block->Speed > 1.0)
				{
					chance = 0.5 * (fabs((double) (min(block->Speed, block->SickAlgorithm->Info->PersonIdentifyInfo.MaxSpeed) - block->SickAlgorithm->Info->PersonIdentifyInfo.MaxSpeed)) / (block->SickAlgorithm->Info->PersonIdentifyInfo.MaxSpeed - 1.0));
				}
				else
				{
					chance = 0.5 * block->Speed;
				}
				if (chance < 0.0)
				{
					chance = 0.0;
				}
				CCrowdBlock* crowd = dynamic_cast<CCrowdBlock*>(block);

				if (strcmp(block->GetObjectName(), "CBlock") != 0 && crowd != NULL)
				{
					if (crowd->_blocks.size() == 0)
					{
						return 0.0;
					}
					if (crowd->_blocks.size() == 1)
					{
						return SingleBlockJuge(*(crowd->_blocks.begin()), chance);
					}
					std::vector<CBlock*> blockArray;
					for (std::list<CBlock*>::const_iterator iter = crowd->_blocks.begin(); iter != crowd->_blocks.end(); iter++)
					{
						blockArray.push_back(*iter);
					}
					g_lastTeleNum = crowd->LastTeleNum;
					std::sort(blockArray.begin(), blockArray.end(), CmpLastTeleNum);
					int num2 = (int) ((block->SickAlgorithm->Info->PersonIdentifyInfo.MinIdentifyFrames * block->SickAlgorithm->Info->Frequency) / 50.0);
					if (CAlgorithmTool::GetInstance()->GetFrameDiff(blockArray[1]->LastTeleNum, blockArray[0]->LastTeleNum) > num2)
					{
						chance = SingleBlockJuge(blockArray[0], chance);
					}
					else if (crowd->_blocks.size() < 4)
					{
						chance = DoubleBlockJuge(chance, crowd);
					}
					double num4 = CAlgorithmTool::GetInstance()->NormalIntersect(CAlgorithmTool::GetInstance()->IntersectAngle(blockArray[0]->Direction, 0.0/*block->SickAlgorithm->Track->Direction*/));
					return (chance * (1.1 - ((num4 * 2.5) / 300.0)));
				}
				chance = SingleBlockJuge(block, chance);
				if (block->Frames.size() < 2)
				{
					return 0.0;
				}
				CFrame* frame = block->Frames.at(0);
				CFrame* frame2 = block->Frames.at(block->Frames.size() - 1);
				
				double num6 = CAlgorithmTool::GetInstance()->IntersectAngle(CAlgorithmTool::GetInstance()->GetDirection(frame->Middle, frame2->Middle), 0.0/*block->SickAlgorithm->Track->Direction*/);
				num6 = CAlgorithmTool::GetInstance()->NormalIntersect(num6);
				chance *= 1.1 - ((num6 * 2.5) / 300.0);
				return chance;
			}
		}
	}
	return 0.0;
}

const char* CPerson::GetObjectName()
{
	return "CPerson";
}

bool CPerson::IsPerson(CBlock* b)
{
	return ((strcmp(b->GetObjectName(), "CPerson")) && !(dynamic_cast<CPerson*>(b))->Uncertain);
}

bool CPerson::Discard(std::list<CBlock*>& blockList)
{
	if ((_blocks.size() < 2) || (Length < 1500.0))
	{
		return false;
	}
	CBlock* leg = NULL;
	int num = 0;
	for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
	{
		CBlock* block = *iter;
		int frameDiff = CAlgorithmTool::GetInstance()->GetFrameDiff(block->FirstTeleNum, LastTeleNum);
		if (frameDiff > num)
		{
			leg = block;
			num = frameDiff;
		}
	}

	for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
	{
		if (CAlgorithmTool::GetInstance()->GetDistance(leg->CurrentPos, (*iter)->CurrentPos) <= 1500.0)
		{
			if ((*iter)->Length > SickAlgorithm->Info->PersonIdentifyInfo.MaxLength)
			{
				LogDebug("%s lengrh %d greater than the Person Identity Info max length %d",
						(*iter)->Name.c_str(), (int)(*iter)->Length, SickAlgorithm->Info->PersonIdentifyInfo.MaxLength);
				LogDebug("%s discard from person %s", (*iter)->Name.c_str(), Name.c_str());
				blockList.push_back(*iter);
			}
		}
		else
		{
			blockList.push_back(*iter);
			LogDebug("%s distance greater than 1500", (*iter)->Name.c_str());
			LogDebug("%s discard from person %s", (*iter)->Name.c_str(), Name.c_str());
		}
	}

	return true;
}
