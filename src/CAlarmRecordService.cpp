/*
 * CAlarmRecordService.cpp
 *
 *  Created on: May 12, 2016
 *      Author: yuan
 */

#include "CAlarmRecordService.h"
#include <liblogger/liblogger.h>
#include <string>
#include <sys/time.h>

#include <iostream>
#include <sstream>

bool CAlarmRecordService::_CreateSucFlag = false;
sqlite3* CAlarmRecordService::_recordDB = NULL;
std::string CAlarmRecordService::_dbName = "AlarmRecord.db";

CAlarmRecordService::CAlarmRecordService() {
	// TODO Auto-generated constructor stub
	_dbPath = "";
}

CAlarmRecordService::~CAlarmRecordService() {
	// TODO Auto-generated destructor stub
}

void CAlarmRecordService::SetDbPath(const char* path)
{
	_dbPath = path;
}

bool CAlarmRecordService::Create()
{
	_CreateSucFlag = true;
	_dbName = _dbPath + _dbName;
	 int nResult = sqlite3_open(_dbName.c_str(), &_recordDB);
	 if (nResult != SQLITE_OK)
	 {
	     LogError("打开数据库失败：%d.", sqlite3_errmsg(_recordDB));
	     _CreateSucFlag = false;
	     return false;
	 }
	 else
	 {
	     LogDebug("数据库打开成功.");
	 }

	 std::string strTable = "create table if not exists AlarmInfo ([id]  INTEGER primary key autoincrement,"; //<[id]  INTEGER primary key autoincrement,
	 strTable += "[alarmTime] INTEGER,";
	 strTable += "[hasSent] INTEGER,";
	 strTable += "[curArea_A1] DOUBLE,";
	 strTable += "[curArea_A2] DOUBLE,";
	 strTable += "[curArea_R1] DOUBLE,";
	 strTable += "[curArea_R2] DOUBLE,";
	 strTable += "[nameID] INTEGER,";
	 strTable += "[eventType] INTEGER,";
	 strTable += "[alarmLevel] INTEGER,";
	 strTable += "[alarmStateType] INTEGER,";
	 strTable += "[abnormalPoint] INTEGER,";
	 strTable += "[angle] FLOAT,";
	 strTable += "[radialLen] FLOAT,";
	 strTable += "[blockLen] FLOAT,";
	 strTable += "[remainTime] FLOAT,";
	 strTable += "[frameNO] INTEGER";
	 strTable += ",[CreatedTime] TimeStamp NOT NULL DEFAULT (datetime('now','localtime'))";
	 strTable += ")";

	 char* errmsg;

	 nResult = sqlite3_exec(_recordDB, strTable.c_str(), NULL, NULL, &errmsg);
	 if (nResult != SQLITE_OK)
	 {
	      sqlite3_close(_recordDB);
	      LogError(errmsg);
	      sqlite3_free(errmsg);
	      _CreateSucFlag = false;
	      return false;
	  }
	 sqlite3_close(_recordDB);
     return true;
}

