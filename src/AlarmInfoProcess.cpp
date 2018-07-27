
#include "AlarmInfoProcess.h"
#include <stdlib.h>
#include <math.h>
#include "AlgorithmTool.h"
#include "../main/CThreadLock.h"
#include <liblogger/liblogger.h>
#include <sys/time.h>

CThreadLock g_currAlarmInfoLock;
CThreadLock g_idMapNameLock;
CThreadLock g_currLazyAreaLock;

std::vector<CPolarArea> _currLazyArea;
/*
* alarm name 和 alarmInfo的对应关系
*/
std::map<std::string, CAlarmInfo*> _currAlarmInfo;

/*
* 报警ID和name对应关系
*/
std::map<int, std::string> _idMapName;

CAlarmInfoProcess::CAlarmInfoProcess(void)
{
	_bInitUpgradeAlarm = false;
	UpgradeOneLevelAlarmTime = 25;
	_currAlarmInfo.clear();
	_currLazyArea.clear();
	_idMapName.clear();
}

CAlarmInfoProcess::~CAlarmInfoProcess(void)
{
}

bool CAlarmInfoProcess::Init(const char* dbPath)
{
	if (!_bInitUpgradeAlarm)
	{
	    _upgradeAlarm.Init(UpgradeOneLevelAlarmTime, HasAlarmUpgrade, this);
	    _bInitUpgradeAlarm = true;
	}
	AlarmRecord.SetDbPath(dbPath);
	bool bRet = AlarmRecord.Create();
	/*if (bRet)
	{
		CAlarmRecordService::RemoveExtendedRecord();
	}*/
	return bRet;
}

void CAlarmInfoProcess::AddAlarmInfo(std::string strName, CAlarmInfo* info)
{
	CAlarmInfo* tmpInfo = NULL;
	g_currAlarmInfoLock.Lock();
	std::map<std::string, CAlarmInfo*>::iterator iter = _currAlarmInfo.find(strName);
	int id;
	if (iter == _currAlarmInfo.end())
	{
		if (info->blockPar->alarmStateType == Resume)
		{
			g_currAlarmInfoLock.UnLock();
            LogDebug("取消掉报警恢复，直接返回");
			return;
		}
		id = AlarmRecord.AddRecord(info);
		LogInfo("Add record id = %d", id);
		if (id == -1)
		{
            LogError("Insert alarm info fail");
            g_currAlarmInfoLock.UnLock();
            return;
		}
		info->blockPar->ID = id;
		tmpInfo = info;
	}
	else
	{
		//mod by xxf2
		iter->second->blockPar->fAngle = info->blockPar->fAngle;
		iter->second->blockPar->fRadialLen = info->blockPar->fRadialLen;
		iter->second->blockPar->fRemainTime = info->blockPar->fRemainTime;
		iter->second->blockPar->fBlockLen = info->blockPar->fBlockLen;
		iter->second->blockPar->frameNO = info->blockPar->frameNO;
		tmpInfo = iter->second;
		//tmpInfo->blockPar->ID = iter->second->blockPar->ID;
	}

	CAlarmInfo* tmp = NULL;

    if (info->blockPar->alarmStateType == Alarm)
    {
    	_currAlarmInfo.insert(make_pair(strName, info));
    	info->DeleteFlag = false;
    	g_idMapNameLock.Lock();
    	_idMapName[tmpInfo->blockPar->ID] = strName;
    	g_idMapNameLock.UnLock();
    	_upgradeAlarm.AddUpgradeAlarm(tmpInfo->blockPar->ID, info);
    }
    else if (info->blockPar->alarmStateType == Resume)
    {
    	 tmpInfo->blockPar->alarmStateType = Resume;
    	 tmpInfo->blockPar->fRemainTime = info->blockPar->fRemainTime;
    	//<报警恢复，所以去掉升级
    	_upgradeAlarm.RemoveUpgradeAlarm(tmpInfo->blockPar->ID);

    	if (iter != _currAlarmInfo.end())
    	{
    		tmp = iter->second;
    		_currAlarmInfo.erase(iter);
    	}

    	g_idMapNameLock.Lock();
    	std::map<int, std::string>::iterator iter1 = _idMapName.find(tmpInfo->blockPar->ID);
    	if (iter1 != _idMapName.end())
    	{
    		_idMapName.erase(iter1);
    	}
    	g_idMapNameLock.UnLock();
    }
    g_currAlarmInfoLock.UnLock();
    //<调用回调事件
    m_pfEventInfoCallback(tmpInfo->EventInfo, RetPar);

    if (tmp != NULL)
    {
         delete tmp;
         tmp = NULL;
     }
}

