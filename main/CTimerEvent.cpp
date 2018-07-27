/*
 * CTimerEvent.cpp
 *
 *  Created on: May 31, 2016
 *      Author: yuan
 */

#include "CTimerEvent.h"
#include <sys/time.h>
#include <signal.h>
#include "CThreadLock.h"
using namespace std;

std::map<int, S_EventPar> _eventMap;
CThreadLock g_obEventMapLock;

CTimerEvent::CTimerEvent() {
	// TODO Auto-generated constructor stub

}

CTimerEvent::~CTimerEvent() {
	// TODO Auto-generated destructor stub
}

void CTimerEvent::timer_handler(int msg)
{
	switch(msg)
	{
	case SIGALRM:
		EventLoop();
		break;
	default:
		break;
	}
}

bool CTimerEvent::SetEventTimer()
{
	/*设置SIGALRM信号的处理函数，即定时器处理函数*/
	if (signal(SIGALRM, timer_handler) == SIG_ERR)
	{
		return false;
	}

	struct itimerval nvalue;
	nvalue.it_value.tv_sec = 1;
	nvalue.it_value.tv_usec = 0;
	nvalue.it_interval.tv_sec = 1;//每隔sec时间触发一次定时器
	nvalue.it_interval.tv_usec = 0;
		/*此处使用的是ITIMER_REAL，所以对应的是SIGALRM信号*/
	if (setitimer(ITIMER_REAL, &nvalue, NULL) < 0)
	{
		return false;
	}
    return true;
}

void CTimerEvent::AddEvent(int eventType, int time_interval, PfEventHandle eventHandle, void* par)
{
	S_EventPar tmpPar;
	tmpPar.stopFlag = true;
	tmpPar.intervalNum = 0;
	tmpPar.timeInterval = time_interval;
	tmpPar.eventHandle = eventHandle;
	tmpPar.retPar = par;
	g_obEventMapLock.Lock();
	std::map<int, S_EventPar>::iterator iter = _eventMap.find(eventType);
	if (iter == _eventMap.end())
	{
		_eventMap.insert(make_pair(eventType, tmpPar));
	}
	else
	{
		_eventMap[eventType] = tmpPar;
	}
	g_obEventMapLock.UnLock();
}

void CTimerEvent::RemoveEvent(int eventType)
{
	g_obEventMapLock.Lock();
	std::map<int, S_EventPar>::iterator iter = _eventMap.find(eventType);
	if (iter != _eventMap.end())
	{
		_eventMap.erase(iter);
	}
	g_obEventMapLock.UnLock();
}

void CTimerEvent::StartEvent(int eventType)
{
	g_obEventMapLock.Lock();
	std::map<int, S_EventPar>::iterator iter = _eventMap.find(eventType);
	if (iter != _eventMap.end())
	{
		iter->second.stopFlag = false;
	}
	g_obEventMapLock.UnLock();
}

void CTimerEvent::StopEvent(int eventType)
{
	g_obEventMapLock.Lock();
	std::map<int, S_EventPar>::iterator iter = _eventMap.find(eventType);
	if (iter != _eventMap.end())
	{
	    iter->second.stopFlag = true;
	    iter->second.intervalNum = 0;
	}
	g_obEventMapLock.UnLock();
}

void CTimerEvent::EventLoop()
{
	g_obEventMapLock.Lock();
	if (!_eventMap.empty())
	{
		std::map<int, S_EventPar>::iterator iter = _eventMap.begin();
		for (; iter != _eventMap.end(); iter++) {
			if (iter->second.stopFlag)
			{
				continue;
			}
			iter->second.intervalNum++;
			if (iter->second.intervalNum >= iter->second.timeInterval) {
				iter->second.intervalNum = 0;
				iter->second.eventHandle(iter->second.retPar);
			}
		}
	}
	g_obEventMapLock.UnLock();
}

