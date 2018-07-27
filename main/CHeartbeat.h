/*
 * CHeartbeat.h
 *
 *  Created on: May 3, 2016
 *      Author: yuan
 */

#ifndef CHEARTBEAT_H_
#define CHEARTBEAT_H_

typedef struct S_StatusInfo
{
    int status;
}S_StatusInfo, *LP_S_StatusInfo;

typedef void ( *PFHeartbeatCallback)(char* pData, int iDataLen, void* arg);

class CHeartbeat {
public:
	CHeartbeat();
	virtual ~CHeartbeat();
	bool Init(int version, int deviceID, void* par, PFHeartbeatCallback pf, void* arg);
	void UpdateStatus(int status);
	static void Handler(void* par);
	static char* _frame;
	static unsigned short _frameLen;
	static PFHeartbeatCallback g_pfHeartbeatCallback;
	static void* g_retArg;
private:
	bool _bInitFlag;
};

#endif /* CHEARTBEAT_H_ */
