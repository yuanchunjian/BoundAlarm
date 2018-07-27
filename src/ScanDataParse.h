#pragma once
#include "ParseInterface.h"
#include "Outputchannel16.h"
#include "Outputchannel8.h"
#include "ScanDevice.h"
#include "Encoder.h"
#include "EventInfo.h"
#include "PositionInfo.h"
#include "StatusInfo.h"
#include "TimeInfo.h"
#include <vector>
using namespace std;
class CScanDataParse : public CParseInterface
{
public:
	CScanDataParse(void);
	virtual ~CScanDataParse(void);
	virtual bool Parse(unsigned char buffer[], int& index, bool reverse = true);
public:
	vector<COutputchannel16> Channel16s;
	vector<COutputchannel8> Channel8s;
	string Comment;
	CScanDevice Device;
	string DeviceName;
	vector<CEncoder> Encoders;
	CEventInfo EventInfo;
	CFrequencys Frequencys;
	CPositionInfo PositionInfo;
	CStatusInfo StatusInfo;
	CTimeInfo TimeInfo;
	unsigned short Version;
};

