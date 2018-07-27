
#include "LargBlockInfo.h"
//#include "AlgorithmTool.h"

CLargBlockInfo::CLargBlockInfo(CPOINT currentPos, ushort lastTeleNum)
{
	FirstPos = currentPos;
	FirstTeleNum = lastTeleNum;
	ValidCount = 0;
}

CLargBlockInfo::~CLargBlockInfo()
{

}

//void CLargBlockInfo::CalDistanceAndDirection(const CBlock* block)
//{
//	double speed = block->Speed;
//	double standSpeed = block->SickAlgorithm->Info->StandSpeed;
//	if (speed < standSpeed)
//	{
//		FirstPos = block->CurrentPos;
//		FirstTeleNum = block->LastTeleNum;
//		ValidCount = 0;
//	}
//	double distance = CAlgorithmTool::GetInstance()->GetDistance(block->CurrentPos, FirstPos);
//	BlockDirection = CAlgorithmTool::GetInstance()->GetDirection(block->RightPos, block->LeftPos);
//	MoveDirection = CAlgorithmTool::GetInstance()->GetDirection(FirstPos, block->CurrentPos);
//	double num2 = CAlgorithmTool::GetInstance()->IntersectAngle(BlockDirection, MoveDirection);
//	Distance = abs((double)(distance * sin(CAlgorithmTool::GetInstance()->ToArc(num2))));
//	if (Distance > block->SickAlgorithm->Info->LandslipIdentifyInfo.MinDistance)
//	{
//		ValidCount++;
//	}
//	else
//	{
//		ValidCount = 0;
//	}
//}
