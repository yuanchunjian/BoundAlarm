
#include "Leg.h"
#include "AlgorithmTool.h"
#include <algorithm>


CLeg::CLeg(void)
{
	_standPosInitFlag = false;
	LastTelNum = 0;
	StandTelNum = 0;
}

CLeg::CLeg(int legType)
{
	_legType = legType;
	_standPosInitFlag = false;
	LastTelNum = 0;
	StandTelNum = 0;
}

CLeg::~CLeg(void)
{

}

void CLeg::Add(CFrame* frame)
{
	LastTelNum = frame->TeleNum;
	switch (_legType)
	{
	case 0:
		{
			if (!_standPosInitFlag)
			{
				if (frame->FirstDistance <= 20.0)
				{
					_standPos = frame->First;
					_firstPos = _standPos;
					StandTelNum = frame->TeleNum;
					_standPosInitFlag = true;
				}
				return;
			}
			double distance = CAlgorithmTool::GetInstance()->GetDistance(_standPos, frame->First);
			if ((distance <= 300.0) || (frame->FirstDistance >= 20.0))
			{
				break;
			}
			_steps.push_back(distance);
			_standPos = frame->First;
			StandTelNum = frame->TeleNum;
			TotalDistance = CAlgorithmTool::GetInstance()->GetDistance(_firstPos, _standPos);
			return;
		}
	case 1:
		{
			if (!_standPosInitFlag)
			{
				if (frame->LastDistance <= 20.0)
				{
					_standPos = frame->Last;
					_firstPos = _standPos;
					StandTelNum = frame->TeleNum;
					_standPosInitFlag = true;
				}
				return;
			}
			double item = CAlgorithmTool::GetInstance()->GetDistance(_standPos, frame->Last);
			if ((item <= 300.0) || (frame->LastDistance >= 20.0))
			{
				break;
			}
			_steps.push_back(item);
			_standPos = frame->Last;
			TotalDistance = CAlgorithmTool::GetInstance()->GetDistance(_firstPos, _standPos);
			StandTelNum = frame->TeleNum;
			return;
		}
	case 2:
		{
			if (!_standPosInitFlag)
			{
				if (frame->Speed <= 1.0)
				{
					_standPos = frame->Middle;
					_firstPos = _standPos;
					StandTelNum = frame->TeleNum;
					_standPosInitFlag = true;
				}
				return;
			}
			double num3 = CAlgorithmTool::GetInstance()->GetDistance(_standPos, frame->Middle);
			if ((num3 > 300.0) && (frame->Speed < 1.0))
			{
				_steps.push_back(num3);
				_standPos = frame->Middle;
				TotalDistance = CAlgorithmTool::GetInstance()->GetDistance(_firstPos, _standPos);
				StandTelNum = frame->TeleNum;
			}
		}
		break;
	default:
		return;
	}
}

double CLeg::GetStepLength()
{
	if (_steps.size() == 0)
	{
		return 0.0;
	}
	double dbAvg = 0.0;
	for (std::list<double>::iterator iter = _steps.begin(); iter != _steps.end(); iter++)
	{
		dbAvg += *iter;
	}
	return dbAvg / _steps.size();
}

int CLeg::GetSteps()
{
	return _steps.size();
}

//-------------------------------------------CLegAlgorithm------------------------------------------

CLegAlgorithm::CLegAlgorithm()
{

}

CLegAlgorithm::CLegAlgorithm(CBlock* block)
{
	_block = block;
	_leftLeg = CLeg(0);
	_rightLeg = CLeg(1);
	_lastTelNum = 0;
}

CLegAlgorithm::~CLegAlgorithm()
{

}

double CLegAlgorithm::GetChance(double chance)
{
	if (!_block->IsAlarm)
	{
		int num = -1;
		if (_lastTelNum == 0)
		{
			for (uint i = 0; i < _block->Frames.size(); i++)
			{
				if (_block->Frames.at(i)->Speed > _block->SickAlgorithm->Info->StandSpeed)
				{
					num = i;
					break;
				}
			}
		}
		else
		{
			for (uint i = 0; i < _block->Frames.size(); i++)
			{
				if (_block->Frames.at(i)->TeleNum == _lastTelNum)
				{
					num = i;
					break;
				}
			}
		}
		if (num != -1)
		{
			for (int i = num + 1; i < (_block->Frames.size() - 1); i++)
			{
				CFrame* frame = _block->Frames[i];
				_leftLeg.Add(frame);
				_rightLeg.Add(frame);
			}
			_lastTelNum = _block->Frames[_block->Frames.size() - 2]->TeleNum;
			if ((((_leftLeg.GetSteps() > 2) && (_rightLeg.GetSteps() > 2)) && ((_rightLeg.GetStepLength() > 300.0) && (_leftLeg.GetStepLength() > 300.0))) && (((_leftLeg.TotalDistance > (_leftLeg.GetSteps() * 300)) && (_rightLeg.TotalDistance > (_rightLeg.GetSteps() * 300))) && (abs((int)(_rightLeg.GetSteps() - _leftLeg.GetSteps())) < 5)))
			{
				chance = CAlgorithmTool::GetInstance()->MembershipAdd(chance, 0.7);
			}
		}
	}
	return chance;
}
//-------------------------------------------CDoubleLegAlgorithm------------------------------------------