void CAlarmInfoProcess::AlarmConfirm(int alarmID)
{
	LogInfo("ID：%d, 报警确认", alarmID);
	g_idMapNameLock.Lock();
	std::map<int, std::string>::iterator iter = _idMapName.find(alarmID);
	if (iter == _idMapName.end())
	{
		g_idMapNameLock.UnLock();
		return;
	}
	g_idMapNameLock.UnLock();
    //当前的报警信息中查找信息
	std::string strName = iter->second;
	g_currAlarmInfoLock.Lock();
	std::map<std::string, CAlarmInfo*>::iterator iter1 = _currAlarmInfo.find(strName);
	if (iter1 == _currAlarmInfo.end())
	{
		g_currAlarmInfoLock.UnLock();
		return;
	}
	g_currAlarmInfoLock.UnLock();

	CAlarmInfo* tmpInfo = iter1->second;
    if (tmpInfo->blockPar->eventType == ONE_LEVEL_ALARM)
    {
    	LogInfo("ID: %d 已经为1级报警，不用确认", alarmID);
    	return;
    }

    tmpInfo->blockPar->eventType = ONE_LEVEL_ALARM;
    tmpInfo->blockPar->alarmStateType = Alarm;

    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    tmpInfo->blockPar->fRemainTime = tv.tv_sec - tmpInfo->AlarmTime + (tv.tv_usec - tmpInfo->AlarmTime_ms)/1000.0;
    //<调用回调事件
    m_pfEventInfoCallback(tmpInfo->EventInfo, RetPar);
    //<去掉由于已经为1级报警，所以去掉升级
    LogInfo("ID: %d 确认为1级报警，从升级中去掉", alarmID);
    _upgradeAlarm.RemoveUpgradeAlarm(alarmID);
}

void CAlarmInfoProcess::AlarmConfirm(CAlarmInfo* info)
{
	info->blockPar->eventType = ONE_LEVEL_ALARM;
	info->blockPar->alarmStateType = Alarm;
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	info->blockPar->fRemainTime = (tv.tv_sec - info->AlarmTime)*1000.0 + (tv.tv_usec - info->AlarmTime_ms)/1000.0;
	    //<调用回调事件
	m_pfEventInfoCallback(info->EventInfo, RetPar);
}

void CAlarmInfoProcess::ConfirmNormal(CAlarmInfo* info)
{
	CPolarArea curArea = info->curArea;
	if (curArea.CheckValidFlag())
	{
	   curArea.Enlarge(2.0, 500.0);
	   LogInfo("增加异常点面积");
	   g_currLazyAreaLock.Lock();
	   _currLazyArea.push_back(curArea);
	   g_currLazyAreaLock.UnLock();
	}
	else
	{
		LogError("当前面积无效");
	}
}

void CAlarmInfoProcess::AlarmIgnore(CAlarmInfo* info)
{

}

void CAlarmInfoProcess::AlarmIgnore(int alarmID)
{
	//<根据报警报警ID查找
	LogInfo("ID: %d，报警取消", alarmID);
	g_idMapNameLock.Lock();
	std::map<int, std::string>::iterator iter = _idMapName.find(alarmID);
	if (iter == _idMapName.end())
	{
		g_idMapNameLock.UnLock();
		LogInfo("ID: %d，idMapName没有找到，直接返回", alarmID);
		return;
	}
	_idMapName.erase(iter);
	g_idMapNameLock.UnLock();
	//当前的报警信息中查找信息
    std::string strName = iter->second;
    g_currAlarmInfoLock.Lock();
	std::map<std::string, CAlarmInfo*>::iterator iter1 = _currAlarmInfo.find(strName);
	if (iter1 == _currAlarmInfo.end())
	{
		g_currAlarmInfoLock.UnLock();
		LogInfo("ID: %d，currAlarmInfo没有找到，直接返回", alarmID);
		return;
	}
	g_currAlarmInfoLock.UnLock();

	CAlarmInfo* tmpInfo = iter1->second;
    tmpInfo->blockPar->alarmStateType = Cancel;

    m_pfEventInfoCallback(tmpInfo->EventInfo, RetPar);
    ConfirmNormal(tmpInfo);

 /*   g_idMapNameLock.Lock();
    std::map<int, std::string>::iterator iter1 = _idMapName.find(tmpInfo->blockPar->ID);
    if (iter1 != _idMapName.end())
    {
         _idMapName.erase(iter1);
    }
     g_idMapNameLock.UnLock();
    */
    LogInfo("ID: %d，报警取消，从升级中去掉", alarmID);
    _upgradeAlarm.RemoveUpgradeAlarm(alarmID);
    CAlarmInfo* tmp = iter1->second;
    g_currAlarmInfoLock.Lock();
    LogInfo("ID: %d，报警取消，从currAlarmInfo中去掉", alarmID);
    _currAlarmInfo.erase(iter1);
    g_currAlarmInfoLock.UnLock();
    if (tmp != NULL)
    {
    	delete tmp;
    	tmp = NULL;
    }
}

