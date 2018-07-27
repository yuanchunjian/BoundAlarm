#pragma once
#ifndef _AlarmInfoProcess_H
#define _AlarmInfoProcess_H

#include "CheckAlgorithm.h"
#include "SegmentInfo.h"
#include "CAlarmRecordService.h"
#include "CUpgradeAlarm.h"
#include <map>

#define ALARM_NORMAL  0x00
#define ALARM_CONFIRM 0x01
#define ALARM_IGNORE  0x02

#define NO_RECOVER_ONE_LEVEL_ALARM 0x01
#define NO_RECOVER_TOW_LEVEL_ALARM 0x02
#define RECOVER_ONE_LEVEL_ALARM    0x03
#define RECOVER_TWO_LEVEL_ALARM    0x04

#define ALARM_CONFIRM_FAIL               0x00  //<ȷ��ʧ�ܣ�û�ҵ�����������������
#define ALARM_CONFIRM_TO_ONE_LEVEL       0x01
#define REMOVED_ONE_LEVEL_ALARM_CONFIRM  0x02

#define ALARM_ALL_IGNORE   0x00

#define ALARM_IGNORE_FAIL  0x00
#define ALARM_IGNORE_SUCC  0x01

class CAlarmInfoProcess
{
public:
	CAlarmInfoProcess(void);
	virtual ~CAlarmInfoProcess(void);

	bool Init(const char* dbPath);

	void AddAlarmInfo(std::string strName, CAlarmInfo* info);
	void AlarmConfirm(int alarmID);
	void AlarmConfirm(CAlarmInfo* info);
	void ConfirmNormal(CAlarmInfo* info);
	void AlarmIgnore(int alarmID);
	void IgnoreAllAlarm();
	void ClearAbnormalAlarm();
	bool CheckAbnormalPoint(CPolarArea& curArea);
	void AddCurLazyArea(CPolarArea curArea);
	void ClearAllAlarm();
	void ResumeAllAlarm();
	void SendAllAlarm();
	void ClearUpgradeAlarm();
	//<事件信息回调
	PFEventCallback m_pfEventInfoCallback;
	void* RetPar;
    int UpgradeOneLevelAlarmTime;

    CAlarmRecordService AlarmRecord;
private:
	void AlarmIgnore(CAlarmInfo* info);
	//<

	//std::vector<CPolarArea> _currLazyArea;
	/*
	 * alarm name 和 alarmInfo的对应关系
	 */
	//std::map<std::string, CAlarmInfo*> _currAlarmInfo;

	/*
		 * 报警ID和name对应关系
		 */
	//std::map<int, std::string> _idMapName;

	static void HasAlarmUpgrade(int id, CAlarmInfo* info, void* retPar);
	CUpgradeAlarm _upgradeAlarm;
	bool _bInitUpgradeAlarm;
};

#endif

