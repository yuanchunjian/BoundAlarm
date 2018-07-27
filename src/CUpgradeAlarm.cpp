/*
 * CUpgradeAlarm.cpp
 *
 *  Created on: May 13, 2016
 *      Author: yuan
 */

#include "CUpgradeAlarm.h"
#include <sys/time.h>
#include <signal.h>
#include "../main/CThreadLock.h"
#include <liblogger/liblogger.h>

CThreadLock g_lockUpgradeAlarm;
std::map<int, CAlarmInfo*> g_upgradeAlarm;
PFUpgradeCallback g_pfUpgradeCallback;
void* g_retPar;
int g_upgradeTime;

CUpgradeAlarm::CUpgradeAlarm() {
	// TODO Auto-generated constructor stub

}

CUpgradeAlarm::~CUpgradeAlarm() {
	// TODO Auto-generated destructor stub
}

void CUpgradeAlarm::AddUpgradeAlarm(int id, CAlarmInfo* info)
{
	LogInfo("Add upgrade alarm id = %d", id);
	g_lockUpgradeAlarm.Lock();
	g_upgradeAlarm.insert(make_pair(id, info));
	g_lockUpgradeAlarm.UnLock();
}

void CUpgradeAlarm::RemoveUpgradeAlarm(int id)
{
	LogInfo("Remove  upgrade alarm id = %d", id);
	g_lockUpgradeAlarm.Lock();
	std::map<int, CAlarmInfo*>::iterator iter = g_upgradeAlarm.find(id);
	if (iter != g_upgradeAlarm.end())
	{
	   g_upgradeAlarm.erase(iter);
	}
	g_lockUpgradeAlarm.UnLock();
}

void CUpgradeAlarm::RemoveAllUpgradeAlarm()
{
	LogFuncEntry();
	g_lockUpgradeAlarm.Lock();
	g_upgradeAlarm.clear();
	g_lockUpgradeAlarm.UnLock();
	LogFuncExit();
}

bool CUpgradeAlarm::Init(int upgradeTime, PFUpgradeCallback callback, void* par)
{
	g_pfUpgradeCallback = callback;
	g_retPar = par;
	g_upgradeTime = upgradeTime;
	_bInitFlag = true;
	return true;
}

void CUpgradeAlarm::handler(void* par)
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	g_lockUpgradeAlarm.Lock();
	for (std::map<int, CAlarmInfo*>::iterator iter = g_upgradeAlarm.begin(); iter != g_upgradeAlarm.end();) {
		if (iter->second->AlarmTime + g_upgradeTime <= tv.tv_sec) {
			g_pfUpgradeCallback(iter->first, iter->second, g_retPar);
			LogInfo("Remove  upgrade alarm id = %d", iter->first);
			g_upgradeAlarm.erase(iter++);
		} else {
			iter++;
		}
	}
	g_lockUpgradeAlarm.UnLock();
}

