/*
 * CMainFrame.cpp
 *
 *  Created on: Jun 1, 2016
 *      Author: yuan
 */

#include "CMainFrame.h"
#include <liblogger/liblogger.h>
#include <sys/time.h>
#include "../src/CUpgradeAlarm.h"
#include "../src/CAlarmRecordService.h"
#include "../src/PublicHead.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "ReadINI.h"
#include "uart.h"

#include<sys/select.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "udp.h"
using namespace std;
extern char info_rec[128];
char ContinuousScans[] = {0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x11,0x73,0x45,0x4E,0x20,0x4C,0x4D,0x44,0x73,0x63,0x61,0x6E,0x64,0x61,0x74,0x61,0x20,0x01,0x33};
char ConfigInit[] = {0x02, 0x02, 0x02, 0x02, (char)0xFF, 0X01, 0X00, 0X00, 0X00, 0X01, 0X03, 0X03, 0X03, 0X03, 0X00};

char StopSound[] = {0x01, 0x51, 0x00, 0x00, 0x1c, 0x4c, 0x02};
char StartSound[] = {0x01, 0x51, 0x01, 0x00, 0x1c, 0x4d, 0x02};
int fd=-1;
CMainFrame::CMainFrame() {
	// TODO Auto-generated constructor stub
	m_iFrameCount = 0;
	m_bInitSendEventInfo = false;
	m_bLmsScanInitFlag = false;
	m_obEventTCPClient = NULL;
	m_obLmsTCPClient = NULL;
	m_LmsDataArriveTime = 0;
	m_bHeartInitFlag = false;
}

CMainFrame::~CMainFrame() {
	// TODO Auto-generated destructor stub
}

void CMainFrame::SetExePath(char* arg)
{
	m_strExePath = arg;
	int iPos = m_strExePath.rfind('/');
	if (iPos != string::npos)
	{
	    m_strExePath = m_strExePath.substr(0, iPos);
	}
}

bool CMainFrame::InitAllPar()
{
	InitLog();
	LogConfig();

	//LogDebug("Test log debug");"/dev/ttyO4"


	fd = open(m_obClientConfig.UartName.c_str(), O_RDWR);
	if (fd < 0)
	{
			printf("open device %s faild\n", m_obClientConfig.UartName.c_str());
			return false;
	}
	else
	{
		printf("open device %s suc\n", m_obClientConfig.UartName.c_str());
	}

	set_speed(fd,9600); //���ô��ڲ�����

	set_Parity(fd,8,1,'N');


	//m_obControlGpio.Open(1);
	LogInfo("Init Turn off sound");
	//01 51 00 00 1c 4c 02 stop
	//01 51 01 00 1c 4d 02  start
	write(fd, StopSound, 7);
	//m_obControlGpio.TurnOff();

	LogInfo("Init algorithrm stop");
	m_obAlgorithrmControl.SetAlgorithmRun(false, NET_STATUS);

	InitFrameStruct();
	m_obReceiveParse.SetPar(OnDataParseCallback, this);
	m_obReceiveParse.InitFrame();

	m_obUpgradeMsgParse.SetPar(OnUpgradeMsgCallback, this);
	m_obUpgradeMsgParse.InitFrame();

	int iRet = m_obAlgorithrmControl.InitControl(OnRetFrameCallback, OnEventInfoCallback, m_strExePath.c_str(),
			      m_obClientConfig.AlarmRecordDbPath.c_str(), this);
	if (iRet < 0)
	{
		if (iRet == CONFIG_ERROR)
		{
            exit(ALGORITHM_CONFIG_ERR_EXIT);
		}
		else
		{
			exit(ABNORMAL_EXIT);
		}
		return false;
	}

	if (m_obClientConfig.Heartbeat.bTurnOn)
	{
		CAlgorithmInfo* info = CAlgorithmService::GetInstance()->GetAlgorithmInfo(m_obClientConfig.SerialNum);
		if (info == NULL)
		{
			m_bHeartInitFlag = false;
			LogError("对应的sick序号没找到算法配置文档，算法停止.");
			exit(ALGORITHM_CONFIG_ERR_EXIT);
		}
		else
		{
		   m_bHeartInitFlag = m_obHeartbeat.Init(m_obClientConfig.VersionNum, m_obClientConfig.DeviceID, info, OnHeartbeatCallback, this);
		}
	}

	InitTimerEvent();

	char localPath[100];			// /home/yuan/Ftp/
	char buf[MAX_PATH];
	memset(buf,0,sizeof(buf));
	GetCurrentPath(buf,CONF_FILE_PATH);

	strcpy( localPath, GetIniKeyString("FtpServer","FilePath", buf) );
	m_strFtpPath = localPath;
	if (m_strFtpPath.size() > 0 && m_strFtpPath.at(m_strFtpPath.length() - 1) != '/')
	{
		m_strFtpPath += "/";
	}

    return true;
}

