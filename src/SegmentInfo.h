#ifndef _SegmentInfo_H
#define _SegmentInfo_H
#include <string>
#include <list>
#include <vector>

class CPOINT
{
	double x;
	double y;
public:
	CPOINT(double _x, double _y){X = _x; Y = _y; x = _x; y = _y;}
	CPOINT(){X = 0; Y = 0; x = 0; y = 0;}
	~CPOINT(){}
	double X;
	double Y;
};

class CSegmentInfo
{
public:
	CSegmentInfo()
	{
		Resolution = 0.5;
		Separator = ',';
	}
	~CSegmentInfo()
	{

	}
	char Separator;
	std::string Data;
    double Resolution;
	double StartAngle;
};

class CDoubleSegmentInfo
{
public:
	CDoubleSegmentInfo()
	{
		Far = new CSegmentInfo;
		Near = new CSegmentInfo;
	}
	~CDoubleSegmentInfo()
	{
		if (Far != NULL)
		{
			delete Far;
			Far = NULL;
		}

		if (Near != NULL)
		{
			delete Near;
			Near = NULL;
		}
	}

	CSegmentInfo* Far;
	CSegmentInfo* Near;
};


class CAreaInfo : public CDoubleSegmentInfo
{
public:
	CAreaInfo()
	{
		FarTrackDistance = 750;
		NearTrackDistance = 750;
		FarPoint.X = 126.61080674323958;
		FarPoint.Y = 3402.9999073307258;
		NearPoint.X = 100.61080674323958;
		NearPoint.Y = 6402.9999073307258;
	}
	~CAreaInfo()
	{

	}
	int FarDistance;
	int FarTrackDistance;
	int NearDistance;
	int NearTrackDistance;
    int TrackDistance;
    CPOINT FarPoint;
    CPOINT NearPoint;
};

class CTrackInfo : public CDoubleSegmentInfo
{
public:
	CTrackInfo(){}
	~CTrackInfo(){}

	double Direction;
	CPOINT EndPoint1;
	CPOINT EndPoint2;
};

class CTrackInfo2
{
public:
	CTrackInfo2()
	{
	}
	~CTrackInfo2(){}
	std::vector<CPOINT> Far;
	std::vector<CPOINT> Near;
};


class CPolarArea
{
public:
	CPolarArea(){A1 = 0; A2 = 0; R1 = 0; R2 = 0;}
	~CPolarArea(){}
	void Enlarge(double angle, double distance)
	{
		A1 -= angle;
		A2 += angle;
		R1 -= distance;
		R2 += distance;
	}

	/*public override string ToString()
	{
		string str = string.Format("{0:f2},{1:f0}~{2:f2},{3:f0}", new object[] { this.A1, this.R1, this.A2, this.R2 });
		return str;
	}*/
    bool CheckValidFlag()
    {
    	if (0 == A1 || 0 == A2 || 0 == R1 || 0 == R2)
    	{
    		return false;
    	}
    	return true;
    }
	double A1;
    double A2;
    double R1;
    double R2;
};


#endif
