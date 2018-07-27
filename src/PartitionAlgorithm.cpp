
#include "PartitionAlgorithm.h"
#include "AlgorithmTool.h"
#include "Train.h"
#include "Landslip.h"
#include <iostream>

#include <algorithm>
#include <liblogger/liblogger.h>
#include "../main/CThreadLock.h"

CThreadLock g_clearFlagLock;

CPartitionAlgorithm::CPartitionAlgorithm(void)
{
	_clearFlag = false;
}

CPartitionAlgorithm::CPartitionAlgorithm(CPartitionInfo info)
{
	_info = info;
	_clearFlag = false;
}

CPartitionAlgorithm::~CPartitionAlgorithm(void)
{
	for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); )
	{
		std::list<CBlock*>::iterator iter_e = iter++;
		CBlock* block = *iter_e;
		if (block != NULL)
		{
			LogDebug("%s is deleted", block->Name.c_str());
			delete block;
			block = NULL;
		}
		_blocks.erase(iter_e);
	}
	//_blocks.clear();
}

void CPartitionAlgorithm::CheckAlarm()
{
	try
	{
		if (_blocks.size() != 0)
		{
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if (*iter != NULL)
				{
				   CBlock* block = *iter;
				   if (block != NULL&&strcmp((*iter)->GetObjectName(),"CBlock")!=0)
				   {
				      bool bAlarm = block->CheckAlarm();
				      try {
				      block->SetIsAlarm(bAlarm);
				      }catch(...)
				      {
				    	  LogError("catch SetIsAlarm abnomal");
				    	  std::cout << "catch SetIsAlarm abnomal" << endl;
				    	  Reset();
				    	  return;
				      }
				   }
				   else
				   {
					   LogError("block is NULL");
				   }
				}
	           else
	           {
			      LogError("iter is NULL");
	           }
			}
		}
	}
	catch (...)
	{
		LogError("catch abnomal,reset");
		Reset();
	}
}

