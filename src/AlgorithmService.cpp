
#include "AlgorithmService.h"
#include "../pugixml/src/pugiconfig.hpp"
#include "../pugixml/src/pugiutil.hpp"
#include "../pugixml/src/pugixml.hpp"
#include <unistd.h>
#include <iostream>
#include <liblogger/liblogger.h>
//extern HANDLE g_hModule;
CAlgorithmService* CAlgorithmService::_instance = NULL;

CAlgorithmService::CAlgorithmService(void)
{
	CfgFileName = "Algorithm.config";
	_initialized = false;
}


CAlgorithmService::~CAlgorithmService(void)
{
}

CAlgorithmService* CAlgorithmService::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new CAlgorithmService();
	}
	return _instance;
}

void CAlgorithmService::AddAlgorithmInfo(CAlgorithmInfo* info)
{
	for (int i = 0; i < _infos.size(); i++)
	{
		if (_infos.at(i)->SerialNumber == info->SerialNumber)
		{
			_infos.push_back(info);
			break;
		}
	}
}

CAlgorithmInfo* CAlgorithmService::CreateAlgorithmInfo()
{
	CAlgorithmInfo* tmpInfo = new CAlgorithmInfo;
	tmpInfo->MajorVersion = 2;
	return tmpInfo;
}

 CAlgorithmInfo* CAlgorithmService::GetAlgorithmInfo(uint serialNum)
{
	for (int i = 0; i < _infos.size(); i++)
	{
		if (_infos.at(i)->SerialNumber == serialNum)
		{
			return _infos.at(i);
		}
	}
	return NULL;
}

bool CAlgorithmService::Initialize(const char* strExePath)
{
	if (!_initialized)
	{
		try
		{
			if (!LoadConfig(strExePath))
			{
				return false;
			}
			_initialized = true;
		}
		catch (...)
		{
			LogError("Load config abnomal.");
			return false;
		}
	}
	return true;
}


