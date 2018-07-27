
#include "CrowdBlock.h"
#include "AlgorithmTool.h"
#include <liblogger/liblogger.h>

CCrowdBlock::CCrowdBlock(void)
{
	Tag = NULL;
}


CCrowdBlock::~CCrowdBlock(void)
{
	for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end();)
	{
		std::list<CBlock*>::iterator iter_e = iter++;
		CBlock* block = *iter_e;
		if (block != NULL)
		{
			LogDebug("Name %s is deleted", block->Name.c_str());
			delete block;
			block = NULL;
		}
		_blocks.erase(iter_e);
	}
	_blocks.clear();
	
}

CCrowdBlock::CCrowdBlock(CBlock* b1, CBlock* b2)
{
	Tag = NULL;
	SickAlgorithm = b1->SickAlgorithm;
	Name = SickAlgorithm->NameService->GetNewName("CrowdBlock");
	_blocks.push_back(b1);
	_blocks.push_back(b2);
	FirstTeleNum = LastTeleNum = b1->LastTeleNum;
	Length = b1->Length + b2->Length;
	CalCurrentPos();
	_firstPos = _lastPos = CurrentPos;
	CalSpeed();
	LogDebug("Find crowdblock, name: %s, length: %.2f, teleNum: %d, leftPos: x->%.2f y->%.2f, rigthPos: x->%.2f y->%.2f",
					Name.c_str(), Length, FirstTeleNum, LeftPos.X, LeftPos.Y, RightPos.X, RightPos.Y);
}

void CCrowdBlock::CalCurrentPos()
{
	try
	{
		if (_blocks.size() != 0)
		{
			list<CBlock*>::iterator iter = _blocks.begin();
			CPOINT currentPos = (*iter)->CurrentPos;
			iter++;
			for (; iter != _blocks.end(); iter++)
			{
				if ((*iter)->LastTeleNum == LastTeleNum)
				{
					currentPos = CAlgorithmTool::GetInstance()->GetMiddlePoint(currentPos, (*iter)->CurrentPos);
				}
			}
			CurrentPos = currentPos;
		}
	}
	catch (...)
	{
		LogFatal("Find fatal");
	}
}

void CCrowdBlock::CalSpeed()
{
	try
	{
		if (_blocks.size() != 0)
		{
			//<桢号为最后一桢的Block计算平均速度
			std::list<CBlock*> tmpList;
			tmpList.clear();
			double dbAvgSpeed = 0.0;
			for (list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if ((*iter)->LastTeleNum == LastTeleNum && (*iter)->Speed > 0)
				{
					tmpList.push_back(*iter);
					dbAvgSpeed += (*iter)->Speed;
				}
			}

			if (tmpList.size() != 0)
			{
				//<判断是否不动，不动则报警
				Speed = dbAvgSpeed / tmpList.size();
				//JudgeStand(Speed);
				JudgeValidFrameNum();
				//<计算方向
				Direction = CAlgorithmTool::GetInstance()->GetDirection(_lastPos, CurrentPos);
			}
		}
	}
	catch (...)
	{
		LogFatal("Find fatal, exit(0)");
		exit(0);
	}
}

void CCrowdBlock::Add(CFrame* frame)
{
	try
	{
		if (_blocks.size() != 0)
		{
			double dbMaxMembership = 0.0;
			CBlock* tmpMaxBlock;
			for (list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				double dbTemp = (*iter)->Membership(frame);
				if (dbTemp > dbMaxMembership)
				{
					dbMaxMembership = dbTemp;
					tmpMaxBlock = *iter;
				}
			}	
			//���뵽������ϵ�����Ǹ�֡����
			if (tmpMaxBlock != NULL)
			{
				tmpMaxBlock->Add(frame);
				LastTeleNum = frame->TeleNum;
			}
		}
	}
	catch (...)
	{
		LogFatal("catch abnormal, exit(0)");
		exit(0);
	}
}