void CPartitionAlgorithm::CombineBlocks()
{
	LogFuncEntry();
	try
	{
		std::list<CBlock*> collection;
	    
		std::list<CBlock*> crowdList;
		std::list<CBlock*> sourceList;
		
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			CCrowdBlock* tmpCrowd = dynamic_cast<CCrowdBlock*>(*iter);
			if (tmpCrowd != NULL || strcmp((*iter)->GetObjectName(), "CBlock") != 0 )
			{
				crowdList.push_back(*iter);
			}
			else
			{
				sourceList.push_back(*iter);
			}
		}
		
		if (sourceList.size() > 0)
		{
			for (std::list<CBlock*>::iterator iter = sourceList.begin(); iter != sourceList.end(); iter++)
			{
				CBlock* block = *iter;
				if (!block->Removed)
				{
					//
					if (crowdList.size() > 0)
					{
						for (std::list<CBlock*>::iterator iter1 = crowdList.begin(); iter1 != crowdList.end(); iter1++)
						{
							CCrowdBlock* tmpCrowdBlock = dynamic_cast<CCrowdBlock*>(*iter1);
							if (tmpCrowdBlock != NULL && tmpCrowdBlock->CanCombine(block))
							{
								LogDebug("%s is combined into %s ", block->Name.c_str(), tmpCrowdBlock->Name.c_str());
								/*if (block->AlarmLevel == ONE_LEVEL_ALARM)
								{
									block->AlarmLevel = TWO_LEVEL_ALARM;
								}*/
								tmpCrowdBlock->Combine(block);
								block->SetRemoved(true);
								break;
							}
						}
					}
				}

				if (!block->Removed)
				{
					std::list<CBlock*>::iterator iter2 = iter;
					iter2++;
					for (; iter2 != sourceList.end(); iter2++)
					{
						CBlock* block1 = *iter2;
						if (!block1->Removed && block->CanCombine(block1))
						{
							//
							CCrowdBlock* item = new CCrowdBlock(block, block1);
                            if (item == NULL)
                            {
                            	LogFatal("new crowdBlock abnomal, exit(0)");
                            	exit(0);
                            	return;
                            }
							LogDebug("%s and %s combine for %s ", block->Name.c_str(), block1->Name.c_str(), item->Name.c_str());
							block->SetRemoved(true);
							block1->SetRemoved(true);
							crowdList.push_back(item);
							collection.push_back(item);
							break;
						}
					}
				}
			}
		}
		
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); )
		{
			std::list<CBlock*>::iterator iter_e = iter++;
			CBlock* block = *iter_e;
			if (block->Removed)
			{
				LogDebug("%s is removed form _blocks of partition but not delete", block->Name.c_str());
				_blocks.erase(iter_e);
			}
		}
		 if (collection.size() > 0)
		{
		    _blocks.insert(_blocks.end(), collection.begin(), collection.end());
		}
	}
	catch (...)
	{
		LogError("catch abnomal,reset");
		Reset();
	}
	LogFuncExit();
}
//<点的聚类
int CPartitionAlgorithm::ToFrames(const std::vector<ushort>& matter, double startAngle, ushort teleNum, std::vector<CFrame*>& result)
{
	LogFuncEntry();
	try
	{
		double num = -1.0;
		bool bInitFlag = false;
		
		double dbResolution = SickAlgorithm->Info->Resolution;
		ushort maxVal = SickAlgorithm->Info->MaxValue;
		ushort minVal = SickAlgorithm->Info->MinValue;
		CFrame* frame = NULL;
		for (uint i = 0; i < matter.size(); i++)
		{
			ushort num2 = matter[i];
			if (num2 >= maxVal || num2 <= minVal)
			{
				if (bInitFlag && frame->Data.size() > 1)
				{
					frame->SetParameter();
					if(frame->Length>=50) result.push_back(frame);
					bInitFlag = false;
				}
				else if(bInitFlag)
				{
					if (frame != NULL)
					{
					   delete frame;
					   frame = NULL;
					}
					bInitFlag = false;
				}
				num = -1;
			}
			else
			{
				if (num > 0)
				{
					//<高度差大于500的归为一类
					if (fabs((double)num2 - num) > 500.0)
					{
						if (frame->Data.size() > 1)
						{
							frame->SetParameter();
							result.push_back(frame);
							bInitFlag = false;
						}
						else if(bInitFlag)
						{
							delete frame;
							frame = NULL;
							bInitFlag = false;
						}
					}
				}
				if (!bInitFlag)
				{	
					frame = new CFrame;
					if (frame == NULL)
					{
					    LogFatal("new frame abnomal, exit(0)");
						exit(0);
						return 0;
					}
					frame->Data.clear();
					frame->SegInfo->Resolution = dbResolution;
					frame->TeleNum = teleNum;
					frame->SegInfo->StartAngle = startAngle + i * dbResolution;
					bInitFlag = true;
				}
		
				frame->Data.push_back(num2);
				num = (double)num2;
			}
		}
	}
	catch (...)
	{
		LogError("Find frames catch abnormal");
		Reset();
		for (uint i = 0; i < result.size(); i++)
		{
			delete result.at(i);
			result.at(i) = NULL;
		}
		result.clear();
		return 0;
	}
	LogFuncExit();
	return result.size();
}

void CPartitionAlgorithm::Reset(bool bResume)
{
	if (bResume)
	{
	   SickAlgorithm->AlarmInfoProcess.ResumeAllAlarm();
	}

	g_clearFlagLock.Lock();
	_clearFlag = true;
	g_clearFlagLock.UnLock();
}