bool CAlgorithmService::LoadConfig(const char* szPath)
{
	std::string fullPath;

	fullPath = std::string(szPath) + std::string("/") + CfgFileName;
	//std::cout << "path:" << fullPath.c_str() << endl;
    pugi::xml_document m_pugiDoc;
	if (!m_pugiDoc.load_file(fullPath.c_str())) 
	{
		LogError("加载算法配置失败，请检测配置是否存在.");
		return false;
	} 
	pugi::xpath_node_set tools = m_pugiDoc.select_nodes("//List/cra:AlgorithmInfo");
	for (pugi::xpath_node_set::const_iterator it = tools.begin(); 
		it !=  tools.end(); ++it)
	{
		pugi::xpath_node AlgorithmInfoNode = *it;
		CAlgorithmInfo* info = new CAlgorithmInfo;
		std::string strFre = AlgorithmInfoNode.node().attribute("Frequency").value();
		std::string strMajorVer = AlgorithmInfoNode.node().attribute("MajorVersion").value();

		std::string strMaxValue = AlgorithmInfoNode.node().attribute("MaxValue").value();
		std::string strMinValue = AlgorithmInfoNode.node().attribute("MinValue").value();
		std::string strResolution = AlgorithmInfoNode.node().attribute("Resolution").value();
		std::string strSerialNumber = AlgorithmInfoNode.node().attribute("SerialNumber").value();
		std::string strTrainDirection = AlgorithmInfoNode.node().attribute("TrainDirection").value();
		info->Frequency = atof(strFre.c_str());
		info->MajorVersion = atoi(strMajorVer.c_str());
		info->MaxValue = (ushort)atoi(strMaxValue.c_str());
		info->MinValue = (ushort)atoi(strMinValue.c_str());
		info->Resolution = atof(strResolution.c_str());
		info->SerialNumber = (uint)atoi(strSerialNumber.c_str());
		if (strTrainDirection == "Both")
			info->TrainDirection = Both; 
		else if (strTrainDirection == "Rigth")
		{
			info->TrainDirection = Right;
		}
		else
		{
			info->TrainDirection = Left;
		}

		pugi::xml_node AreaInfoNode = AlgorithmInfoNode.node().child("cra:AlgorithmInfo.AreaInfo");

		std::string strFarDistance = AreaInfoNode.child("cra:AreaInfo").attribute("FarDistance").value();
		std::string strNearDistance = AreaInfoNode.child("cra:AreaInfo").attribute("NearDistance").value();
		std::string strTrackDistance = AreaInfoNode.child("cra:AreaInfo").attribute("TrackDistance").value();
		info->AreaInfo.FarDistance = atoi(strFarDistance.c_str());
		info->AreaInfo.NearDistance = atoi(strNearDistance.c_str());
		info->AreaInfo.TrackDistance = atoi(strTrackDistance.c_str());

		std::string strPoint = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Far").child("cv:Point").value();
		//info->AreaInfo.FarPoint = ;
		info->AreaInfo.Far->Data = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Far").child("cra:SegmentInfo").attribute("Data").value();
		std::string strFarResolution = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Far").child("cra:SegmentInfo").attribute("Resolution").value();
		info->AreaInfo.Far->Resolution = atof(strFarResolution.c_str());
		std::string strFarStartAngle = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Far").child("cra:SegmentInfo").attribute("StartAngle").value();
		info->AreaInfo.Far->StartAngle = atof(strFarStartAngle.c_str());
		
		info->AreaInfo.Near->Data = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Near").child("cra:SegmentInfo").attribute("Data").value();
		std::string strNearResolution = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Near").child("cra:SegmentInfo").attribute("Resolution").value();
		info->AreaInfo.Near->Resolution = atof(strNearResolution.c_str());
		std::string strNearStartAngle = AreaInfoNode.child("cra:AreaInfo").child("cra:AreaInfo.Near").child("cra:SegmentInfo").attribute("StartAngle").value();
		info->AreaInfo.Near->StartAngle = atof(strNearStartAngle.c_str());
		
		pugi::xml_node LandslipIdentifyInfoNode = AlgorithmInfoNode.node().child("cra:AlgorithmInfo.LandslipIdentifyInfo");
		//Info->LandslipIdentifyInfo
		pugi::xml_node LazyAreaNode = AlgorithmInfoNode.node().child("cra:AlgorithmInfo.LazyArea"); 
		//Info->LazyArea
		pugi::xml_node NonDetectedAreaNode = AlgorithmInfoNode.node().child("cra:AlgorithmInfo.NonDetectedArea"); 
		//������̽�����
		std::string strCapacity = NonDetectedAreaNode.child("List").attribute("Capacity").value();
		int iCapacity = atoi(strCapacity.c_str());
	    if (iCapacity > 0)
		{
			pugi::xml_node ListTools = NonDetectedAreaNode.child("List"); 
			for (pugi::xml_node tmptool = ListTools.first_child(); tmptool; tmptool = tmptool.next_sibling()) 
			{
				CPolarArea tmpPolarArea;
				std::string strTmp = tmptool.attribute("A1").value();
				tmpPolarArea.A1 = atof(strTmp.c_str());
				strTmp = tmptool.attribute("A2").value();
				tmpPolarArea.A2 = atof(strTmp.c_str());
				strTmp = tmptool.attribute("R1").value();
				tmpPolarArea.R1 = atof(strTmp.c_str());
				strTmp = tmptool.attribute("R2").value();
				tmpPolarArea.R2 = atof(strTmp.c_str());
				info->NonDetectedArea.push_back(tmpPolarArea);
			}
		}
		//������Ϣ��ʱ���������Ժ�����
	    pugi::xml_node TrackInfoNode = AlgorithmInfoNode.node().child(
				"cra:AlgorithmInfo.TrackInfo");
		if (TrackInfoNode != NULL) {
			std::string
					strEndPoint =
							TrackInfoNode.child("cra:TrackInfo").attribute(
									"EndPoint1").value();
			if (strEndPoint != "{x:Null}" && strEndPoint != "") {
				int pos = strEndPoint.find(",");
				if (pos != std::string::npos) {
					std::string strX, strY;
					strX = strEndPoint.substr(0, pos);
					strY = strEndPoint.substr(pos + 1,
							strEndPoint.length() - pos);
					info->TrackInfo.EndPoint1.X = atof(strX.c_str());
					info->TrackInfo.EndPoint1.Y = atof(strY.c_str());
				} else {
					info->TrackInfo.EndPoint1.X = 0.0;
					info->TrackInfo.EndPoint1.Y = 0.0;
				}
			}

			strEndPoint = TrackInfoNode.child("cra:TrackInfo").attribute(
					"EndPoint2").value();
			if (strEndPoint != "{x:Null}" && strEndPoint != "") {
				int pos = strEndPoint.find(",");
				if (pos != std::string::npos) {
					std::string strX, strY;
					strX = strEndPoint.substr(0, pos);
					strY = strEndPoint.substr(pos + 1,
							strEndPoint.length() - pos);
					info->TrackInfo.EndPoint2.X = atof(strX.c_str());
					info->TrackInfo.EndPoint2.Y = atof(strY.c_str());
				} else {
					info->TrackInfo.EndPoint2.X = 0.0;
					info->TrackInfo.EndPoint2.Y = 0.0;
				}
			}

			std::string
					strDirection =
							TrackInfoNode.child("cra:TrackInfo").attribute(
									"Direction").value();
			info->TrackInfo.Direction = atof(strDirection.c_str());
			std::string strData = TrackInfoNode.child("cra:TrackInfo").child(
					"cra:TrackInfo.Far").child("cra:SegmentInfo").attribute(
					"Data").value();
			if (strData == "{x:Null}" && strEndPoint != "") {
				info->TrackInfo.Far->Data = "";
			} else {
				info->TrackInfo.Far->Data = strData;
			}
			std::string
					strStartAngle =
							TrackInfoNode.child("cra:TrackInfo").child(
									"cra:TrackInfo.Far").child(
									"cra:SegmentInfo").attribute("StartAngle").value();
			info->TrackInfo.Far->StartAngle = atoi(strStartAngle.c_str());

			strData = TrackInfoNode.child("cra:TrackInfo").child(
					"cra:TrackInfo.Near").child("cra:SegmentInfo").attribute(
					"Data").value();
			if (strData == "{x:Null}") {
				info->TrackInfo.Near->Data = "";
			} else {
				info->TrackInfo.Near->Data = strData;
			}
			strStartAngle = TrackInfoNode.child("cra:TrackInfo").child(
					"cra:TrackInfo.Near").child("cra:SegmentInfo").attribute(
					"StartAngle").value();
			info->TrackInfo.Near->StartAngle = atoi(strStartAngle.c_str());
		}

		// ---Joan Add Begin----
		pugi::xml_node TrackInfo2Node = AlgorithmInfoNode.node().child(
				"cra:AlgorithmInfo.TrackInfo2");
		if (TrackInfo2Node != NULL) {
			strCapacity
					= TrackInfo2Node.child("cra:TrackInfo2").child(
							"cra:TrackInfo2.Far").child("List").attribute(
							"Capacity").value();
			iCapacity = atoi(strCapacity.c_str());
			if (iCapacity > 0) {
				pugi::xml_node ListTrackInfo2Tools = TrackInfo2Node.child(
						"cra:TrackInfo2").child("cra:TrackInfo2.Near").child(
						"List");
				for (pugi::xml_node tmptool = ListTrackInfo2Tools.first_child(); tmptool; tmptool
						= tmptool.next_sibling()) {
					std::string strtest = tmptool.first_child().value();
					int pos = strtest.find(",");
					if (pos != std::string::npos) {
						std::string strX, strY;
						strX = strtest.substr(0, pos);
						strY = strtest.substr(pos + 1, strtest.length() - pos);
						CPOINT stPoint;
						stPoint.X = atof(strX.c_str());
						stPoint.Y = atof(strY.c_str());
						info->TrackInfo2.Far.push_back(stPoint);
					}
				}
			}

			strCapacity
					= TrackInfo2Node.child("cra:TrackInfo2").child(
							"cra:TrackInfo2.Near").child("List").attribute(
							"Capacity").value();
			iCapacity = atoi(strCapacity.c_str());

			if (iCapacity > 0) {
				pugi::xml_node ListTrackInfo2Tools = TrackInfo2Node.child(
						"cra:TrackInfo2").child("cra:TrackInfo2.Near").child(
						"List");
				for (pugi::xml_node tmptool = ListTrackInfo2Tools.first_child(); tmptool; tmptool
						= tmptool.next_sibling()) {
					std::string strtest = tmptool.first_child().value();
					int pos = strtest.find(",");
					if (pos != std::string::npos) {
						std::string strX, strY;
						strX = strtest.substr(0, pos);
						strY = strtest.substr(pos + 1, strtest.length() - pos);
						CPOINT stPoint;
						stPoint.X = atof(strX.c_str());
						stPoint.Y = atof(strY.c_str());
						info->TrackInfo2.Near.push_back(stPoint);
					}
				}
			}
			// ---Joan Add End----
		}

		_infos.push_back(info);
	}

	return true;
}

