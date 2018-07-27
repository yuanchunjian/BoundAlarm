
#include "AlgorithrmControl.h"
#include "AlgorithmService.h"
#include <liblogger/liblogger.h>
#include "../main/CThreadLock.h"
#include "PublicHead.h"
CThreadLock g_algorithmRunLock;
CThreadLock g_updateFlagLock;

CAlgorithrmControl::CAlgorithrmControl(void)
{
	_bAlgorithmRunFlag = true;
	_bStopAlgorithmRun = false;
	_keyStatus = true;
	_netStatus = true;
	_updateMsgFlag = false;
	_initInfoFlag = false;
}


CAlgorithrmControl::~CAlgorithrmControl(void)
{
}

int CAlgorithrmControl::InitControl(PFEventCallback pfRetFrameCallback, PFEventCallback pfEventInfoCallback, const char* path, const char* dbPath, void* par)
{
	m_pSickDataAlgrithm.RetPar = par;
	m_pSickDataAlgrithm.m_pfEventInfoCallback = pfEventInfoCallback;
	m_pSickDataAlgrithm.AlarmInfoProcess.m_pfEventInfoCallback = pfEventInfoCallback;
	m_pSickDataAlgrithm.AlarmInfoProcess.RetPar = par;
	m_pfRetFrameInfoCallback = pfRetFrameCallback;
	_strExePath = path;
	m_pRetPar = par;
	if (!m_pSickDataAlgrithm.AlarmInfoProcess.Init(dbPath))
	{
		return INIT_ERROR;
	}
	if (!CAlgorithmService::GetInstance()->Initialize(path))
	{
		return CONFIG_ERROR;
	}
	return SUCCESS;
}

void CAlgorithrmControl::UnInitControl()
{
	CAlgorithmService::GetInstance()->UnInitialize();
}

uchar CAlgorithrmControl::Control(uchar* lmsData, ushort len)
{
	_dataIndex = 0;

	bool bRet = _scanData.Parse(lmsData, _dataIndex, true); //<true
	if (bRet)
	{
		g_updateFlagLock.Lock();
		bool bUpdateMsgFlag = _updateMsgFlag;
	    uchar ucUpdateMsgType = _updateMsgType;
	    if (bUpdateMsgFlag)
	    {
	    	_updateMsgFlag = false;
	    	if (ucUpdateMsgType == UPDATE_CONFIG)
	    	{
	    	    LogInfo("更新配置：先卸载配置.");
	    		UnInitControl();
	    		LogInfo("更新配置：重新加载配置.");
	    	     if (!CAlgorithmService::GetInstance()->Initialize(_strExePath.c_str()))
	    	     {
	    	    	 LogInfo("更新配置：重新加载配置失败，退出.");
	    	    	 exit(ALGORITHM_CONFIG_ERR_EXIT);
	    	     }
	    	     LogInfo("更新配置：重新加载配置成功.");
	    	     _initInfoFlag = false;
	    	 }
	    }
		g_updateFlagLock.UnLock();

		g_algorithmRunLock.Lock();
		bool bFlag = _bAlgorithmRunFlag;
		g_algorithmRunLock.UnLock();


		if (bFlag && bUpdateMsgFlag) {
			m_pSickDataAlgrithm.AlarmInfoProcess.ClearAbnormalAlarm();
			if (ucUpdateMsgType == SORTWARE_UPGRADE) {
				m_pSickDataAlgrithm.AlarmInfoProcess.ResumeAllAlarm();
				exit(UPGRADE_EXIT);
			}
			LogInfo("算法复位");
			m_pSickDataAlgrithm.AlgorithmReset();
			m_partitionAlgorithm.Reset();
		}
		if (!_initInfoFlag) {
			if (!m_pSickDataAlgrithm.InitSickDataAlgorithm(&_scanData)) {
				exit(ALGORITHM_CONFIG_ERR_EXIT);
			}
			_initInfoFlag = true;
		}

		if (m_pfRetFrameInfoCallback != NULL)
	    {
			m_pSickDataAlgrithm.GetRetFramePar(&_scanData);
			m_pfRetFrameInfoCallback(&m_pSickDataAlgrithm.RetFramePar, m_pRetPar);
	    }

		if (bFlag)
        {
			if (_bStopAlgorithmRun)
        	{
        		_bStopAlgorithmRun = false;
        	}
        	float startAngle;
		   ushort scanCounter;
		   std::vector<ushort> Matter;
		   Matter.clear();

		   bRet = m_pSickDataAlgrithm.OnDataChanged(&_scanData, Matter, startAngle, scanCounter);

		   if (bRet)
		   {
			  m_partitionAlgorithm.SickAlgorithm = &m_pSickDataAlgrithm;
			  m_partitionAlgorithm.Partition(Matter, (double) startAngle, scanCounter);
	       }
        }
		else if (bUpdateMsgFlag && ucUpdateMsgType == SORTWARE_UPGRADE)
		{
			exit(UPGRADE_EXIT);
		}
        else if (!_bStopAlgorithmRun)
        {
        	_bStopAlgorithmRun = true;
        	ClearAbnormalPointsProc();
        }
	}
	else
	{
		return DATA_ERROR;
	}
	return SUCCESS;
}

void CAlgorithrmControl::AlarmConfirmProc(int alarmID)
{
	m_pSickDataAlgrithm.AlarmInfoProcess.AlarmConfirm(alarmID);
}

void CAlgorithrmControl::AlarmIgnoreProc(int alarmID)
{
	m_pSickDataAlgrithm.AlarmInfoProcess.AlarmIgnore(alarmID);
}

void CAlgorithrmControl::ClearAbnormalPointsProc()
{
	m_pSickDataAlgrithm.AlarmInfoProcess.ClearAbnormalAlarm();
	//m_pSickDataAlgrithm.AlarmInfoProcess.ResumeAllAlarm();
	LogInfo("算法复位");
	m_pSickDataAlgrithm.AlgorithmReset();
	m_partitionAlgorithm.Reset();
}

void CAlgorithrmControl::AllAlarmIgnoreProc()
{
	m_pSickDataAlgrithm.AlarmInfoProcess.IgnoreAllAlarm();
}

void CAlgorithrmControl::AllAlarmResumeProc()
{
	m_pSickDataAlgrithm.AlarmInfoProcess.ResumeAllAlarm();
}

void CAlgorithrmControl::ResendAllAlarm()
{
	LogInfo("重新发送所有报警");
	m_pSickDataAlgrithm.AlarmInfoProcess.SendAllAlarm();
}

void CAlgorithrmControl::SetAlgorithmRun(bool bRunFlag, int type)
{
    switch (type)
    {
    case KEY_STATUS:
    	LogInfo("key状态：%d", bRunFlag);
    	_keyStatus = bRunFlag;
    	break;
    case NET_STATUS:
    	LogInfo("网络状态：%d", bRunFlag);
    	_netStatus = bRunFlag;
    	break;
    default:
    	break;
    }
	g_algorithmRunLock.Lock();
	if (_keyStatus && _netStatus)
	{
		LogInfo("key状态和网络状态都正常，算法运行");
		_bAlgorithmRunFlag = true;
	}
	else
	{
		LogInfo("key状态或网络状态不正常，算法停止");
		_bAlgorithmRunFlag = false;
	}
	g_algorithmRunLock.UnLock();
}

void CAlgorithrmControl::UpdateMsgProc(uchar type)
{
	g_updateFlagLock.Lock();
	_updateMsgFlag = true;
	_updateMsgType = type;
	g_updateFlagLock.UnLock();
}
