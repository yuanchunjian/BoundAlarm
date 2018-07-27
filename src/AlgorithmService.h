#pragma once
#include "AlgorithmInfo.h"
#include "CustomType.h"

class CAlgorithmService
{
public:
	static CAlgorithmService* GetInstance();
	void AddAlgorithmInfo(CAlgorithmInfo* info);
	static CAlgorithmInfo* CreateAlgorithmInfo();
	CAlgorithmInfo* GetAlgorithmInfo(uint serialNum);
	bool Initialize(const char* strExePath);
	void UnInitialize();
	void UpgradeInfo();
	void UpgradeInfo(CAlgorithmInfo* info);
	bool LoadConfig(const char* szPath);
	void SaveConfig();
protected:
	CAlgorithmService(void);
	virtual ~CAlgorithmService(void);
private:
	std::vector<CAlgorithmInfo*> _infos;
	static CAlgorithmService* _instance;
	std::string CfgFileName;
    bool _initialized;
};

