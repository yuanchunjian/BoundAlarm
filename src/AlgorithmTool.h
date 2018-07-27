#pragma once
#include "SegmentInfo.h"
#include "AlgorithmInfo.h"
#include "CustomType.h"
class CAlgorithmTool
{
protected:
	CAlgorithmTool(void);
	virtual ~CAlgorithmTool(void);
public:
	static CAlgorithmTool* GetInstance();
	static CAlgorithmTool* m_pInstance;

	bool Chebyshev(CPOINT* data, int length, int m, double* result);
	int FirstValidDataIndex(const CAlgorithmInfo* info, const std::vector<double>& data, int startIndex = 0);
	int FirstValidDataIndex(const CAlgorithmInfo* info, const std::vector<ushort>& data,  int startIndex = 0);
	double GetDirection(CPOINT p1, CPOINT p2);
	int GetFrameDiff(ushort teleNum1, ushort teleNum2);
	double GetDistance(CPOINT p1, CPOINT p2);
	double ToArc(double value);
	CPOINT ToCartesian(double angle, double radius);
	double ToDegree(double value);
	CPOINT ToPolar(CPOINT p);
	double MembershipAdd(double d1, double d2);
	double GetInnerData(const std::vector<double>& data, int first, int last, int index);
	double GetInnerData(const std::vector<ushort>& data, int first, int last, int index);
	CPOINT GetMiddlePoint(CPOINT p1, CPOINT p2);
	ushort GetMinValue(ushort* data, int dataLen, int index, int n);
	CPOINT GetTargetPos(CPOINT curPos, double angle, double speed, int span);
	double IntersectAngle(double d1, double d2);
	bool IsInArea(CPolarArea lazy, CPolarArea area);
	bool IsInArea(CPolarArea area, CPOINT p);
	int LastValidDataIndex(const CAlgorithmInfo* info, const std::vector<double>& data);
	int LastValidDataIndex(const CAlgorithmInfo* info, const std::vector<ushort>& data);
	double Interpolate(CPOINT p1, CPOINT p2, double angle);
	std::vector<ushort> ParseData(std::string msg);
	std::vector<std::string> split(std::string str,std::string pattern);
	double NormalIntersect(double intersect);
};