bool CMainFrame::CreateTcpClient()
{
	char   ServerIP[20];

	strcpy( ServerIP, m_obClientConfig.Channels.strIP.c_str());

	//添加一个TCP客户端连接对象
	//<连接雷达
	TCPClntManager.AddTCPClientObject( ServerIP, m_obClientConfig.Channels.port, OnDataReceive, this );
	m_obLmsTCPClient = (CTCPClient*)TCPClntManager.m_TCPClientInfo[LMS_TCP_CLIENT].pTCPClient;
   //客户端
	strcpy( ServerIP, m_obClientConfig.DataServer.strIP.c_str() );
	TCPClntManager.AddTCPClientObject( ServerIP, m_obClientConfig.DataServer.port, OnEventReceive, this);
	m_obEventTCPClient = (CTCPClient*)TCPClntManager.m_TCPClientInfo[EVENT_TCP_CLIENT].pTCPClient;
    //升级程序通讯
	//strcpy( ServerIP, "127.0.0.1" );
	//TCPClntManager.AddTCPClientObject( ServerIP, m_obClientConfig.ConfigureServer.port, OnUpgradeMsgReceive, this);
	//m_obConfigTCPClient = (CTCPClient*)TCPClntManager.m_TCPClientInfo[CONFIG_TCP_CLIENT].pTCPClient;
	//启动TCPClient连接管理操作，并创建TCPClient连接管理线程
	TCPClntManager.Start();

	return true;
}

