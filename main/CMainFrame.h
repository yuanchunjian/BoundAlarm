/*
 * CMainFrame.h
 *
 *  Created on: Jun 1, 2016
 *      Author: yuan
 */

#ifndef CMAINFRAME_H_
#define CMAINFRAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../TCPClient/TcpClientManager.h"
#include "../src/AlgorithrmControl.h"
#include "../src/CheckAlgorithm.h"
#include "../TCPClient/TCPClient.h"
#include "../src/AlgorithmService.h"
#include "../TCPClient/PacketAssembly.h"
#include "../TCPClient/ReceivePacketParse.h"
#include "CClientConfig.h"
#include "CSickProtocol.h"

#include "CLogConfig.h"
#include "CInitLog.h"
#include "CHeartbeat.h"

#include "CControlGPIO.h"
#include "CThreadLock.h"
#include "CTimerEvent.h"

#define LMS_TCP_CLIENT    0x00
#define EVENT_TCP_CLIENT  0x01
#define CONFIG_TCP_CLIENT 0x02

typedef struct LmsFrameStruct_tag
{
	unsigned int LmsDataIndex;
	unsigned char LmsHeadIndex;
	bool LmsFindHeadFlag;
	unsigned char LmsFrameLenIndex;
	unsigned char LmsFrameLen[4];
	unsigned int LmsDataLen;
	bool  LmsFramePrcing;
	unsigned char  Lms_crc;
	unsigned char  LmsBuf[4096];
	unsigned char  Lms_crc_Flag;
}S_LmsFrameStruct;

class CMainFrame {
public:
	CMainFrame();
	virtual ~CMainFrame();
    void SetExePath(char* arg);
    bool InitAllPar();
    bool CreateTcpClient();
    void Runing();
private:
    void InitTimerEvent();
    void InitLog();
	void InitFrameStruct();
	bool InitSendEventInfo();
	bool SendEventInfo(CPacketFrame *pPacket, void* par);
	void LogConfig();
	void SaveLmsRawData(char* pData, int iDataLen);
	/*
	 * *根据雷达报文，做相应处理，这个以后需要添加不同的处理命令
	 */
	void ProcessTelegramStream();

	void UpdateLmsDataArriveTime();
	void UnpackLmsData(char* pData, int iDataLen);
	static void OnDataReceive(char* pData, int iDataLen, void* arg);
	static void OnEventReceive(char* pData, int iDataLen, void* arg);
	static void OnRetFrameCallback(void* eventPar, void* retPar);
	static void OnEventInfoCallback(void* eventPar, void* retPar);
	static void OnDataParseCallback(unsigned char type, char* pData, int iDataLen, void* arg);
	static void OnHeartbeatCallback(char* pData, int iDataLen, void* arg);
	static void OnUpgradeMsgReceive(char* pData, int iDataLen, void* arg);
	static void OnUpgradeMsgCallback(unsigned char type, char* pData, int iDataLen, void* arg);
    static int AddFlagLmsDataThreadFunc(void*lparam);

	CThreadLock m_LmsDataArriveTimeLock;
	int m_LmsDataArriveTime;

	class CTCPClientManager	TCPClntManager;
	class CAlgorithrmControl m_obAlgorithrmControl;
	class CTCPClient* m_obLmsTCPClient;
	class CTCPClient* m_obEventTCPClient;
	class CTCPClient* m_obConfigTCPClient;
	CReceivePacketParse m_obReceiveParse;
	CReceivePacketParse m_obUpgradeMsgParse;

	bool m_bLmsScanInitFlag;
	bool m_bInitSendEventInfo;
	std::string m_strExePath;
	CClientConfig m_obClientConfig;
	CSickProtocol m_obSickProtocol;
	CInitLog m_initLog;
	CHeartbeat m_obHeartbeat;
	//CControlGPIO m_obControlGpio;
	CTimerEvent m_obTimerEvent;
	unsigned int m_iFrameCount;
	fstream m_saveDataFile;
	S_LmsFrameStruct m_lmsFrameStruct;
	bool m_bHeartInitFlag;

	std::string m_strFtpPath;
};

#endif /* CMAINFRAME_H_ */