void CPartitionAlgorithm::RemoveExpiredBlocks(ushort teleNum)
{
	LogFuncEntry();
	try
	{
		std::vector<CBlock*> vecFilter_1;
		std::vector<CBlock*> vecFilter_2;
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			CBlock *block = *iter;
			if (block->LastTeleNum != teleNum)
			{
				vecFilter_1.push_back(block);
			}
			else
			{
				vecFilter_2.push_back(block);
			}
		}

		/*for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			CBlock *block = *iter;
			if (block->LastTeleNum == teleNum)
			{
				vecFilter_2.push_back(block);
			}
		}*/

		for (uint i = 0; i < vecFilter_1.size(); i++)
		{
			CBlock *block = vecFilter_1.at(i);
			block->BeShield = false;
			for (uint j = 0; j < vecFilter_2.size(); j++)
			{
				CBlock* block2 = vecFilter_2.at(j);
				block2->BeShield = false;
				if (block2->IsCover(&block->CurrentPos))
				{
					LogDebug("Check %s is shielded %s", block->Name.c_str(), block2->Name.c_str());
					block->BeShield = true;
					break;
				}
			}
		}
		double num = (SickAlgorithm->Info->Frequency * SickAlgorithm->Info->ExpiredTime) / 1000.0;
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			CBlock* block3 = *iter;
			if (!block3->BeShield)
			{
				if (CAlgorithmTool::GetInstance()->GetFrameDiff(block3->LastTeleNum, teleNum) > num)
				{
                    LogDebug("Check %s has %d frames is not added, to be removed and deleted", block3->Name.c_str(), (int)num);
					block3->SetRemoved(true);
				}
				if (strcmp(block3->GetObjectName(), "CBlock") != 0)
				{
					CCrowdBlock* block4 = dynamic_cast<CCrowdBlock*>(block3);
					if (block4 != NULL && block4->_blocks.size() == 0)
					{
						LogDebug("Check %s is empty, to be removed and deleted",  block4->Name.c_str());
						block3->SetRemoved(true);
					}
				}
			}
		}
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); )
		{
			std::list<CBlock*>::iterator iter_e = iter++;
			CBlock* block = *iter_e;
			if (block->Removed)
			{
				_blocks.erase(iter_e); //
				if (block != NULL)
				{
					delete block;
					block = NULL;
				}
			}
		}
	}
	catch (...)
	{
		LogError("catch abnomal,reset");
		Reset();
	}
	LogFuncExit();
}

void CPartitionAlgorithm::Partition(const std::vector<ushort>& matter, double startAngle, ushort teleNum)
{
	LogFuncEntry();
	try
	{
		g_clearFlagLock.Lock();
		bool bFlag = _clearFlag;
		_clearFlag = false;
		g_clearFlagLock.UnLock();
		if (bFlag)
		{
			LogInfo("Check to clear flag, deleted all block");
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end();)
			{
				std::list<CBlock*>::iterator iter_e = iter++;
				CBlock* block = *iter_e;
				_blocks.erase(iter_e);
				if (block != NULL)
				{
					LogDebug("Name %s is deleted", block->Name.c_str());
					delete block;
					block = NULL;
				}
			}
			_blocks.clear();
		}
		/*if (teleNum == 60525)//39230
		{
			ushort a1 = matter[0];
		}*/
		std::vector<CFrame*> frames;
		int iRet = ToFrames(matter, startAngle, teleNum, frames);
		if (iRet > 0)
		{
			DistributeFrames(frames);
			if (_blocks.size() > 1)
			{
				CombineBlocks();
			}
			//Distinguish(frames, teleNum); //<只检测报警，不检测其他信息
		}
		RemoveExpiredBlocks(teleNum);
		CheckAlarm();
		Decompose();
	}
	catch (...)
	{
		LogError("catch abnomal,reset");
		Reset();
	}
	LogFuncExit();
}

