
#include "Landslip.h"
#include "AlgorithmTool.h"
#include "CrowdBlock.h"


CLandslip::CLandslip(void)
{
}


CLandslip::~CLandslip(void)
{
}

CLandslip::CLandslip(CBlock* block)
{
	SickAlgorithm = block->SickAlgorithm;
	Name = SickAlgorithm->NameService->GetNewName("Landslip");
	CBlock* block2 = block;
	if (strcmp(block->GetObjectName(), "CCrowdBlock") == 0)
	{
		CCrowdBlock* block3 = dynamic_cast<CCrowdBlock*>(block);
		if (block3->_blocks.size() > 0)
		{
			double dbMaxLen = 0.0;
			//<���ҳ�������֡
			for (std::list<CBlock*>::iterator iter = block3->_blocks.begin(); iter != block3->_blocks.end(); iter++)
			{
				if ((*iter)->Length > dbMaxLen)
				{
					dbMaxLen = (*iter)->Length;
					block2 = *iter;
				}
			}
		}
	}
	Confirmed = true;
	for (uint i = 0; i < block2->Frames.size(); i++)
	{
		Frames.push_back(block2->Frames.at(i));
	}
	LastTeleNum = block2->LastTeleNum;
	FirstTeleNum = block2->FirstTeleNum;
	Length = block2->Length;
	CurrentPos = block2->CurrentPos;
	Speed = block2->Speed;
	Direction = block2->Direction;
	
	SickAlgorithm->EventInfo = new S_EventInfo;
	S_PassProtectionAreaPar* passAreaPar = new S_PassProtectionAreaPar;
	SickAlgorithm->EventInfo->eventPar = passAreaPar;

	SickAlgorithm->EventInfo->blockNameID = LANDSLIP;
	SickAlgorithm->EventInfo->eventTypeID = PASS_PROTECTION_AREA;

	passAreaPar->eventType = ENTER_PROTECTION_AREA;
	passAreaPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	passAreaPar->fSpeed = Speed;
	passAreaPar->unionDirection.fDirection = Direction;
	passAreaPar->fAngle = CurrentPos.X;
	passAreaPar->fRadialLen = CurrentPos.Y;
	passAreaPar->fRemainTime = 0.0;
	passAreaPar->enterFrameNO = LastTeleNum;
	passAreaPar->leaveFrameNO = 0;

	//SickAlgorithm->EventInfo = new S_EventInfo;

	//SickAlgorithm->EventInfo->blockType = LANDSLIP;
	//S_LandslipPar* landslipPar = new S_LandslipPar;

	//landslipPar->eventType = LANDSLIP_ENTER_AREA;
	//landslipPar->abnormalPoint = NORMAL_POINT;
	//landslipPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	//landslipPar->fSpeed = Speed;
	//landslipPar->fDirection = Direction;
	//landslipPar->fAngle = AlarmPos.X;
	//landslipPar->fRadialLen = AlarmPos.Y;
	//if (StandLength == 0)
	//{
	//	landslipPar->fBlockLen = SickAlgorithm->Info->MinDetectSize;
	//}
	//else
	//{
	//	landslipPar->fBlockLen = StandLength;
	//}
	//landslipPar->fRemainTime = 0.0;
	//landslipPar->frameNO = LastTeleNum;
	//SickAlgorithm->EventInfo->eventPar = landslipPar;

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

}

void CLandslip::BeforeRemoved()
{
	SickAlgorithm->EventInfo = new S_EventInfo;
	S_PassProtectionAreaPar* passAreaPar = new S_PassProtectionAreaPar;
	SickAlgorithm->EventInfo->eventPar = passAreaPar;

	SickAlgorithm->EventInfo->blockNameID = LANDSLIP;
	SickAlgorithm->EventInfo->eventTypeID = PASS_PROTECTION_AREA;

	passAreaPar->eventType = LEAVE_PROTECTION_AREA;
	passAreaPar->ID = atoi(Name.substr(1, Name.length() - 1).c_str());
	passAreaPar->fSpeed = Speed;
	passAreaPar->unionDirection.fDirection = Direction;
	passAreaPar->fAngle = CurrentPos.X;
	passAreaPar->fRadialLen = CurrentPos.Y;
	passAreaPar->fRemainTime = (CAlgorithmTool::GetInstance()->GetFrameDiff(FirstTeleNum, LastTeleNum) * 1000) / SickAlgorithm->Info->Frequency;;
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

	CBlock::BeforeRemoved();
}

bool CLandslip::CanCombine(const CBlock* block)
{
	return false;
}

bool CLandslip::CheckAlarm()
{
	return true;
}

