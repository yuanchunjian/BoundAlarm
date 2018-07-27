/*
 * CThreadLock.cpp
 *
 *  Created on: May 6, 2016
 *      Author: yuan
 */

#include "CThreadLock.h"

CThreadLock::CThreadLock() {
	// TODO Auto-generated constructor stub
	pthread_mutex_init (&g_mutex_Msg,NULL);
}

CThreadLock::~CThreadLock() {
	// TODO Auto-generated destructor stub
	 pthread_mutex_destroy(&g_mutex_Msg);
}

void CThreadLock::Lock()
{
	pthread_mutex_lock (&g_mutex_Msg);
}

void CThreadLock::UnLock()
{
	pthread_mutex_unlock(&g_mutex_Msg);
}