void CPartitionAlgorithm::Decompose()
{
	LogFuncEntry();
	try
	{
		std::list<CBlock*> collection ;
		for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
		{
			if (strcmp((*iter)->GetObjectName(), "CBlock") != 0)
			{
				CCrowdBlock* block = dynamic_cast<CCrowdBlock*>(*iter);
				if (block != NULL) {
					std::list<CBlock*> list2;
					bool bRet = block->Discard(list2);
					if (bRet && (list2.size() != 0)) {
						for (std::list<CBlock*>::iterator iter = list2.begin(); iter != list2.end(); iter++) {
							(*iter)->Confirmed = false;
							(*iter)->Removed = false;
							collection.push_back(*iter);
							LogDebug("CrowBlock %s move %s to blocks of partition", block->Name.c_str(), (*iter)->Name.c_str());
							//<
							for (std::list<CBlock*>::iterator iter1 = block->_blocks.begin(); iter1 != block->_blocks.end(); /*iter1++*/) {
								std::list<CBlock*>::iterator iter_e = iter1++;
								if ((*iter_e)->Name == (*iter)->Name) {
									LogDebug("CrowBlock %s erase %s", block->Name.c_str(), (*iter_e)->Name.c_str());
									block->_blocks.erase(iter_e);
								}
							}
						}
					}
				}
			}
		}
		_blocks.insert(_blocks.end(), collection.begin(), collection.end());
	}
	catch (...)
	{
		LogError("catch abnomal,reset");
		Reset();
	}
	LogFuncExit();
}

bool CmpMiddle(const CFrame* a, const CFrame* b)
{
	if (fabs(a->Middle.X - 90) < fabs(b->Middle.X - 90))
	{
		return true;
	}
	return false;
}

void CPartitionAlgorithm::DistributeFrames(std::vector<CFrame*>& frames)
{
	LogFuncEntry();
	if (frames.size() != 0)
	{
		try
		{
			double newBlockGrade = _info.NewBlockGrade;
			if (frames.size() <= _blocks.size())
			{
				newBlockGrade *= 0.7;
			}
			
			std::sort(frames.begin(), frames.end(), CmpMiddle);
			for (uint i = 0; i < frames.size(); i++)
			{
				CFrame* frame = frames.at(i);
				double num2 = 0.0;
				CBlock* block = NULL;
				for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
				{
					CBlock *block2 = *iter;
					double num3 = block2->Membership(frame);
					if (num3 > num2)
					{
						num2 = num3;
						block = block2;
					}
				}
				if ((num2 > newBlockGrade) && (block != NULL))
				{
					block->Add(frame);
				}
				else
				{
					CBlock* pBlock = new CBlock(frame, SickAlgorithm);
					if (pBlock == NULL)
					{
						LogFatal("new block abnomal, exit(0)");
						exit(0);
						return;
					}
					_blocks.push_back(pBlock);
				}
			}
			
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if (strcmp((*iter)->GetObjectName(), "CBlock") != 0)
				{
					CCrowdBlock* tmpBlock = dynamic_cast<CCrowdBlock*>(*iter);
					if (tmpBlock != NULL)
					{
						tmpBlock->AfterAddFrame();
					}
				}
			}
		}
		catch (...)
		{
			LogError("catch abnormal");
			Reset();
		}
	}
	LogFuncExit();
}

