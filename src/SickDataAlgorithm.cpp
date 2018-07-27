
#include "SickDataAlgorithm.h"
#include "AlgorithmTool.h"
#include "AlgorithmService.h"
#include "CustomType.h"
#include <sys/time.h>
#include <liblogger/liblogger.h>
#include "../main/CThreadLock.h"
CThreadLock g_NameLock;

std::string CBlockNameService::GetNewName(std::string category)
{
		ushort num = 0;
		g_NameLock.Lock();
		map<std::string, uint>::iterator result= _serialNum.find(category);
		if (result != _serialNum.end())
		{
			num = _serialNum[category];

			_serialNum[category] = num+1;
		}
		else
		{
			_serialNum.insert(make_pair(category, num+1));
		}
		g_NameLock.UnLock();
		num = (ushort)(num + 1);

		char strName[256];
		if (category.length() > 0)
		{
			sprintf(strName, "%c%d", category.at(0), num);
		}
		else
		{
			sprintf(strName, "%d", num);
		}

		return strName;
}

void CBlockNameService::Reset()
{
	g_NameLock.Lock();
	_serialNum.clear();
	g_NameLock.UnLock();
}


CSickDataAlgorithm::CSickDataAlgorithm(void)
{
	_lastTelNum = 0;
	NameService = new CBlockNameService;
	Area = NULL;
	UnDetectedArea = NULL;
	Track = NULL;
	Info = NULL;
	_initInfoFlag = false;
	EventInfo = NULL;
	_trainFlag = false;
	_trainPassTime = 0;
}


CSickDataAlgorithm::~CSickDataAlgorithm(void)
{
	if (NameService != NULL)
	{
		delete NameService;
		NameService = NULL;
	}
	if (Area != NULL)
	{
		delete Area;
		Area = NULL;
	}
	if (UnDetectedArea != NULL)
	{
		delete UnDetectedArea;
		UnDetectedArea = NULL;
	}
	if (Track != NULL)
	{
		delete Track;
		Track = NULL;
	}
	AlarmInfoProcess.AlarmRecord.Close();
}

void CSickDataAlgorithm::AlgorithmReset()
{
	try
	{
		NameService->Reset();
		/*for (std::map<CAlarmInfo*, int>::iterator iter = _restoreAlarms.begin(); iter
				!= _restoreAlarms.end();) {

			CAlarmInfo* tmp = iter->first;
			_restoreAlarms.erase(iter++);
			if (tmp != NULL) {
				delete tmp;
				tmp = NULL;
			}
			iter++;
		}*/
	}
	catch (...)
	{
		
	}
}

void CSickDataAlgorithm::ClearNormalArea()
{
	Info->LazyArea.clear();
}

void CSickDataAlgorithm::ClearTrackData()
{
		/*Track.Near.Data = (ushort[]) (this.Track.Far.Data = null);
		Track.SaveConfig();
		Area.CreateWithoutTrack(this._info);
		Area.SaveConfig();*/
}


bool CSickDataAlgorithm::InitSickDataAlgorithm(CScanDataParse* _current) {
	//_current = current;

	COutputchannel16 outputchannel = _current->Channel16s.at(0);
	Info = CAlgorithmService::GetInstance()->GetAlgorithmInfo(
			_current->Device.SerialNumber);
	if (Info == NULL)
	{
		/*Info = CAlgorithmService::GetInstance()->CreateAlgorithmInfo();
		Info->SerialNumber = _current->Device.SerialNumber;
		CAlgorithmService::GetInstance()->AddAlgorithmInfo(Info);
		bFlag = false;*/
		LogError("没找到序号：%d的算法配置信息." ,_current->Device.SerialNumber);
		return false;
	}
	Info->Resolution = outputchannel.Steps;
	Info->Frequency = _current->Frequencys.ScanFrequency;
	Info->StartAngle = outputchannel.StartAngle;
	Info->EndAngle = outputchannel.StartAngle
			+ ((outputchannel.Data.size() - 1) * outputchannel.Steps);
	Prepare();
	return true;
}

