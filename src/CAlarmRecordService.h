/*
 * CAlarmRecordService.h
 *
 *  Created on: May 12, 2016
 *      Author: yuan
 */

#ifndef CALARMRECORDSERVICE_H_
#define CALARMRECORDSERVICE_H_
#include "CAlarmInfo.h"
#include <sqlite3.h>
#include <string>

class CAlarmRecordService {
public:
	CAlarmRecordService();
	virtual ~CAlarmRecordService();

	bool Create();
	void Close();
	void SetDbPath(const char* path);

	/*
	 * 增加报警信息，返回报警的ID，如果返回-1，则失败
	 */
	static void RemoveExtendedRecord();
	int AddRecord(CAlarmInfo* info);
	static void OnRemoveExtendedRecord(void* arg);
	static bool _CreateSucFlag;
	static sqlite3* _recordDB;
private:
    std::string _dbPath;
    static std::string _dbName;
};

#endif /* CALARMRECORDSERVICE_H_ */
