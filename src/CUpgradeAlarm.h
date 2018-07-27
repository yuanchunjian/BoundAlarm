/*
 * CUpgradeAlarm.h
 *
 *  Created on: May 13, 2016
 *      Author: yuan
 */

#ifndef CUPGRADEALARM_H_
#define CUPGRADEALARM_H_
#include "CAlarmInfo.h"
#include <map>

extern "C"  typedef void ( *PFUpgradeCallback)(int id, CAlarmInfo* info,  void* retPar);

class CUpgradeAlarm {
public:
	CUpgradeAlarm();
	virtual ~CUpgradeAlarm();

	bool Init(int upgradeTime, PFUpgradeCallback callback, void* par);
	void RemoveAllUpgradeAlarm();
	void RemoveUpgradeAlarm(int id);
	void AddUpgradeAlarm(int id, CAlarmInfo* info);
	static void handler(void* par);
private:
	bool _bInitFlag;
};

#endif /* CUPGRADEALARM_H_ */
