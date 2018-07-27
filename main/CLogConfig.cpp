/*
 * CLogConfig.cpp
 *
 *  Created on: Apr 1, 2016
 *      Author: yuan
 */

#include "CLogConfig.h"
#include "../pugixml/src/pugiconfig.hpp"
#include "../pugixml/src/pugiutil.hpp"
#include "../pugixml/src/pugixml.hpp"

CLogConfig* CLogConfig::m_pInstance = new CLogConfig;

CLogConfig* CLogConfig::GetInstance()
 {
	 return m_pInstance;
 }

CLogConfig::CLogConfig() {
	// TODO Auto-generated constructor stub
	//CfgFileName = "Log.config";
}

CLogConfig::~CLogConfig() {
	// TODO Auto-generated destructor stub
    if (m_pInstance != NULL)
    {
    	delete m_pInstance;
    	m_pInstance = NULL;
    }
}
/*
*/

bool CLogConfig::LoadConfig(const char* path)
{
	std::string fullPath = std::string(path) + std::string("/BoundAlarm_log.config");

	pugi::xml_document m_pugiDoc;
	if (!m_pugiDoc.load_file(fullPath.c_str()))
	{
		return false;
	}

	pugi::xpath_node levelNode = m_pugiDoc.select_single_node("//configuration/loglevel");
	std::string strVal;
	if (levelNode != NULL)
	{
		strVal = levelNode.node().attribute("val").value();
		_logLevel = atoi(strVal.c_str());
	}

	pugi::xpath_node outputNode = m_pugiDoc.select_single_node("//configuration/output");
	if (outputNode != NULL)
	{
        strVal = outputNode.node().attribute("logTo").value();
		_outType = atoi(strVal.c_str());
		_socketIp = outputNode.node().child("socket").attribute("ip").value();
		strVal = outputNode.node().child("socket").attribute("port").value();
		_socketPort = atoi(strVal.c_str());

		_strPath = outputNode.node().child("file").attribute("path").value();
		_strName = outputNode.node().child("file").attribute("name").value();
		strVal = outputNode.node().child("file").attribute("openmode").value();
		_openMode = atoi(strVal.c_str());
		strVal = outputNode.node().child("file").attribute("maxsize").value();
		_maxfilesize = atoi(strVal.c_str());
	}
    return true;
}

