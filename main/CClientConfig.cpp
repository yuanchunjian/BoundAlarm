/*
 * CClientConfig.cpp
 *
 *  Created on: Apr 1, 2016
 *      Author: yuan
 */

#include "CClientConfig.h"
#include "../pugixml/src/pugiconfig.hpp"
#include "../pugixml/src/pugiutil.hpp"
#include "../pugixml/src/pugixml.hpp"

CClientConfig::CClientConfig() {
	// TODO Auto-generated constructor stub

}

CClientConfig::~CClientConfig() {
	// TODO Auto-generated destructor stub
}

void CClientConfig::LoadConfig(const char* szPath)
{
	std::string fullPath;
	fullPath = std::string(szPath) + std::string("/DeviceClient.config");

	pugi::xml_document m_pugiDoc;
	if (!m_pugiDoc.load_file(fullPath.c_str()))
	{
		return;
	}
	std::string strVal;
	pugi::xpath_node MainNode = m_pugiDoc.select_single_node("//configuration/Main");
	strVal = MainNode.node().attribute("VersionNum").value();
	VersionNum = atoi(strVal.c_str());

	pugi::xpath_node DeviceNode = m_pugiDoc.select_single_node("//configuration/Device");
	DeviceName = DeviceNode.node().attribute("Name").value();
	strVal = DeviceNode.node().attribute("DeviceID").value();
	DeviceID = atoi(strVal.c_str());
	strVal = DeviceNode.node().attribute("SerialNum").value();
	SerialNum = atoi(strVal.c_str());
	CfgFileName = DeviceNode.node().attribute("CfgFileName").value();
	strVal = DeviceNode.node().attribute("TimeOut").value();
	SickTimeout = atoi(strVal.c_str());

	Channels.strName = DeviceNode.node().child("Channels").first_child().attribute("Name").value();
	Channels.strIP = DeviceNode.node().child("Channels").first_child().attribute("Address").value();
	strVal = DeviceNode.node().child("Channels").first_child().attribute("PortNum").value();
	Channels.port = atoi(strVal.c_str());
    strVal = DeviceNode.node().child("Channels").first_child().attribute("ReceiveBufferLength").value();
    Channels.dataBufLen = atoi(strVal.c_str());

    S_DeviceTag tmpTag;
    pugi::xpath_node_set tools = m_pugiDoc.select_nodes("//configuration/Device/DeviceTag");
    for (pugi::xpath_node_set::const_iterator it = tools.begin();
    		it !=  tools.end(); ++it)
    {
    	pugi::xpath_node tagNode = *it;
    	tmpTag.strName = tagNode.node().attribute("Name").value();
    /*	tmpTag.strRegister = tagNode.node().attribute("Register").value();
    	tmpTag.strValueType = tagNode.node().attribute("ValueType").value();
        tmpTag.strRWMod = tagNode.node().attribute("RWMod").value();*/
        strVal = tagNode.node().attribute("SamplePeriod").value();
        tmpTag.SamplePeriod = atoi(strVal.c_str());
        pugi::xpath_node_set parIter = tagNode.node().select_nodes("//Para");
        for (pugi::xpath_node_set::const_iterator it1 = parIter.begin(); it1 != parIter.end(); it1++)
        {
             pugi::xpath_node tagPar = *it1;
             S_CmdPara tmpPar;
             tmpPar.strName = tagPar.node().attribute("Name").value();
             tmpPar.strValue = tagPar.node().attribute("Value").value();
            tmpTag.vecPar.push_back(tmpPar);
        }
        DeviceTags.push_back(tmpTag);
    }
  /*  pugi::xpath_node parNode = DeviceNode.node().child("Para");
    strVal = parNode.node().child("BasicPara").attribute("AngleResolution").value();
    BasePar.AngleResolution = atof(strVal.c_str());
    strVal = parNode.node().child("BasicPara").attribute("StartAngle").value();
    BasePar.StartAngle = atof(strVal.c_str());
    strVal = parNode.node().child("BasicPara").attribute("EndAngle").value();
    BasePar.EndAngle = atof(strVal.c_str());
    strVal = parNode.node().child("BasicPara").attribute("Frequency").value();
    BasePar.Frequency = atoi(strVal.c_str());

    strVal = parNode.node().child("ContaminationPara").attribute("ErrorThreshold").value();
    ContaminationPara.ErrorThreshold = atoi(strVal.c_str());
    ContaminationPara.Strategy = parNode.node().child("ContaminationPara").attribute("Strategy").value();
    strVal = parNode.node().child("ContaminationPara").attribute("WarningThreshold").value();
    ContaminationPara.WarningThreshold = atoi(strVal.c_str());*/

    pugi::xpath_node ServerNode = m_pugiDoc.select_single_node("//configuration/DataServer");
    DataServer.strIP = ServerNode.node().attribute("Address").value();
    strVal = ServerNode.node().attribute("PortNum").value();
    DataServer.port = atoi(strVal.c_str());

    pugi::xpath_node SaveDataNode = m_pugiDoc.select_single_node("//configuration/SaveRawLmsData");
    strVal = SaveDataNode.node().attribute("IsSave").value();
    SaveRawData.isSave = atoi(strVal.c_str());
    SaveRawData.strPath = SaveDataNode.node().attribute("Path").value();
    strVal = SaveDataNode.node().attribute("MaxRecFileTime").value();
    SaveRawData.saveTime = atoi(strVal.c_str());

    pugi::xpath_node AlarmRecordNode = m_pugiDoc.select_single_node("//configuration/AlarmRecordDB");
    AlarmRecordDbPath = AlarmRecordNode.node().attribute("Path").value();

    pugi::xpath_node HeartBeatNode = m_pugiDoc.select_single_node("//configuration/Heartbeat");
    strVal = HeartBeatNode.node().attribute("TurnOn").value();
    Heartbeat.bTurnOn = atoi(strVal.c_str());
    strVal = HeartBeatNode.node().attribute("IntervalSec").value();
    Heartbeat.intervalSec = atoi(strVal.c_str());

    pugi::xpath_node ConfigServerNode = m_pugiDoc.select_single_node("//configuration/ConfigureServer");
    ConfigureServer.strIP = ConfigServerNode.node().attribute("IP").value();
    strVal = ConfigServerNode.node().attribute("PortNum").value();
    ConfigureServer.port = atoi(strVal.c_str());

    pugi::xpath_node UartNode = m_pugiDoc.select_single_node("//configuration/Uart");
    UartName = strVal = UartNode.node().attribute("Name").value();
}
