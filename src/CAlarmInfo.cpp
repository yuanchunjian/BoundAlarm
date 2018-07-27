/*
 * CAlarmInfo.cpp
 *
 *  Created on: May 12, 2016
 *      Author: yuan
 */

#include "CAlarmInfo.h"

CAlarmInfo::CAlarmInfo() {
	// TODO Auto-generated constructor stub
	blockPar = NULL;
	EventInfo = NULL;
	EventInfo = new S_EventInfo;
	if (EventInfo == NULL)
	{

	}
	EventInfo->eventPar = NULL;
	blockPar = new S_BlockEventPar;
	if (blockPar == NULL)
	{

	}
	EventInfo->eventPar = blockPar;
	DeleteFlag = true;
}

CAlarmInfo::~CAlarmInfo() {
	// TODO Auto-generated destructor stub
	if (EventInfo != NULL)
	{
		if (EventInfo->eventPar != NULL)
		{
			delete (S_BlockEventPar*)EventInfo->eventPar;
			EventInfo->eventPar = NULL;
		}
		delete EventInfo;
		EventInfo = NULL;
	}
}