double CLandslip::GetMaxSingleLength(CBlock* block)
{
	if (strcmp(block->GetObjectName(), "CCrowdBlock") == 0)
	{
		CCrowdBlock* block1 = dynamic_cast<CCrowdBlock*>(block);
		if (block1 != NULL)
		{
		double dbMaxLength = 0.0;
		for (std::list<CBlock*>::iterator iter = block1->_blocks.begin(); iter != block1->_blocks.end(); iter++)
		{
			double dbTmpLen = (*iter)->GetLastAdjustedLength();
			if (dbTmpLen > dbMaxLength)
			{
				dbMaxLength = dbTmpLen;
			}
		}
	    return dbMaxLength;
		}
		return 0.0;
	}
	CFrame* frame = block->Frames.at(block->Frames.size()-1);
	if (frame == NULL)
	{
		return 0.0;
	}
	return frame->Length;
}

double CLandslip::LandslipProbability(CBlock* block)
{
	CLandslipIdentifyInfo* landslipIdentifyInfo = &block->SickAlgorithm->Info->LandslipIdentifyInfo;
	CLargBlockInfo* largBlockInfo = NULL;
	if (GetMaxSingleLength(block) < landslipIdentifyInfo->MinLength)
	{
		if (block->LargBlockInfo != NULL)
		{
			delete block->LargBlockInfo;
			block->LargBlockInfo = NULL;
		}
		return 0.0;
	}
	if (block->LargBlockInfo == NULL)
	{
		block->LargBlockInfo = new CLargBlockInfo(block->CurrentPos, block->LastTeleNum);
		return 0.0;
	}
	largBlockInfo = block->LargBlockInfo;

	if (CAlgorithmTool::GetInstance()->GetFrameDiff(largBlockInfo->FirstTeleNum, block->LastTeleNum) < landslipIdentifyInfo->MinIdentifyFrames)
	{
		return 0.0;
	}
	if (strcmp(block->GetObjectName(), "CCrowdBlock") == 0)
	{
		CCrowdBlock* block2 = dynamic_cast<CCrowdBlock*>(block);
		if (block2->_blocks.size() > 0)
		{
			CBlock* maxLenBlock;
			double dbMaxLen = 0.0;
			//<���ҳ�������֡
			for (std::list<CBlock*>::iterator iter = block2->_blocks.begin(); iter != block2->_blocks.end(); iter++)
			{
				if ((*iter)->Length > dbMaxLen)
				{
					dbMaxLen = (*iter)->Length;
					maxLenBlock = *iter;
				}
			}
			if (CAlgorithmTool::GetInstance()->NormalIntersect(CAlgorithmTool::GetInstance()->IntersectAngle(maxLenBlock->Direction, block->SickAlgorithm->Track->Direction)) < 60.0)
			{
				return 0.0;
			}
		}
		
	}
	CalDistanceAndDirection(block, largBlockInfo);
	if ((largBlockInfo->Distance < landslipIdentifyInfo->MinDistance) || (largBlockInfo->ValidCount < 5))
	{
		return 0.0;
	}
	if (CAlgorithmTool::GetInstance()->NormalIntersect(CAlgorithmTool::GetInstance()->IntersectAngle(largBlockInfo->MoveDirection, block->SickAlgorithm->Track->Direction)) < 60.0)
	{
		return 0.0;
	}
	return 0.9;
}

void CLandslip::CalDistanceAndDirection(const CBlock* block, CLargBlockInfo* largBlockInfo)
{
	double speed = block->Speed;
	double standSpeed = block->SickAlgorithm->Info->StandSpeed;
	if (speed < standSpeed)
	{
		largBlockInfo->FirstPos = block->CurrentPos;
		largBlockInfo->FirstTeleNum = block->LastTeleNum;
		largBlockInfo->ValidCount = 0;
	}
	//<垂直于轨道方向的移动距离
	double distance = CAlgorithmTool::GetInstance()->GetDistance(block->CurrentPos, largBlockInfo->FirstPos);
	largBlockInfo->BlockDirection = CAlgorithmTool::GetInstance()->GetDirection(block->RightPos, block->LeftPos);
	largBlockInfo->MoveDirection = CAlgorithmTool::GetInstance()->GetDirection(largBlockInfo->FirstPos, block->CurrentPos);
	double num2 = CAlgorithmTool::GetInstance()->IntersectAngle(largBlockInfo->BlockDirection, largBlockInfo->MoveDirection);
	largBlockInfo->Distance = fabs((double)(distance * sin(CAlgorithmTool::GetInstance()->ToArc(num2))));
	if (largBlockInfo->Distance > block->SickAlgorithm->Info->LandslipIdentifyInfo.MinDistance)
	{
		largBlockInfo->ValidCount++;
	}
	else
	{
		largBlockInfo->ValidCount = 0;
	}
}

const char* CLandslip::GetObjectName()
{
	return "CLandslip";
}

