#pragma once
#include "ScanDataParse.h"
#include "SickDataAlgorithm.h"
#include "ScanDataParse.h"
#include "PartitionAlgorithm.h"
#include "CheckAlgorithm.h"

typedef enum
{
	KEY_STATUS,
	NET_STATUS
}AlgorithmStatusType;

class CAlgorithrmControl
{
public:
	CAlgorithrmControl(void);
	virtual ~CAlgorithrmControl(void);
	int InitControl(PFEventCallback pfRetFrameCallback, PFEventCallback pfEventInfoCallback, const char* path, const char* dbPath, void* par);
	void UnInitControl();
	void AlarmConfirmProc(int alarmID);
	void AlarmIgnoreProc(int alarmID);
	void ClearAbnormalPointsProc();
	void AllAlarmIgnoreProc();
	void AllAlarmResumeProc();
	void ResendAllAlarm();
	uchar Control(uchar* lmsData, ushort len);
    void SetAlgorithmRun(bool bRunFlag, int type);
    void UpdateMsgProc(uchar type);
private:
    bool _bAlgorithmRunFlag;
    bool _bStopAlgorithmRun;
	CScanDataParse _scanData;
	int _dataIndex;
	CSickDataAlgorithm m_pSickDataAlgrithm;
	CPartitionAlgorithm m_partitionAlgorithm;
	PFEventCallback m_pfRetFrameInfoCallback;
	void* m_pRetPar;
	std::string _strExePath;
	bool _keyStatus;
	bool _netStatus;
	bool _updateMsgFlag;
	uchar _updateMsgType;
	bool _initInfoFlag;
};