void CMainFrame::Runing()
{
	int i1, i2, i3;
	bool bStartHeartbeat = false;
	bool bAlgorithrmStopFlag = true;
	bool bConfigSend = false;

	m_obTimerEvent.SetEventTimer();
	for( i1=0; i1<TCPClntManager.m_nTCPClientNum; i1++ )
	{
		//设置TCP客户端连接对象为csINIT初始状态，连接管理线程将自动进行TCP的连接操作
		TCPClntManager.Open(i1);
	}

	for( ; ; )
	{
		sleep(1);

		for( i2=0; i2<TCPClntManager.m_nTCPClientNum; i2++ )
		{
			// 查询TCP客户端连接状态
			i3 = TCPClntManager.GetTCPClientState(i2);
			if ( i3 == csRuning )
			{
	             if ( i2 == EVENT_TCP_CLIENT)
	             {
	                	if (!m_bInitSendEventInfo)
	                	{
	                	   InitSendEventInfo();
	                	   m_bInitSendEventInfo = true;
	                	}
	                	if (!bStartHeartbeat && m_bHeartInitFlag)
	                	{
	                		m_obTimerEvent.StartEvent(Heartbeat);
	                		bStartHeartbeat = true;
	                	}
	                	if (bAlgorithrmStopFlag)
	                	{
	                		bAlgorithrmStopFlag = false;

	                		m_obAlgorithrmControl.SetAlgorithmRun(true, NET_STATUS);
	                	}
	              }

				if ( i2 == LMS_TCP_CLIENT)
				{

						if (!m_bLmsScanInitFlag)
						{
							// 连续扫描指令 发送数据
							int len = m_obLmsTCPClient->SendData(ContinuousScans, sizeof(ContinuousScans));
							if (len < 0)
							{
								LogError("SendData scan init failed!");
							}
							else
							{
								m_bLmsScanInitFlag = true;
							}
						}
						m_LmsDataArriveTimeLock.Lock();
						//<雷达数据超时处理
						if (m_LmsDataArriveTime > 0)
						{
						    struct timeval tv;
						    struct timezone tz;
						    if (gettimeofday(&tv, &tz) == 0) {
							if (tv.tv_sec - m_LmsDataArriveTime > m_obClientConfig.SickTimeout) {
								m_obLmsTCPClient->Close();
								m_LmsDataArriveTime = 0;
								LogError("超时%d秒没接送到雷达数据，断开链接，重新链接", m_obClientConfig.SickTimeout);
							}
						}
						}
						m_LmsDataArriveTimeLock.UnLock();
				}
				if (i2 == CONFIG_TCP_CLIENT)
				{
                     if (!bConfigSend)
                     {
                    	 m_obConfigTCPClient->SendData(ConfigInit, sizeof(ConfigInit));
                    	 bConfigSend = true;
                     }
				}
			}

			if( i3 == csWAIT )
			{
					//设置TCP客户端连接对象为csINIT初始状态，连接管理线程将自动进行TCP的连接操作
				LogError( "TCP Connect%d State: csWAIT, ", i2+1 );
				if (i2 == EVENT_TCP_CLIENT)
				{
					if (!bAlgorithrmStopFlag)
					{
						bAlgorithrmStopFlag = true;
						LogInfo("Turn off sound");
					   // m_obControlGpio.TurnOff();

						m_obAlgorithrmControl.SetAlgorithmRun(false, NET_STATUS);
					 }
					 if (bStartHeartbeat && m_bHeartInitFlag)
					 {
						  m_obTimerEvent.StopEvent(Heartbeat);
						  bStartHeartbeat = false;
						  m_bInitSendEventInfo = false;
					}
				}

				int iRet = TCPClntManager.Open( i2 );
				if (iRet == -1)
				{
					LogError( "TCP open fail, %d", i2 );
				}
				else
				{
					if (i2 == LMS_TCP_CLIENT)
					{
						m_bLmsScanInitFlag = false;
					}
					else if ( i2 == CONFIG_TCP_CLIENT)
					{
						bConfigSend = false;
					}
				}
			}
		}
	}
	//m_obControlGpio.Close();
	close(fd);
}

void CMainFrame::InitTimerEvent()
{
	if (m_bHeartInitFlag)
	{
		m_obTimerEvent.AddEvent(Heartbeat, m_obClientConfig.Heartbeat.intervalSec, CHeartbeat::Handler, NULL);
	}

	m_obTimerEvent.AddEvent(UpgradeAlarm, 1, CUpgradeAlarm::handler, NULL);
	m_obTimerEvent.StartEvent(UpgradeAlarm);

	//m_obTimerEvent.AddEvent(RemoveAlarmRecord, 86400, CAlarmRecordService::OnRemoveExtendedRecord, NULL);
	//m_obTimerEvent.StartEvent(RemoveAlarmRecord);

}

void CMainFrame::InitLog()
{
	CLogConfig::GetInstance()->LoadConfig(m_strExePath.c_str());
	m_initLog.InitLog();
}

void CMainFrame::LogConfig()
{
	m_obClientConfig.LoadConfig(m_strExePath.c_str());
	m_obSickProtocol.LoadConfig(m_strExePath.c_str(), m_obClientConfig.CfgFileName.c_str());
}

