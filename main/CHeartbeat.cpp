/*
 * CHeartbeat.cpp
 *
 *  Created on: May 3, 2016
 *      Author: yuan
 */

#include "CHeartbeat.h"
#include <unistd.h>
#include "../TCPClient/TCPClient.h"
#include "../TCPClient/PacketAssembly.h"
#include <liblogger/liblogger.h>
#include <fstream>
#include <iostream>
#include "CThreadLock.h"
#include <signal.h>
#include <sys/time.h>

using namespace std;

CThreadLock g_lockBuf;

char* CHeartbeat::_frame = NULL;
unsigned short CHeartbeat::_frameLen = 0;
PFHeartbeatCallback CHeartbeat::g_pfHeartbeatCallback = NULL;
void* CHeartbeat::g_retArg = NULL;

CHeartbeat::CHeartbeat() {
	// TODO Auto-generated constructor stub
	_bInitFlag = false;
	_frame = NULL;
}

CHeartbeat::~CHeartbeat() {
	// TODO Auto-generated destructor stub
	if (_frame != NULL)
	{
		delete[] _frame;
		_frame = NULL;
	}
}

bool CHeartbeat::Init(int version, int deviceID, void* par, PFHeartbeatCallback pf, void* arg)
{
	g_pfHeartbeatCallback = pf;
	g_retArg = arg;

	CHeartBeatFrame obHeartBeatFrame;
    obHeartBeatFrame.SetPar(par);
    obHeartBeatFrame.HeartBeat.baseInfo.version = version;
    obHeartBeatFrame.HeartBeat.baseInfo.deviceNo = deviceID;
    char* tmp = obHeartBeatFrame.GetPaketFrame(_frameLen);
    _frame = new char[_frameLen];
    memcpy(_frame, tmp,_frameLen);

    _bInitFlag = true;
    return true;
}

void CHeartbeat::UpdateStatus(int status)
{
     int startIndex = _frameLen - sizeof(S_StatusInfo) - 5;
     g_lockBuf.Lock();
     memcpy(_frame + startIndex, &status, sizeof(status));
     g_lockBuf.UnLock();
}

/*定时器处理函数*/

void CHeartbeat::Handler(void* par)
{
	LogTrace("Send Heartbeat info");
	if (g_pfHeartbeatCallback == NULL)
	{
		LogError("回调函数为空");
		return;
	}
	g_lockBuf.Lock();
	g_pfHeartbeatCallback(_frame, _frameLen, g_retArg);
    g_lockBuf.UnLock();

    return ;
}