/*
 *  增加桢，重新计算参数
*/
void CCrowdBlock::AfterAddFrame()
{
	//
	list<CBlock*> tmpList;
	tmpList.clear();
	for (list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
	{
		if ((*iter)->LastTeleNum == LastTeleNum)
		{
			tmpList.push_back(*iter);
		}
	}

	if (tmpList.size() > 0)
	{
		CPOINT first;
		first.X = 10000000;
		CPOINT last;
		last.X = 0;
		bool bFirstFlag = false;
		bool bLastFlag = true;
		for (list<CBlock*>::iterator iter = tmpList.begin(); iter != tmpList.end(); iter++)
		{
			int iSize = (*iter)->Frames.size();
			if (iSize != 0)
			{
				CFrame* frame = (*iter)->Frames.at(iSize - 1);
				
				if ((frame->First.X < first.X))
				{//
					first = frame->First;
					if (!bFirstFlag)
					   bFirstFlag = true;
				}
				
				if ( (frame->Last.X > last.X))
				{
					last = frame->Last;
					if (!bLastFlag)
					{
						bLastFlag = true;
					}
				}
			}
		}
		//
		if (bFirstFlag && bLastFlag)
		{
			Length = CAlgorithmTool::GetInstance()->GetDistance(first, last);
			LeftPos = first;
			RightPos = last;
		}

		RemoveExpiredBlocks(LastTeleNum);
		CalCurrentPos();
		CalSpeed();
		LogDebug("Add frame to crowdblock, blockname: %s, first tele num: %d, last tele num: %d, speed: %.2f,leftPos: x->%.2f y->%.2f, rigthPos: x->%.2f y->%.2f",
									Name.c_str(), FirstTeleNum, LastTeleNum, Speed, LeftPos.X, LeftPos.Y, RightPos.X, RightPos.Y);
	}
}

void CCrowdBlock::RemoveExpiredBlocks(ushort teleNum)
{
	try
	{
		if (_blocks.size() > 0)
		{
			double num = (SickAlgorithm->Info->Frequency * SickAlgorithm->Info->ExpiredTime) / 1000.0;
			for (list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if (CAlgorithmTool::GetInstance()->GetFrameDiff((*iter)->LastTeleNum, teleNum) > num)
				{
					(*iter)->SetRemoved(true);
					LogDebug("Removed and deleted block, name: %s, last tele num: %d, judge tele num: %d, judge val: %d",
							(*iter)->Name.c_str(), (*iter)->LastTeleNum, teleNum, (int)num);
				}
				else
				{
					(*iter)->SetRemoved(false);
				}
			}
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); )
			{
				std::list<CBlock*>::iterator iter_e = iter++;
				CBlock* block = *iter_e;
				if (block->Removed)
				{
					_blocks.erase(iter_e);
					if (block != NULL)
					{
						delete block;
						block = NULL;
					}
				}
			}
		}
	}
	catch (...)
	{
	    LogError("catch abnomal");
	}
}

double CCrowdBlock::Membership(const CFrame* frame)
{
	try
	{
		if (_blocks.size() == 0)
		{
			return 0.0;
		}
		double num = 0.0;
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			CBlock* tmpBlock = *iter;
			double num2 = tmpBlock->Membership(frame);
			if (num2 > num)
			{
				num = num2;
			}
		}
		return num;
	}
	catch (...)
	{
		LogError("Catch abnomal, retturn 0.0");
		return 0.0;
	}
}

bool CCrowdBlock::IsCover(const CPOINT* p)
{
	for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
	{
		if ((*iter)->IsCover(p))
		{
			return true;
		}
	}
	return false;
}

 bool CCrowdBlock::Discard(std::list<CBlock*>& blockList)
{
	blockList.clear();
	if (_blocks.size() == 1)
	{
		//CBlock* tmpBlock = new CBlock();
		//*tmpBlock = *(*(_blocks.begin()));
		blockList.push_back(*(_blocks.begin()));

		return true;
	}
	return false;
}

void CCrowdBlock::Combine(CBlock* block)
{
	try
	{
		if (strcmp(block->GetObjectName(), "CBlock") != 0)
		{
			CCrowdBlock* tmpBlock = dynamic_cast<CCrowdBlock*>(block);
			if (tmpBlock != NULL)
			{
			  for (std::list<CBlock*>::iterator iter = tmpBlock->_blocks.begin(); iter != tmpBlock->_blocks.end(); iter++)
			  {
				_blocks.push_back(*iter);
			  }
			}
		}
		else
		{
			_blocks.push_back(block);
		}
		CalCurrentPos();
		CalSpeed();
		LogDebug("Crowdblock %s combine block %s, after add, speed: %.2f, current pos: x->%.2f, y->%.2f",
				Name.c_str(), block->Name.c_str(), Speed, CurrentPos.X, CurrentPos.Y);
	}
	catch (...)
	{
       LogError("catch abnomal");
	}
}

bool CCrowdBlock::GetCurArea(CPolarArea& area)
{
	std::list<CPolarArea> tmpList;
	for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
	{
		CPolarArea curArea;
		bool bRet = (*iter)->GetCurArea(curArea);
		if (bRet)
		{
		   tmpList.push_back(curArea);
		}
		
	}
	if (tmpList.size() == 0)
	{
		return false;
	}

	area.A1 = 10000000.0;
	area.A2 = 0.0;
	area.R1 = 10000000.0;
	area.R2 = 0.0;
	for (std::list<CPolarArea>::iterator iter = tmpList.begin(); iter != tmpList.end(); iter++)
	{
		if (area.A1 > (*iter).A1)
		{
			area.A1 = (*iter).A1;
		}

		if (area.A2 < (*iter).A2)
		{
			area.A2 = (*iter).A2;
		}

		if (area.R1 > (*iter).R1)
		{
			area.R1 = (*iter).R1;
		}

		if (area.R2 < (*iter).R2)
		{
			area.R2 = (*iter).R2;
		}
	}

	return true;
}

int CCrowdBlock::GetBlocksCounts()
{
	return _blocks.size();
}

const char* CCrowdBlock::GetObjectName()
{
	return "CCrowdBlock";
}

double CCrowdBlock::GetLastAdjustedLength()
{
	return Length;
}