void CAlgorithmService::SaveConfig()
{

}

void CAlgorithmService::UnInitialize()
{
	if (_initialized)
	{
		_initialized = false;
		try
		{
			//�ͷ����
			for (int i = 0; i < _infos.size(); i++)
			{
				if (_infos.at(i) != NULL)
				{
					delete _infos.at(i);
					_infos.at(i) = NULL;
				}
			}
			_infos.clear();
		}
		catch(...)
		{
			
			//ServiceProvider.RemoveService<AlgorithmService>();
		}
	}
}

void CAlgorithmService::UpgradeInfo()
{
	if (_infos.size() != 0)
	{
		for(int i = 0; i < _infos.size(); i++)
		{
			try
			{
				//UpgradeInfo(_infos.at(i));
			}
			catch (...)
			{
				
			}
		}
	}
}

void CAlgorithmService::UpgradeInfo(CAlgorithmInfo* info)
{
	if (info->MajorVersion == 0)
	{
		try
		{
			
				info->TrackInfo2.Far.clear();
				info->TrackInfo2.Near.clear();
				CTrackInfo trackInfo = info->TrackInfo;
				/*
					Segment segment = Segment.CreateSegment(trackInfo.Near);
					if ((segment.Data != null) && (segment.Data.Length != 0))
					{
						for (int i = 0; i < segment.Data.Length; i++)
						{
							ushort num2 = segment.Data[i];
							if ((num2 < Info->MaxValue) && (num2 > Info->MinValue))
							{
								Info->TrackInfo2.Near.Add(new Point(trackInfo.Near.StartAngle + (i * trackInfo.Near.Resolution), (double)num2));
								break;
							}
						}
						for (int j = segment.Data.Length - 1; j > 0; j--)
						{
							ushort num4 = segment.Data[j];
							if ((num4 < Info->MaxValue) && (num4 > Info->MinValue))
							{
								Info->TrackInfo2.Near.Add(new Point(trackInfo.Near.StartAngle + (j * trackInfo.Near.Resolution), (double)num4));
								break;
							}
						}
					}
					/*}
					else*/
				{
					info->TrackInfo2.Near.push_back(trackInfo.EndPoint1);
					info->TrackInfo2.Near.push_back(trackInfo.EndPoint2);
				}
			
		}
		catch(...)
		{
			//Info->TrackInfo = null;
			info->MajorVersion = 2;
		}
	}
}