int CAlarmRecordService::AddRecord(CAlarmInfo* info)
{
	if (!_CreateSucFlag)
	{
		return -1;
	}

	int nResult = sqlite3_open_v2( _dbName.c_str(), &_recordDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
	if (nResult != SQLITE_OK)
	{
		  LogError("打开数据库失败：%d.", sqlite3_errmsg(_recordDB));
		  return -1;
    }

	ostringstream ostrSql("");
	ostrSql << "INSERT INTO AlarmInfo ([id], [alarmTime], [hasSent], [curArea_A1], [curArea_A2],"
		   << "[curArea_R1], [curArea_R2], [nameID], [eventType], [alarmLevel], [alarmStateType], "
		   << "[abnormalPoint], [angle], [radialLen], [blockLen], [remainTime], [frameNO])"
		   << "VALUES(NULL,"
		   << info->AlarmTime << ","
		   << info->HasSent << ","
		   << info->curArea.A1 << ","
		   << info->curArea.A2 << ","
	       << info->curArea.R1 << ","
	       << info->curArea.R2 << ","
	       << info->EventInfo->blockNameID << ","
	       << info->EventInfo->eventTypeID << ","
	       << info->blockPar->eventType << ","
           << info->blockPar->alarmStateType << ","
           << info->blockPar->abnormalPoint << ","
           << info->blockPar->fAngle << ","
           << info->blockPar->fRadialLen << ","
           << info->blockPar->fBlockLen << ","
           << info->blockPar->fRemainTime << ","
           << info->blockPar->frameNO
		   <<	");";
	 char* errmsg = NULL;
	 LogInfo("插入数据：%s.", ostrSql.str().c_str());
	// int nResult = sqlite3_exec(_recordDB, ostrSql.str().c_str(), NULL, NULL, &errmsg);
	 sqlite3_stmt * stmt;
	 nResult = sqlite3_prepare_v2(_recordDB, ostrSql.str().c_str(), -1, &stmt, NULL);
	 if(SQLITE_OK != nResult)
	 {
	     LogError("sqlite3_prepare_v2 fail, retcode = %d", nResult);
		 sqlite3_finalize(stmt);
		 sqlite3_close( _recordDB);
		 return -1;
	 }

	//执行SQL语句。
	 nResult = sqlite3_step(stmt);

	 if (nResult != SQLITE_DONE)
	 {
		 //LogError(errmsg);
		 //sqlite3_free(errmsg);
		 sqlite3_finalize(stmt);
		 LogError("sqlite3_step fail, retcode = %d", nResult);
		 return -1;
	}
	int id = sqlite3_last_insert_rowid(_recordDB);
	LogInfo("id = %d", id);
	//释放
	sqlite3_finalize(stmt);
    //执行成功后依然要关闭数据库
	sqlite3_close(_recordDB);

	return id;
}

void CAlarmRecordService::Close()
{
	/*if (_CreateSucFlag)
	{
	    sqlite3_close(_recordDB);
	}*/
}

void CAlarmRecordService::RemoveExtendedRecord()
{
	LogFuncEntry();

	int nResult = sqlite3_open_v2( _dbName.c_str(), &_recordDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
	if (nResult != SQLITE_OK)
	{
	     LogError("打开数据库失败：%d.", sqlite3_errmsg(_recordDB));
		 return;
	}

	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tv.tv_sec = tv.tv_sec - 86400;
	struct tm *p;
    p = localtime(&tv.tv_sec);
    char now_time[20] = {0};
    sprintf(now_time, "%4d-%02d-%02d", p->tm_year + 1900, p->tm_mon+1, p->tm_mday);
	stringstream ostrSql("");
	ostrSql << "delete from AlarmInfo where"
	        << " CreatedTime <= datetime('"
	        << now_time << "')";
	LogInfo("删除过期数据，语句：%s.", ostrSql.str().c_str());
	sqlite3_stmt * stmt;
	nResult = sqlite3_prepare_v2(_recordDB, ostrSql.str().c_str(), -1, &stmt, NULL);
    if (SQLITE_OK != nResult) {
		LogError("sqlite3_prepare_v2 fail, retcode = %d", nResult);
		sqlite3_finalize(stmt);
		sqlite3_close(_recordDB);
		return;
	}

    //执行SQL语句。
    nResult = sqlite3_step(stmt);

   if (nResult != SQLITE_DONE) {
		sqlite3_finalize(stmt);
		LogError("sqlite3_step fail, retcode = %d", nResult);
		return;
	}

   //释放
   	sqlite3_finalize(stmt);
       //执行成功后依然要关闭数据库
   	sqlite3_close(_recordDB);

	/*char* errmsg;
	int nResult = sqlite3_exec(_recordDB, ostrSql.str().c_str(), NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		LogError("删除过期数据失败.");
		LogError(errmsg);
	    sqlite3_free(errmsg);
	}
	else
	{
		LogInfo("删除过期数据成功");
	}*/
	LogFuncExit();
}

void CAlarmRecordService::OnRemoveExtendedRecord(void* arg)
{
	if (!_CreateSucFlag)
	{
		return;
	}

	RemoveExtendedRecord();
}
