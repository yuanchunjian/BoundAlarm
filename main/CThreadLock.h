/*
 * CThreadLock.h
 *
 *  Created on: May 6, 2016
 *      Author: yuan
 */

#ifndef CTHREADLOCK_H_
#define CTHREADLOCK_H_
#include <pthread.h>

class CThreadLock {
public:
	CThreadLock();
	virtual ~CThreadLock();
	void Lock();
	void UnLock();
private:
	pthread_mutex_t g_mutex_Msg;
};

#endif /* CTHREADLOCK_H_ */
