/*
 * CTimerEvent.h
 *
 *  Created on: May 31, 2016
 *      Author: yuan
 */

#ifndef CTIMEREVENT_H_
#define CTIMEREVENT_H_
#include <map>

typedef void (*PfEventHandle) (void*);
typedef enum
{
	Heartbeat,
	UpgradeAlarm,
	RemoveAlarmRecord,
	WatchDogEvent
}TimerEventType;

typedef struct S_EventPar
{
	bool stopFlag;
	int intervalNum;
	int timeInterval;
	PfEventHandle eventHandle;
	void* retPar;
}S_EventPar, *LP_S_EventPar;

class CTimerEvent {
public:
	CTimerEvent();
	virtual ~CTimerEvent();
	bool SetEventTimer();

	void AddEvent(int eventType, int time_interval, PfEventHandle eventHandle, void* par);
	void RemoveEvent(int eventType);
	void StartEvent(int eventType);
	void StopEvent(int eventType);
private:
    static void EventLoop();
    static void timer_handler(int msg);
};

#endif /* CTIMEREVENT_H_ */
