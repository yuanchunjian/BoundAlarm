/*
 * CSickProtocol.cpp
 *
 *  Created on: Apr 1, 2016
 *      Author: yuan
 */

#include "CSickProtocol.h"
#include "../pugixml/src/pugiconfig.hpp"
#include "../pugixml/src/pugiutil.hpp"
#include "../pugixml/src/pugixml.hpp"

CSickProtocol::CSickProtocol() {
	// TODO Auto-generated constructor stub
	mapProtocolInfo.clear();
}

CSickProtocol::~CSickProtocol() {
	// TODO Auto-generated destructor stub
	mapProtocolInfo.clear();
}

void CSickProtocol::LoadConfig(const char* szPath, const char* cfgName)
{
	std::string fullPath;
    fullPath = std::string(szPath) + std::string("/") + std::string(cfgName);

	pugi::xml_document m_pugiDoc;
	if (!m_pugiDoc.load_file(fullPath.c_str()))
	{
		return;
	}

	pugi::xpath_node_set tools = m_pugiDoc.select_nodes("//ProtocolInfo/ProtocolInfo.SupportCommands/SendCmdInfo");
	for (pugi::xpath_node_set::const_iterator it = tools.begin();
	    		it !=  tools.end(); ++it)
	{
	    pugi::xpath_node tagNode = *it;
	    S_Par tmpPar;
	    S_CmdInfo tmpCmdInfo;
	    S_ResponseInfo tmpResponseInfo;
        tmpCmdInfo.strCmdName = tagNode.node().attribute("CmdName").value();
        tmpCmdInfo.strComment = tagNode.node().attribute("Comment").value();
        tmpCmdInfo.strName = tagNode.node().attribute("Name").value();
        tmpCmdInfo.vecPar.clear();
        pugi::xpath_node parNode = tagNode.node().child("SendCmdInfo.Parameters");
        if (parNode != NULL)
        {
        	pugi::xpath_node_set parIter = parNode.node().select_nodes("//SickParameter");
        	for (pugi::xpath_node_set::const_iterator it1 = parIter.begin(); it1 != parIter.end(); it1++)
        	{
        		pugi::xpath_node tagPar = *it1;
                tmpPar.strName = tagPar.node().attribute("Name").value();
                tmpPar.strValueType = tagPar.node().attribute("ValueType").value();
                tmpCmdInfo.vecPar.push_back(tmpPar);
        	}
        }
        pugi::xpath_node ResponseNode = tagNode.node().child("SendCmdInfo.Response").child("ResponseCmdInfo");
        tmpResponseInfo.vecPar.clear();
        if (ResponseNode != NULL)
        {
        	tmpResponseInfo.strCmdName = ResponseNode.node().attribute("CmdName").value();
        	parNode = tagNode.node().child("ResponseCmdInfo.Parameters");
        	if (parNode != NULL)
        	{
        		pugi::xpath_node_set parIter = parNode.node().select_nodes("//SickParameter");
        		for (pugi::xpath_node_set::const_iterator it1 = parIter.begin(); it1 != parIter.end(); it1++)
        		{
        		    pugi::xpath_node tagPar = *it1;
        		    tmpPar.strName = tagPar.node().attribute("Name").value();
        		    tmpPar.strValueType = tagPar.node().attribute("ValueType").value();
        		    tmpResponseInfo.vecPar.push_back(tmpPar);
        		}
        	}
        }
        S_ProtocolInfo tmpProtocolInfo;
        tmpProtocolInfo.CmdInfo = tmpCmdInfo;
        tmpProtocolInfo.ResponseInfo = tmpResponseInfo;
        mapProtocolInfo.insert(make_pair(tmpCmdInfo.strName, tmpProtocolInfo));
	}

}
