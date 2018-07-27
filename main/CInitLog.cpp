/*
 * CInitLog.cpp
 *
 *  Created on: Apr 13, 2016
 *      Author: yuan
 */

#include "CInitLog.h"
#include <stdio.h>
#include <string.h>
#include "CLogConfig.h"

#include <liblogger/liblogger.h>
#include <liblogger/file_logger.h>
#include <liblogger/socket_logger.h>

tFileLoggerInitParams fileInitParams;
tSockLoggerInitParams sockInitParams;

CInitLog::CInitLog() {
	// TODO Auto-generated constructor stub
	fileInitParams.filePath = NULL;
	fileInitParams.fileName = NULL;
}

CInitLog::~CInitLog() {
	// TODO Auto-generated destructor stub
	if (fileInitParams.fileName != NULL)
	{
		delete[] fileInitParams.fileName;
		fileInitParams.fileName = NULL;
	}
	if (fileInitParams.filePath != NULL)
	{
		delete[] fileInitParams.filePath;
		fileInitParams.filePath = NULL;
	}
	DeInitLogger();
}

void CInitLog::InitLog()
{
	int iRet;
	switch(CLogConfig::GetInstance()->_outType)
	{
	case LogToFile:
	   {
		   memset(&fileInitParams,0,sizeof(tFileLoggerInitParams));
		   //std::string strName = CLogConfig::GetInstance()->_strPath + CLogConfig::GetInstance()->_strName;
		   fileInitParams.filePath = new char[CLogConfig::GetInstance()->_strPath.length() + 1];
		   strcpy(fileInitParams.filePath, CLogConfig::GetInstance()->_strPath.c_str());
		   fileInitParams.fileName = new char[CLogConfig::GetInstance()->_strName.length() + 1];
		   strcpy(fileInitParams.fileName, CLogConfig::GetInstance()->_strName.c_str());
		   fileInitParams.filemaxsize = CLogConfig::GetInstance()->_maxfilesize;
		   fileInitParams.fileOpenMode = tFileOpenMode(CLogConfig::GetInstance()->_openMode);
		   iRet = InitLogger(LogToFile,&fileInitParams, CLogConfig::GetInstance()->_logLevel);
	   }
	   break;
	case LogToSocket:
	   {
		   memset(&sockInitParams,0,sizeof(tSockLoggerInitParams));
		   sockInitParams.server 	= (char*)CLogConfig::GetInstance()->_socketIp.c_str();
		   sockInitParams.port		= CLogConfig::GetInstance()->_socketPort;
		   iRet =InitLogger(LogToSocket,&sockInitParams, CLogConfig::GetInstance()->_logLevel);
	   }
	   break;
	default:
		iRet = InitLogger(LogToConsole, stdout, CLogConfig::GetInstance()->_logLevel);
	   break;
	}

}