ushort CDoubleLegAlgorithm::_TeleNum = 0;

CDoubleLegAlgorithm::CDoubleLegAlgorithm(CCrowdBlock* block)
{
	_isAlarm = block->IsAlarm;
	CDoubleLegAlgorithm::_TeleNum = block->LastTeleNum;
	for (std::list<CBlock*>::iterator iter = block->_blocks.begin(); iter != block->_blocks.end(); iter++)
	{
		LEG_BLOCK* tmpLegBlock = new LEG_BLOCK;
		tmpLegBlock->block = *iter;
		tmpLegBlock->leg = NULL;
		_vecLegBlock.push_back(tmpLegBlock);
	}
}

CDoubleLegAlgorithm::~CDoubleLegAlgorithm()
{
	for (uint i = 0; i < _vecLegBlock.size(); i++)
	{
		if (_vecLegBlock.at(i)->leg != NULL)
		{
			delete _vecLegBlock.at(i)->leg;
			_vecLegBlock.at(i)->leg = NULL;
		}
		delete _vecLegBlock.at(i);
		_vecLegBlock.at(i) = NULL;
	}
	_vecLegBlock.clear();
	std::vector<LEG_BLOCK*>().swap(_vecLegBlock);
}

bool CmpLastTeleNum(LEG_BLOCK* legBlock1, LEG_BLOCK* legBlock2)
{
	if (CAlgorithmTool::GetInstance()->GetFrameDiff(CDoubleLegAlgorithm::_TeleNum, legBlock1->block->LastTeleNum) < CAlgorithmTool::GetInstance()->GetFrameDiff(CDoubleLegAlgorithm::_TeleNum, legBlock2->block->LastTeleNum))
	{
		return true;
	}
	return false;
}

double CDoubleLegAlgorithm::GetChance(double chance)
{
	std::list<CLeg*> list;
	if (!_isAlarm)
	{
	     for (uint i = 0; i < _vecLegBlock.size(); i++)
	     {
			 CBlock* tmpBlock = _vecLegBlock.at(i)->block;
			 CLeg* leg = _vecLegBlock.at(i)->leg;
			 if (leg == NULL)
			 {
				 leg = new CLeg(2);
				 _vecLegBlock.at(i)->leg = leg;
			 }
			 int num = -1;
			 if (leg->LastTelNum == 0)
			 {
				 for (uint j = 0; j < tmpBlock->Frames.size(); j++)
				 {
					 if (tmpBlock->Frames.at(j)->Speed > tmpBlock->SickAlgorithm->Info->StandSpeed)
					 {
						 num = j;
						 break;
					 }
				 }
			 }
			 else
			 {
				 for (uint j = 0; j < tmpBlock->Frames.size(); j++)
				 {
					 if (tmpBlock->Frames.at(j)->TeleNum == leg->LastTelNum)
					 {
						 num = j;
						 break;
					 }
				 }
			 }
			 if (num != -1)
			 {
				 for (int i = num + 1; i < (tmpBlock->Frames.size() - 1); i++)
				 {
					 CFrame* frame = tmpBlock->Frames[i];
					 leg->Add(frame);
				 }
				 list.push_back(leg);
			 }
	     }
		//-------------------------------------------------
		if (list.size() < 2)
		{
			return chance;
		}

		std::sort(_vecLegBlock.begin(), _vecLegBlock.end(), CmpLastTeleNum);

		CLeg* tag = _vecLegBlock[0]->leg;
		CLeg* leg2 = _vecLegBlock[1]->leg;
		if ((tag == NULL) || (leg2 == NULL))
		{
			return chance;
		}
		if ((((tag->GetSteps() > 2) && (leg2->GetSteps() > 2)) && ((leg2->GetStepLength() > 300.0) && (tag->GetStepLength() > 300.0))) && (((leg2->TotalDistance > (leg2->GetSteps() * 300)) && (tag->TotalDistance > (tag->GetSteps() * 300))) && (abs((int) (leg2->GetSteps() - tag->GetSteps())) < 5)))
		{
			chance = CAlgorithmTool::GetInstance()->MembershipAdd(chance, 0.7);
		}
	}
	return chance;
}
