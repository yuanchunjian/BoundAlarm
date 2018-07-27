/*
 * CLogConfig.h
 *
 *  Created on: Apr 1, 2016
 *      Author: yuan
 */

#ifndef CLOGCONFIG_H_
#define CLOGCONFIG_H_
#include <string>

class CLogConfig {
protected:
	CLogConfig();
	virtual ~CLogConfig();
public:
	bool LoadConfig(const char* path);
	static CLogConfig* GetInstance();
	static CLogConfig* m_pInstance;
public:
	std::string _strConfigPath;
    int _logLevel;
	std::string _strPath;
	std::string _strName;
    unsigned char _maxfilesize;
    unsigned char _openMode;
    unsigned char _outType;
	std::string _socketIp;
	unsigned short _socketPort;
};


#endif /* CLOGCONFIG_H_ */