void CAlarmInfoProcess::IgnoreAllAlarm()
{
	LogInfo("取消所有报警");
	_upgradeAlarm.RemoveAllUpgradeAlarm();
	g_currAlarmInfoLock.Lock();
	for (std::map<std::string, CAlarmInfo*>::iterator iter = _currAlarmInfo.begin(); iter != _currAlarmInfo.end(); iter++)
	{
		//当前的报警信息中查找信息
		CAlarmInfo* tmpInfo = iter->second;
		tmpInfo->blockPar->alarmStateType = Cancel;
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		tmpInfo->blockPar->fRemainTime = (tv.tv_sec - tmpInfo->AlarmTime)*1000.0 + (tv.tv_usec - tmpInfo->AlarmTime_ms)/1000.0;

		LogInfo("ID：%d, 取消报警", tmpInfo->blockPar->ID);
		m_pfEventInfoCallback(tmpInfo->EventInfo, RetPar);
		ConfirmNormal(tmpInfo);
   }
	ClearAllAlarm();
	g_currAlarmInfoLock.UnLock();
}

void CAlarmInfoProcess::ClearAllAlarm()
{
	LogFuncEntry();
	g_idMapNameLock.Lock();
	_idMapName.clear();
	g_idMapNameLock.UnLock();

	for (std::map<std::string, CAlarmInfo*>::iterator iter = _currAlarmInfo.begin(); iter != _currAlarmInfo.end();)
	{
		CAlarmInfo* tmp = iter->second;
		_currAlarmInfo.erase(iter++);
		if (tmp != NULL)
		{
		    delete tmp;
		    tmp = NULL;
		}
	}
	LogFuncExit();
}

void CAlarmInfoProcess::ClearAbnormalAlarm()
{
	LogInfo("清除异常点");
	g_currLazyAreaLock.Lock();
	_currLazyArea.clear();
	g_currLazyAreaLock.UnLock();
}

void CAlarmInfoProcess::AddCurLazyArea(CPolarArea curArea)
{
	LogInfo("增加异常点面积");
	g_currLazyAreaLock.Lock();
	_currLazyArea.push_back(curArea);
	g_currLazyAreaLock.UnLock();
}

bool CAlarmInfoProcess::CheckAbnormalPoint(CPolarArea& curArea)
{
	LogInfo("检测面积是否在异常点面积中");
	g_currLazyAreaLock.Lock();
	for (int i = 0; i < _currLazyArea.size(); i++)
	{
        if (CAlgorithmTool::GetInstance()->IsInArea(_currLazyArea.at(i), curArea))
        {
        	g_currLazyAreaLock.UnLock();
        	LogInfo("报警为异常点面积，不报警");
        	return true;
        }
	}
	g_currLazyAreaLock.UnLock();
	LogInfo("报警不在异常点面积，报警");
	return false;
}

void CAlarmInfoProcess::HasAlarmUpgrade(int id, CAlarmInfo* info, void* retPar)
{
	LogInfo("ID: %d 到达一级报警时间升级为1级报警", id);
	CAlarmInfoProcess* pThis = (CAlarmInfoProcess*)retPar;
	pThis->AlarmConfirm(info);
}

void CAlarmInfoProcess::ClearUpgradeAlarm()
{
	_upgradeAlarm.RemoveAllUpgradeAlarm();
}

void CAlarmInfoProcess::ResumeAllAlarm()
{
	LogFuncEntry();
	LogInfo("恢复所有报警");
    try{
    	_upgradeAlarm.RemoveAllUpgradeAlarm();
    	    g_currAlarmInfoLock.Lock();

    	    LogInfo("恢复报警开始");
    	    std::map<std::string, CAlarmInfo*>::iterator iter = _currAlarmInfo.begin();
    	    for (; iter != _currAlarmInfo.end(); iter++)
    	    {
    	    	//当前的报警信息中查找信息
    	    	CAlarmInfo* tmpInfo = iter->second;
    	    	if (tmpInfo == NULL)
    	    	{
    	    		LogError("报警信息为空");
    	    		continue;
    	    	}
    	    	LogInfo("ID：%d, 恢复报警", tmpInfo->blockPar->ID);
    	    	tmpInfo->blockPar->alarmStateType = Resume;
    	    	struct timeval tv;
    	    	struct timezone tz;
    	    	gettimeofday(&tv, &tz);
    	    	tmpInfo->blockPar->fRemainTime = (tv.tv_sec - tmpInfo->AlarmTime)*1000.0 + (tv.tv_usec - tmpInfo->AlarmTime_ms)/1000.0;

    	    	m_pfEventInfoCallback(tmpInfo->EventInfo, RetPar);
    	     }
    	    LogInfo("恢复报警发送完成");
    	    ClearAllAlarm();
    	    g_currAlarmInfoLock.UnLock();
    }catch(...)
    {
        LogError("catch abnomal");
    }
    LogFuncExit();
}

void CAlarmInfoProcess::SendAllAlarm()
{
	g_currAlarmInfoLock.Lock();
	 for (std::map<std::string, CAlarmInfo*>::iterator iter = _currAlarmInfo.begin(); iter != _currAlarmInfo.end(); iter++)
	 {
	    	//当前的报警信息中查找信息
	      CAlarmInfo* tmpInfo = iter->second;

	      LogInfo("ID：%d, 发送报警", tmpInfo->blockPar->ID);
	      m_pfEventInfoCallback(tmpInfo->EventInfo, RetPar);
	  }
	  g_currAlarmInfoLock.UnLock();
}



