/*
 * CAlarmInfo.h
 *
 *  Created on: May 12, 2016
 *      Author: yuan
 */

#ifndef CALARMINFO_H_
#define CALARMINFO_H_
#include "Segment.h"
#include "CheckAlgorithm.h"

class CAlarmInfo
{
public:
	CAlarmInfo();

	~CAlarmInfo();

	bool DeleteFlag;
	int AlarmTime;   //<信息时间，以seconds为单位
	int AlarmTime_ms; //<信息时间，以微妙为单位
	bool HasSent;  //<信息是否已经发送过
	CPolarArea curArea;
	S_BlockEventPar *blockPar;
	S_EventInfo* EventInfo;
};

#endif /* CALARMINFO_H_ */
