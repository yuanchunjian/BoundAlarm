/*
 * CSickProtocol.h
 *
 *  Created on: Apr 1, 2016
 *      Author: yuan
 */

#ifndef CSICKPROTOCOL_H_
#define CSICKPROTOCOL_H_
#include <string>
#include <vector>
#include <map>

typedef struct S_Par
{
	std::string strName;
	std::string strValueType;
}S_Par, *LP_S_Par;

typedef struct S_CmdInfo
{
   std::string strCmdName;
   std::string strComment;
   std::string strName;
   std::vector<S_Par> vecPar;
}S_CmdInfo, *LP_S_CmdInfo;

typedef struct S_ResponseInfo
{
    std::string strCmdName;
    std::vector<S_Par> vecPar;
}S_ResponseInfo, *LP_S_ResponseInfo;

typedef struct S_ProtocolInfo
{
	S_CmdInfo CmdInfo;
	S_ResponseInfo ResponseInfo;
}S_ProtocolInfo, *LP_S_ProtocolInfo;

class CSickProtocol {
public:
	CSickProtocol();
	virtual ~CSickProtocol();

    void LoadConfig(const char* szPath, const char* cfgName);

    std::map<std::string, S_ProtocolInfo> mapProtocolInfo;
};

#endif /* CSICKPROTOCOL_H_ */
