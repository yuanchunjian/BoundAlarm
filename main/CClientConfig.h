/*
 * CClientConfig.h
 *
 *  Created on: Apr 1, 2016
 *      Author: yuan
 */

#ifndef CCLIENTCONFIG_H_
#define CCLIENTCONFIG_H_
#include <string>
#include <vector>

typedef struct S_Channels
{
   std::string strName;
   std::string strIP;
   unsigned int port;
   unsigned int dataBufLen;
}S_Channels, *LP_S_Channels;

typedef struct S_CmdPara
{
	std::string strName;
	std::string strValue;
}S_CmdPara, *LP_S_CmdPara;

typedef struct S_DeviceTag
{
	std::string strName;
	unsigned int SamplePeriod;
	std::vector<S_CmdPara> vecPar;
}S_DeviceTag, *LP_S_DeviceTag;
/*
typedef struct S_BasicPar
{
	float AngleResolution;
    float StartAngle;
    float EndAngle;
    unsigned char Frequency;
}S_BasicPar, *LP_S_BasicPar;

typedef struct S_ContaminationPara
{
	unsigned short ErrorThreshold;
    std::string    Strategy;
	unsigned short WarningThreshold;
}S_ContaminationPara, *LP_S_ContaminationPara;
*/
typedef struct S_DataServer
{
	std::string strIP;
	unsigned int port;
}S_DataServer, *LP_S_DataServer;

typedef struct S_SaveRawLmsData
{
	bool isSave;
	std::string strPath;
	unsigned char saveTime;
}S_SaveRawLmsData, *LP_S_SaveRawLmsData;

typedef struct S_Heartbeat
{
	bool bTurnOn;
	int intervalSec;
}S_Heartbeat, *LP_S_Heartbeat;

class CClientConfig {
public:
	CClientConfig();
	virtual ~CClientConfig();
    void LoadConfig(const char* path);

public:
   int VersionNum;
   std::string DeviceName;
   int DeviceID;
   unsigned int SerialNum;
   std::string CfgFileName;
   long SickTimeout;
   S_Channels Channels;
   std::vector<S_DeviceTag> DeviceTags;
  // S_BasicPar BasePar;
  //S_ContaminationPara ContaminationPara;
   S_DataServer DataServer;
   S_SaveRawLmsData SaveRawData;
   S_Heartbeat Heartbeat;
   S_DataServer ConfigureServer;
   std::string AlarmRecordDbPath;
   std::string UartName;
};

#endif /* CCLIENTCONFIG_H_ */