bool CSickDataAlgorithm::OnDataChanged(CScanDataParse* _current, std::vector<ushort> &Matter, float &sAngle, ushort& sCounter)
{
		if (_current->StatusInfo.ScanCounter > 8400)
		{//

		}
		Matter.resize(_current->Channel16s[0].Data.size());
		if (Contour.size() != _current->Channel16s.at(0).Data.size())
		{
			Contour.resize(_current->Channel16s[0].Data.size());
		}
		COutputchannel16 outputchannel = _current->Channel16s.at(0);

		int num = (int)((double) ((Area->GetStartAngle() - outputchannel.StartAngle) / ((double) outputchannel.Steps)));
		int num2 = (int)((double) ((UnDetectedArea->SegInfo->StartAngle - outputchannel.StartAngle) / ((double) outputchannel.Steps)));
		int num3 = num + Area->GetDataLength();
		for (int i = 0; i < outputchannel.Data.size(); i++)
		{
			ushort num5 = outputchannel.Data.at(i);
			//<进行滤波
			if ((num5 < Info->MinValue) || (num5 > Info->MaxValue))
			{
				if ((((i < (outputchannel.Data.size() - 2)) && (i != 0)) && ((outputchannel.Data[i - 1] > Info->MinValue) && (outputchannel.Data[i - 1] < Info->MaxValue))) 
					&& ((outputchannel.Data[i + 1] >Info->MinValue) && (outputchannel.Data[i + 1] < Info->MaxValue)))
				{
					num5 = outputchannel.Data[i - 1];
				}
				else
				{
					num5 = 0xffff;
				}
			}
			if (((i >= num) && (i < num3)) && (Area->IsInArea(i - num, (double) num5) && !UnDetectedArea->IsInArea(i - num2, (double) num5)))
			{
				Contour[i] = Info->MaxValue;
				Matter[i] = num5;
			}
			else
			{
				Contour[i] = num5;
				Matter[i] = Info->MaxValue;
			}
		}

		ushort scanCounter = _current->StatusInfo.ScanCounter;
		//<桢中断不小于等于50
		if (CAlgorithmTool::GetInstance()->GetFrameDiff(_lastTelNum, scanCounter) < 50)
		{
			_lastTelNum = scanCounter;
			_teleNumList.clear();
			sAngle = outputchannel.StartAngle;
			sCounter = scanCounter;
			return true;
		}
		else
		{
			_teleNumList.push_back(scanCounter);
			if (_teleNumList.size() > 5)
			{
				int num7 = _teleNumList.size() - 1;
				bool flag = true;
				int num8 = 0;
				while (num8 < 4)
				{
					//<如果5个桢号中，有2个桢差大于20，则为跳过
					if (CAlgorithmTool::GetInstance()->GetFrameDiff(_teleNumList[num7 - 1], _teleNumList[num7]) > 20)
					{
						flag = false;
						break;
					}
					num8++;
					num7--;
				}
				if (flag)
				{
					_lastTelNum = _teleNumList.at(_teleNumList.size() - 1);
					_teleNumList.clear();
				}
			}
		}
		return false;
}

void CSickDataAlgorithm::Prepare()
{
	Area = CProtectArea::CreateProtectArea(&Info->AreaInfo);
	
	Track = CTrack::CreateTrack(Info);
	/*CPartitionAlgorithm algorithm;
		SickAlgorithm = this
	this._partition = algorithm;*/
	//MsgService.Clear();
	NameService->Reset();
	//RightAlgorithm = RMService.Instance.GetRightAlgorithm(this);
	//LeftAlgorithm = RMService.Instance.GetLeftAlgorithm(this);
	UnDetectedArea = CUnDetectedArea::CreateArea(Info);
	//AlarmInfoProcess.UpgradeOneLevelAlarmTime = Info->OneLevelAlarmTime;
}