void CPartitionAlgorithm::Distinguish(const std::vector<CFrame*>& frames, ushort teleNum)
{

	LogFuncEntry();
	if (_blocks.size() != 0)
	{
		try
		{
			std::list<CBlock*> collection;
			std::list<CBlock*> list2;
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if (strcmp((*iter)->GetObjectName(), "CTrain") == 0)
				{
					list2.push_back(*iter);
				}
			}
			bool flag = false;
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				if (strcmp((*iter)->GetObjectName(), "CPerson") == 0)
				{
					flag = true;
					break;
				}
			}
			//bool flag2 = this._blocks.Exists(x => x is Animal);
			bool flag2 = false;
			int num = 0;
			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end(); iter++)
			{
				CBlock* block = *iter;

				if (((block == NULL) || block->Confirmed) || (block->LastTeleNum != teleNum))
				{
					continue;
				}

				//<ֻ如果火车不为空，则看新来的block能否组合到火车
				for (std::list<CBlock*>::iterator iter = list2.begin(); iter != list2.end(); iter++)
				{
					//<
					CTrain* train = dynamic_cast<CTrain*>(*iter);
					if (train != NULL && train->CanCombine(block))
					{
						LogDebug("%s is combined into %s", block->Name.c_str(), train->Name.c_str());
						block->SetRemoved(true);
						train->Combine(block);
						break;
					}
				}

				if (!block->Removed)
				{
					if (CTrain::TrainProbability(block) > SickAlgorithm->Info->TrainIdentifyInfo.Probability)
					{
						CFrame* frame = frames.at(0);
						CFrame* frame2 = frames.at(frames.size() - 1);
						if ((frame != NULL) && (frame2 != NULL))
						{
							if (CAlgorithmTool::GetInstance()->GetDistance(frame->First, frame2->Last) > 6000.0)
							{
								block->SetRemoved(true);
								CTrain* item = new CTrain(dynamic_cast<CCrowdBlock*>(block));
								collection.push_back(item);
								list2.push_back(item);
							}
							continue;
						}
					}
					else if (CLandslip::LandslipProbability(block) >= SickAlgorithm->Info->LandslipIdentifyInfo.Probability)
					{
						block->SetRemoved(true);
						collection.push_back(new CLandslip(block));
					}
					else
					{
						double num2 = CPerson::PersonProbability(block);
						//LogDebug("%s person probability is %.2f", block->Name.c_str(), num2);
						if (flag2)
						{
							num2 *= 0.9;
						}

						if (num2 > SickAlgorithm->Info->PersonIdentifyInfo.Probability)
						{
							block->SetRemoved(true);
							CPerson* item = new CPerson(block);

							collection.push_back(item);
							flag = true;
						}
						else if (strcmp(block->GetObjectName(), "CBlock") != 0)
						{
							num++;
							CCrowdBlock *block2 = dynamic_cast<CCrowdBlock*>(block);
							if (block2 != NULL && block2->Length > SickAlgorithm->Info->PersonIdentifyInfo.MaxLength)
							{
								LogTrace("for crowdblock, check every one person probability");
								for (std::list<CBlock*>::iterator iter = block2->_blocks.begin(); iter != block2->_blocks.end(); iter++)
								{
									CBlock* block3 = *iter;
									num2 = CPerson::PersonProbability(block3);

									if (flag2)
									{
										num2 *= 0.9;
									}
									if (num2 > SickAlgorithm->Info->PersonIdentifyInfo.Probability)
									{
										block3->SetRemoved(true);
										flag = true;
										CPerson* item = new CPerson(block3);

										collection.push_back(item);
									}
									else
									{
										block3->SetRemoved(false);
									}
								}
								for (std::list<CBlock*>::iterator iter = block2->_blocks.begin(); iter != block2->_blocks.end();)
								{
									std::list<CBlock*>::iterator iter_e = iter++;
									CBlock* block = *iter_e;
									if (block->Removed)
									{
										block2->_blocks.erase(iter_e);
									}
								}
								if (block2->_blocks.size() == 0)
								{
									block2->SetRemoved(true);
								}
							}
						}
					}
				}
			}

			for (std::list<CBlock*>::iterator iter = _blocks.begin(); iter != _blocks.end();)
			{
				std::list<CBlock*>::iterator iter_e = iter++;
				CBlock* block = *iter_e;
				if (block == NULL)
				{
					LogDebug("%s is removed from blocks of partion but is not deleted");
					_blocks.erase(iter_e);
				}
				else if (block->Removed)
				{
					_blocks.erase(iter_e);
				}
			}
			_blocks.insert(_blocks.begin(), collection.begin(), collection.end());
		}
		catch (...)
		{
			LogError("catch abnomal");
			Reset();
		}
	}
	LogFuncExit();
}


