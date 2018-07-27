#pragma once
#include "ScanDataParse.h"
#include "AlgorithmInfo.h"
#include "Segment.h"
#include "CheckAlgorithm.h"
#include <map>
#include "Track.h"
#include "AlarmInfoProcess.h"
#include <stdlib.h>
#include <stdio.h>

//extern "C"  typedef void ( *PFStatusChangeCallback)(RET_EVENT_PAR* eventPar,  void* retPar);

const int TrackGauge = 0x59b;  //<轨道距离

class CBlockNameService
{
public:
	std::string GetNewName(std::string category = "");

	void Reset();
private:
	std::map<std::string, uint> _serialNum;
};


class CSickDataAlgorithm
{
public:
	CSickDataAlgorithm(void);
	virtual ~CSickDataAlgorithm(void);

	bool InitSickDataAlgorithm(CScanDataParse* current);

	void AlgorithmReset();
	void ClearNormalArea();
	void ClearTrackData();
	bool OnDataChanged(CScanDataParse* _current, std::vector<ushort> &Matter, float &sAngle, ushort& sCounter);

	void GetRetFramePar(CScanDataParse* __current);

	CPOINT AnomalPoint;

	CProtectArea* Area;

	std::vector<ushort> Contour; 
     
	CAlgorithmInfo* Info;
	CUnDetectedArea* UnDetectedArea;
	std::vector<CPolarArea> LazyArea;
	CTrack* Track;
	
	CBlockNameService* NameService;
	//<事件信息回调
	PFEventCallback m_pfEventInfoCallback;
	void ReleaseCallMemory();
	S_EventInfo* EventInfo;
	void* RetPar;
	S_RetFramePar RetFramePar;
	CAlarmInfoProcess AlarmInfoProcess;
    void AlterTrainFlag(bool flag);
    /*
     * 报警信息改变
     */
    void AlarmChange(std::string strName, CAlarmInfo *alarmInfo);

private:
	void Prepare();
    void RemoveRestoreAlarms();
   // void ReleaseAlarms();
	ushort _lastTelNum;
	std::vector<ushort> _teleNumList;
	std::vector<CScanDataParse> _trainData;
	 bool _trainFlag;
	 bool _initInfoFlag;
	 int _trainPassTime;
	std::map<CAlarmInfo*, int> _restoreAlarms;
};

