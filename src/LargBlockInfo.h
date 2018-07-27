#ifndef _LargBlockInfo_H
#define _LargBlockInfo_H
#include "SegmentInfo.h"
#include "CustomType.h"

/*-----------------------------CLargBlockInfo---------------------------------------*/
class CLargBlockInfo
{
public:
	CLargBlockInfo(CPOINT currentPos, ushort lastTeleNum);
	virtual ~CLargBlockInfo();

	double BlockDirection;
	double Distance;
	CPOINT FirstPos;
	ushort FirstTeleNum;
	double MoveDirection;
	int ValidCount;
};

#endif