bool CMainFrame::SendEventInfo(CPacketFrame *pPacket, void* par)
{
	if (pPacket == NULL || par == NULL)
	{
		LogError("参数为空，失败.");
		return false;
	}
	pPacket->SetPar(par);
	unsigned short len;
	char* frame = pPacket->GetPaketFrame(len);

#if 1
	//test send data
/*
 * for(int i=0;i<len;i++)
 *	{
 *		printf("%02x ",frame[i]);
 *	}
 *	printf("\n");
*/
	int iRet = m_obEventTCPClient->SendData(frame, len);
	if (iRet < 0)
	{
		LogError("Send event data fail!");
	}
#endif
	delete pPacket;
	pPacket = NULL;
	return true;
}

bool CMainFrame::InitSendEventInfo()
{
	CAlgorithmInfo* info = CAlgorithmService::GetInstance()->GetAlgorithmInfo(m_obClientConfig.SerialNum);
    if (info != NULL)
    {
    	LogTrace("发送基本信息");
    	CBaseInfoFrame* baseInfo = new CBaseInfoFrame;
    	baseInfo->BaseInfo.version = m_obClientConfig.VersionNum;
    	baseInfo->BaseInfo.deviceNo = m_obClientConfig.DeviceID;
    	SendEventInfo(baseInfo , info);
    	LogTrace("发送算法参数");
    	SendEventInfo(new CAlgorithmPar, info);
    	LogTrace("发送保护区信息");
    	SendEventInfo(new CProtectionArea, info);
    	LogTrace("发送不检测区信息");
    	SendEventInfo(new CNonDetectedArea, info);
    }
	return true;
}

void CMainFrame::InitFrameStruct()
{
	m_lmsFrameStruct.LmsFrameLenIndex = 0;
	m_lmsFrameStruct.LmsHeadIndex = 0;
	m_lmsFrameStruct.LmsDataIndex = 0;
	m_lmsFrameStruct.LmsFramePrcing = false;
	m_lmsFrameStruct.LmsFindHeadFlag = false;
}

/*
 * 这个函数现在不用
 */