void CSickDataAlgorithm::GetRetFramePar(CScanDataParse* _current)
{
	RetFramePar.LMS_ID = _current->Device.DeviceNumber;
	RetFramePar.uiLmsCode = _current->Device.SerialNumber;
	strcpy(RetFramePar.chLmsName, _current->DeviceName.c_str());
	RetFramePar.dbStartAngle = Info->StartAngle;
	RetFramePar.dbEndAngle = Info->EndAngle;
	RetFramePar.dbResolution = Info->Resolution;
	RetFramePar.uiFrameNo =  _current->StatusInfo.ScanCounter;
	RetFramePar.deviceStatus = _current->Device.DeviceStatus;
	//eventPar.par = NULL;
}

void CSickDataAlgorithm::ReleaseCallMemory()
{
	if (EventInfo != NULL)
	{
		if (EventInfo->eventPar != NULL)
		{
			delete EventInfo->eventPar;
			EventInfo->eventPar = NULL;
		}
		delete EventInfo;
		EventInfo = NULL;
	}
}

void CSickDataAlgorithm::AlterTrainFlag(bool flag)
{
    if (_trainFlag != flag)
    {
    	//LogInfo("报警状态改变");
    	_trainFlag = flag;
    	if (_trainFlag)
    	{  //<取消所以报警
    		AlarmInfoProcess.IgnoreAllAlarm();
    	}
    	else
    	{  //<设置通过时间
    		struct timeval tv;
    		struct timezone tz;
    	    gettimeofday(&tv, &tz);
    	    _trainPassTime = tv.tv_sec / 60; //<转换为分钟
    	}
    }
}

void CSickDataAlgorithm::AlarmChange(std::string strName, CAlarmInfo *alarmInfo)
{
	RemoveRestoreAlarms();
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	alarmInfo->AlarmTime = tv.tv_sec;
	alarmInfo->AlarmTime_ms = tv.tv_usec;  //<10-6秒
	if (alarmInfo->blockPar->alarmStateType == Alarm)
    {
		//LogInfo(("Find alarm, name: " + strName).c_str());
		int nowMinute = tv.tv_sec / 60;
		if (nowMinute < _trainPassTime + 1)
		{
			for (std::map<CAlarmInfo*, int>::iterator iter = _restoreAlarms.begin(); iter != _restoreAlarms.end(); iter++)
			{
				CPOINT restoreAlarmPos(iter->first->blockPar->fAngle, iter->first->blockPar->fRadialLen);
				CPOINT curAlarmPos(alarmInfo->blockPar->fAngle, alarmInfo->blockPar->fRadialLen);
				if (CAlgorithmTool::GetInstance()->GetDistance(restoreAlarmPos, curAlarmPos) < 500.0)
				{
					CPolarArea curArea = alarmInfo->curArea;
					if (curArea.CheckValidFlag())
					{
					   curArea.Enlarge(2.0, 500.0);
					   AlarmInfoProcess.AddCurLazyArea(curArea);
					}
					return;
				}
			}
		}
		if (AlarmInfoProcess.CheckAbnormalPoint(alarmInfo->curArea))
		{
			//LogInfo("find alarm, name: %s in abnormal point", strName.c_str());
			return;
	    }
    }
    else if (alarmInfo->blockPar->alarmStateType == Resume)
    {
    	_restoreAlarms.insert(make_pair(alarmInfo, tv.tv_sec / 60));
    	alarmInfo->DeleteFlag = false;
    	//LogInfo("Remove alarm, name: %s", strName.c_str());
    }

    AlarmInfoProcess.AddAlarmInfo(strName, alarmInfo);
}

void CSickDataAlgorithm::RemoveRestoreAlarms()
{
	if (_restoreAlarms.empty())
	{
		return;
	}
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	int nowMinute = tv.tv_sec / 60;
	for (std::map<CAlarmInfo*, int>::iterator iter = _restoreAlarms.begin(); iter != _restoreAlarms.end();)
	{
        int minute = iter->second + 2;
		if (minute < nowMinute)
		{
			CAlarmInfo* tmp = iter->first;
			_restoreAlarms.erase(iter++);
			if (tmp != NULL)
			{
			   delete tmp;
			   tmp = NULL;
			}
		}
		else
		{
			iter++;
		}
	}
}