int CMainFrame::AddFlagLmsDataThreadFunc(void* lparam)
{
	CMainFrame* pMainFrm = (CMainFrame*)lparam;
	time_t timep;
	time(&timep);
	struct tm *p;
	int i = 1;
	while (1)
	{
	    time_t fileSec = timep - 86400 * i;
		p = localtime(&fileSec);
		char chFloder1[20] = { 0 };
		sprintf(chFloder1, "%4d%02d%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
		string strPath = /*pMainFrm->m_strFtpPath*/pMainFrm->m_obClientConfig.SaveRawData.strPath + "ScanData/" + chFloder1;
		if (access(strPath.c_str(), F_OK) == 0) {
			//string strCmd = "tar -czvf " + strTarPath + ".tar " + strTarPath;
			//system(strCmd.c_str());
			if (rename(strPath.c_str(), (strPath + "_s").c_str()) != 0)
			{
				LogError("rename %s fail.", strPath.c_str());
			}
			i++;
		} else {
			break;
		}
	}
	pthread_exit( NULL );
	return 0;
}

void CMainFrame::SaveLmsRawData(char* pData, int iDataLen)
{
    if (m_iFrameCount == 0)
    {
      time_t timep;
      time(&timep);
      struct tm *p;
	  p = localtime(&timep);

	  char chFloder[20] = {0};
	  sprintf(chFloder, "%4d%02d%02d", p->tm_year + 1900, p->tm_mon+1, p->tm_mday);
	  string strPath = /*m_strFtpPath + "ScanData/"*/m_obClientConfig.SaveRawData.strPath + string(chFloder);
      if (access(strPath.c_str(), F_OK) != 0)
      {
    	 /* pthread_attr_t attr;
    	  int res = pthread_attr_init(&attr);
    	  if( res!=0 )
    	  {
    	  	  printf("Create attribute failed\n" );
    	  }
    	  res = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
    	  	//
    	  res += pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    	  if( res!=0 )
    	  {
    	  	printf( "Setting attribute failed\n" );
    	  }
    	 pthread_t thread;
    	  res = pthread_create( &thread, &attr, (void *(*) (void *))&AddFlagLmsDataThreadFunc, (void*)this );
    	  if( res == 0 )
    	  {
    		  pthread_attr_destroy( &attr );
    	  }*/

		  if ( mkdir(strPath.c_str(), S_IRWXU) != 0)
			{
			     LogError("创建文件夹：%s失败.", strPath.c_str());
			     return;
			}
      }

	  char time[20] = {0};
	  sprintf(time, "%4d%02d%02d%02d%02d%02d", p->tm_year + 1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

	  string strFilePath = strPath + "/" + string(time) +  string(".dat");

	  std::locale loc = std::locale::global(std::locale(""));
	  m_saveDataFile.open(strFilePath.c_str(), ios::binary|ios::out);
	  if (!m_saveDataFile)
	  {
	      LogError("Open file:%s fail!", strFilePath.c_str());
	  }
	  std::locale::global( loc );
    }
    if (m_saveDataFile) {
		m_saveDataFile.write(pData, iDataLen);
		m_saveDataFile.flush();
		m_iFrameCount++;
		if (m_iFrameCount >= m_obClientConfig.SaveRawData.saveTime * 1500/*60 * 25*/) {
			m_iFrameCount = 0;
			m_saveDataFile.close();
		}
	}
}

void CMainFrame::UpdateLmsDataArriveTime()
{
	m_LmsDataArriveTimeLock.Lock();
	struct timeval tv;
	struct timezone tz;
	if (gettimeofday(&tv, &tz) == 0)
	   m_LmsDataArriveTime = tv.tv_sec;
	m_LmsDataArriveTimeLock.UnLock();
}

void CMainFrame::UnpackLmsData(char* pData, int iDataLen) {
	for (int index = 0; index < iDataLen;) {
		if (m_lmsFrameStruct.LmsFramePrcing) {
			//<
			if (m_lmsFrameStruct.LmsDataIndex > 0
					&& (m_lmsFrameStruct.LmsDataIndex + iDataLen >= m_lmsFrameStruct.LmsDataLen + 1)) {//»
				memcpy(
						m_lmsFrameStruct.LmsBuf + m_lmsFrameStruct.LmsDataIndex,
						pData,
						m_lmsFrameStruct.LmsDataLen - m_lmsFrameStruct.LmsDataIndex);
				ProcessTelegramStream();
				index += (m_lmsFrameStruct.LmsDataLen - m_lmsFrameStruct.LmsDataIndex + 1);
				InitFrameStruct();
				continue;
			} else if (iDataLen >= (m_lmsFrameStruct.LmsDataLen + index + 1)) {
				memcpy(m_lmsFrameStruct.LmsBuf, pData + index,
						m_lmsFrameStruct.LmsDataLen);
				ProcessTelegramStream();
				index += (m_lmsFrameStruct.LmsDataLen + 1);
				InitFrameStruct();
				continue;
			} else if (iDataLen > index) {
				//
				memcpy(m_lmsFrameStruct.LmsBuf + m_lmsFrameStruct.LmsDataIndex,
						pData + index, iDataLen - index);
				m_lmsFrameStruct.LmsDataIndex += (iDataLen - index);
				break;
			} else {
				break;
			}

		}

		if (m_lmsFrameStruct.LmsFindHeadFlag
				&& !m_lmsFrameStruct.LmsFramePrcing) {
			m_lmsFrameStruct.LmsFrameLen[m_lmsFrameStruct.LmsFrameLenIndex] = pData[index++];
			m_lmsFrameStruct.LmsFrameLenIndex++;
			if (m_lmsFrameStruct.LmsFrameLenIndex == 4) {
				m_lmsFrameStruct.LmsFramePrcing = true;
				//
#if 1
				m_lmsFrameStruct.LmsDataLen = m_lmsFrameStruct.LmsFrameLen[0] << 24;
				m_lmsFrameStruct.LmsDataLen += m_lmsFrameStruct.LmsFrameLen[1] << 16;
				m_lmsFrameStruct.LmsDataLen += m_lmsFrameStruct.LmsFrameLen[2] << 8;
				m_lmsFrameStruct.LmsDataLen += m_lmsFrameStruct.LmsFrameLen[3];
#else
				m_lmsFrameStruct.LmsDataLen = m_lmsFrameStruct.LmsFrameLen[3] << 24;
				m_lmsFrameStruct.LmsDataLen += m_lmsFrameStruct.LmsFrameLen[2] << 16;
				m_lmsFrameStruct.LmsDataLen += m_lmsFrameStruct.LmsFrameLen[1] << 8;
			    m_lmsFrameStruct.LmsDataLen += m_lmsFrameStruct.LmsFrameLen[0];
#endif
			}
		}

		if (!m_lmsFrameStruct.LmsFindHeadFlag) {
			if (pData[index++] == 0x02) {
				m_lmsFrameStruct.LmsHeadIndex++;
				if (m_lmsFrameStruct.LmsHeadIndex == 4) {
					m_lmsFrameStruct.LmsFindHeadFlag = true;
					m_lmsFrameStruct.LmsHeadIndex = 0;
				}
			} else {
				m_lmsFrameStruct.LmsHeadIndex = 0;
			}
		}
	}
}
//《雷达数据接送回调
void CMainFrame::OnDataReceive(char* pData, int iDataLen, void* arg)
{
	CMainFrame* pMainFrm = (CMainFrame*)arg;
	//<
	pMainFrm->UpdateLmsDataArriveTime();

	if (pMainFrm->m_obClientConfig.SaveRawData.isSave)
	{
		pMainFrm->SaveLmsRawData(pData, iDataLen);
	}
	pMainFrm->UnpackLmsData(pData, iDataLen);
}

void CMainFrame::ProcessTelegramStream()
{
	if (m_lmsFrameStruct.LmsDataLen > 1000)
	{
		m_obAlgorithrmControl.Control(m_lmsFrameStruct.LmsBuf + 0x10, m_lmsFrameStruct.LmsDataLen - 0x10);
	}
}

void CMainFrame::OnRetFrameCallback(void* eventPar, void* retPar)
{
	CMainFrame* pMainFrm = (CMainFrame*)retPar;

	S_RetFramePar* retEventPar = (S_RetFramePar*)eventPar;
	LogTrace("StartAngle: %.2f EndAngle: %.2f   Resolution: %.2f  LmsCode: %d  FrameNo: %d DeviceStatus: %d" ,
			retEventPar->dbStartAngle, retEventPar->dbEndAngle, retEventPar->dbResolution,
			retEventPar->uiLmsCode, retEventPar->uiFrameNo, retEventPar->deviceStatus);
	//<更新心跳中设备状态
  /*  printf("StartAngle: %.2f EndAngle: %.2f   Resolution: %.2f  LmsCode: %d  FrameNo: %d DeviceStatus: %d\n" ,
			retEventPar->dbStartAngle, retEventPar->dbEndAngle, retEventPar->dbResolution,
			retEventPar->uiLmsCode, retEventPar->uiFrameNo, retEventPar->deviceStatus);*/
	if (pMainFrm->m_bHeartInitFlag)
	{
	    pMainFrm->m_obHeartbeat.UpdateStatus((int)retEventPar->deviceStatus);
	}
	LogTrace("UpdateStatus finish");
}
void CMainFrame::OnEventInfoCallback(void* eventPar, void* retPar)
{
	CMainFrame* pMainFrm = (CMainFrame*)retPar;
	//<发送数据
	pMainFrm->SendEventInfo(new CEventFrame(), eventPar);
    //<解析记录日志
	S_EventInfo* retEventPar = (S_EventInfo*) eventPar;
	string strName;
	switch (retEventPar->blockNameID) {
	case COMMON_BLOCK:
		strName = "Block";
		break;
	case PERSON:
		strName = "Person";
		break;
	case LANDSLIP:
		strName = "Landslip";
		break;
	case TRAIN:
		strName = "Train";
		break;
	default:
		strName = "CrowdBlock";
		break;
	}
	char strHint[512];
	if (retEventPar->eventTypeID == FIND_BLOCK_ALEAR) {
		S_BlockEventPar* par = (S_BlockEventPar*) retEventPar->eventPar;
		if (par->alarmStateType == Alarm)
		{
		if (par->eventType == ONE_LEVEL_ALARM) {
			sprintf(
					strHint,
					"%s%05d one level alarm, position: %.2f, %.2f, length: %.2f",
					strName.c_str(), par->ID, par->fAngle, par->fRadialLen,
					par->fBlockLen);
		} else if (par->eventType == TWO_LEVEL_ALARM) {
			sprintf(strHint,
					"Find %s%05d alarm, position: %.2f, %.2f, length: %.2f",
					strName.c_str(), par->ID, par->fAngle, par->fRadialLen,
					par->fBlockLen);
		}
		}
		else if (par->alarmStateType == Resume) {
			sprintf(strHint, "%s%05d alarm remove, Remain time: %.2fms",
					strName.c_str(), par->ID, par->fRemainTime);
			//LogInfo("Turn off sound");
			//write(fd, StopSound, 7);
		}
		else if (par->alarmStateType == Cancel) {
					sprintf(strHint, "%s%05d alarm Cancel, Remain time: %.2fms",
							strName.c_str(), par->ID, par->fRemainTime);
				}
	} else if (retEventPar->eventTypeID == PASS_PROTECTION_AREA) {
		S_PassProtectionAreaPar* par =
				(S_PassProtectionAreaPar*) retEventPar->eventPar;
		if (par->eventType == ENTER_PROTECTION_AREA) {
			if (retEventPar->blockNameID == TRAIN) {
				string strDirect;
				if (par->unionDirection.ucDirection == TRAIN_LEFT) {
					strDirect = "up";
				} else {
					strDirect = "down";
				}
				sprintf(
						strHint,
						"Find %s%05d enter protection area, speed: %.2f km/h, direction: %s",
						strName.c_str(), par->ID, par->fSpeed,
						strDirect.c_str());
			} else {
				sprintf(
						strHint,
						"Find %s%05d enter protection area, speed: %.2f, direction: %.2f, position: %.2f, %.2f",
						strName.c_str(), par->ID, par->fSpeed,
						par->unionDirection.fDirection, par->fAngle,
						par->fRadialLen);
			}
		} else if (par->eventType == LEAVE_PROTECTION_AREA) {
			sprintf(
					strHint,
					"%s%05d leave protection area, enter frame NO.:%d, leave frame NO.: %d, Remain time: %.2fms",
					strName.c_str(), par->ID, par->enterFrameNO,
					par->leaveFrameNO, par->fRemainTime);
		}
	}
	std::cout << strHint << endl;
	LogInfo(strHint);
}

void CMainFrame::OnEventReceive(char* pData, int iDataLen, void* arg)
{
	CMainFrame* pMainFrm = (CMainFrame*)arg;
	pMainFrm->m_obReceiveParse.ParseData(pData, iDataLen);
}

void CMainFrame::OnDataParseCallback(unsigned char type, char* pData, int iDataLen, void* arg)
{
	CMainFrame* pMainFrm = (CMainFrame*)arg;
	switch(type)
	{
	case ALARM_HANDLE:
	{
		S_AlarmHandle alarmHandle;
		memcpy(&alarmHandle, pData, sizeof(S_AlarmHandle));
		switch(alarmHandle.type)
       {
       case Ignore:
    	   LogDebug("AlarmIgnore, ID: %d", alarmHandle.alarmID);
    	   pMainFrm->m_obAlgorithrmControl.AlarmIgnoreProc(alarmHandle.alarmID);
    	   break;
       case Confirm:
    	   LogDebug("AlarmConfirm, ID: %d", alarmHandle.alarmID);
    	   pMainFrm->m_obAlgorithrmControl.AlarmConfirmProc(alarmHandle.alarmID);
    	   break;
       case IgnoreAll:
    	   LogDebug("AllAlarmIgnore");
    	   pMainFrm->m_obAlgorithrmControl.AllAlarmIgnoreProc();
    	   break;
       default:
    	   LogInfo("Alarm handle type is not supported");
       		break;
       }
	}
	break;
	case CLEAR_ABNOMAL_POINT:
		LogInfo("ClearAbnormalPoints");
		pMainFrm->m_obAlgorithrmControl.ClearAbnormalPointsProc();
		break;
	case CONTROL_SOUND:
		if (pData == NULL)
		{
		   return;
		}
		else if (pData[0] == TurnOff)
		{
			LogInfo("Turn off sound");
			//pMainFrm->m_obControlGpio.TurnOff();
			write(fd, StopSound, 7);
		}
		else if (pData[0] == TurnOn)
		{
			LogInfo("Turn on sound");
			//pMainFrm->m_obControlGpio.TurnOn();

			/*Debugging,please open when you need*/
			write(fd, StartSound, 7);

		}
		break;
	case CONTROL_KEY:
	   {
		   LogInfo("Key status switch");
		   if (pData == NULL)
		   {
		       return;
		   }
		   else if (pData[0] == TurnOn)
		   {
               LogInfo("Turn on key, close alarm");
               LogInfo("Turn off sound");
              // pMainFrm->m_obControlGpio.TurnOff();
               write(fd, StopSound, 7);
               pMainFrm->m_obAlgorithrmControl.SetAlgorithmRun(false, KEY_STATUS);
		   }
		   else if (pData[0] == TurnOff)
		   {
			   LogInfo("Turn off key, open alarm");
			   pMainFrm->m_obAlgorithrmControl.SetAlgorithmRun(true, KEY_STATUS);
		   }
	   }
	   break;
	case 0xFB:
	{
		LogInfo("Resend all alarm");
#if 0
		if (!pMainFrm->m_stopAlarmStatus)
		{
			pMainFrm->m_obAlgorithrmControl.ResendAllAlarm();
		}
#endif
	}
	  break;
	default:
		LogInfo("Type is not supported");
		break;
	}
}

void CMainFrame::OnHeartbeatCallback(char* pData, int iDataLen, void* arg)
{
	CMainFrame* pMainFrm = (CMainFrame*)arg;
    int iLen = pMainFrm->m_obEventTCPClient->SendData(pData, iDataLen);
    if (iLen < 0)
    {
    	LogError("Send heartbeat data fail");
    }
}

void CMainFrame::OnUpgradeMsgReceive(char* pData, int iDataLen, void* arg)
{
	CMainFrame* pMainFrm = (CMainFrame*)arg;
	pMainFrm->m_obUpgradeMsgParse.ParseData(pData, iDataLen);
}

void CMainFrame::OnUpgradeMsgCallback(unsigned char type, char* pData, int iDataLen, void* arg)
{
	CMainFrame* pMainFrm = (CMainFrame*)arg;
    switch(type)
    {
    case UPDATE_CONFIG:
    case SORTWARE_UPGRADE:
    	LogInfo("收到升级或更新配置消息");
    	pMainFrm->m_obAlgorithrmControl.UpdateMsgProc(type);
    	break;
    default:
    	break;
    }
}
