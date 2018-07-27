#ifndef _CheckAlgorithm_H
#define _CheckAlgorithm_H
#include "CustomType.h"

#define  COMMON_BLOCK   0x01
#define  PERSON         0x02
#define  ANIMAL         0x03
#define  LANDSLIP       0x04
#define  TRAIN          0x05
#define  DEFAULT_BLOCK  0x06

#define FIND_BLOCK_ALEAR      0x01
#define PASS_PROTECTION_AREA  0x02

#define  ONE_LEVEL_ALARM   0x01  
#define  TWO_LEVEL_ALARM   0x02  
#define  ALARM_REMOVE      0x03

#define ENTER_PROTECTION_AREA  0x01
#define LEAVE_PROTECTION_AREA  0x02

#define  ABNORMAL_POINT  0x00
#define  NORMAL_POINT    0x01

#define PERSON_ENTER_AREA       0x04
#define PERSON_LEAVE_AREA       0x05
#define PERSON_ONE_LEVEL_ALARM  0x01
#define PERSON_TWO_LEVEL_ALARM  0x02
#define PERSON_ALARM_REMOVE     0x03

#define ANIMAL_ENTER_AREA       0x04
#define ANIMAL_LEAVE_AREA       0x05
#define ANIMAL_ONE_LEVEL_ALARM  0x01
#define ANIMAL_TWO_LEVEL_ALARM  0x02
#define ANIMAL_ALARM_REMOVE     0x03

#define LANDSLIP_ENTER_AREA       0x04
#define LANDSLIP_LEAVE_AREA       0x05
#define LANDSLIP_ONE_LEVEL_ALARM  0x01
#define LANDSLIP_TWO_LEVEL_ALARM  0x02
#define LANDSLIP_ALARM_REMOVE     0x03

#define TRAIN_ENTER  0x04
#define TRAIN_LEAVE  0x05

#define TRAIN_LEFT   0x00  //������
#define TRAIN_RIGHT  0x01  //������

//�����붨��
#define SUCCESS     0
#define DATA_ERROR  -1
#define CHECK_ERROR -2
#define CONFIG_ERROR -3
#define INIT_ERROR -4


typedef struct S_BlockEventPar
{
	//<报警级别 0x01一级报警 0x02二级报警
	int eventType;
	/// 0x01 报警 Alarm
	/// 0x02 恢复 Resume
    /// 0x03 取消 Cancel
	/// 0x00 未知 Unkonwn
	int alarmStateType;
	int abnormalPoint;
	int  ID;       //<报警ID号，累加计数
	float fAngle;  //<当前位置角度
	float fRadialLen; //<当前位置幅长
	float fBlockLen;
	float fRemainTime;
	uint frameNO;
	/*add RFID flag*/
	int rfidFlag;
}S_BlockEventPar, *LP_S_BlockEventPar;

typedef struct S_PassProtectionAreaPar
{
	int eventType;
	unsigned int  ID;
	float fRadialLen;
	float fAngle;
	float fSpeed;
	union
	{
		float fDirection;   //<
		int ucDirection;  //<
	}unionDirection;
	float fRemainTime;
	uint enterFrameNO;
	uint leaveFrameNO;
}S_PassProtectionAreaPar, LP_S_PassProtectionAreaPar;


typedef struct S_EventInfo
{
	int blockNameID;  //<0x01一般障碍物 0x02行人 0x03小动物 0x04泥石流 0x05火车
	int eventTypeID;  //<0x01障碍物 0x02穿过防区
	void* eventPar;   //<对应参数
}S_EventInfo, *LP_S_EventInfo;

typedef struct S_RetFramePar
{
	uint LMS_ID;  //�״�ID
	uint uiLmsCode; //�״���
	char chLmsName[256]; //�״����
	char chFrameTime[256]; //ʱ��
	float dbStartAngle; //�״�ɨ�迪ʼ�Ƕ�
	float dbEndAngle;  //�״�ɨ�����Ƕ�
	float dbResolution; //�״�Ƿֱ���
	uint  uiFrameNo;   //���֡��
	uchar deviceStatus;
}S_RetFramePar, *LP_S_RetFramePar;

typedef enum
{
	Unknown,
	Alarm,
	Resume,
	Cancel
}AlarmStateType;

extern "C"  typedef void ( *PFEventCallback)(void* eventPar,  void* retPar);

#endif
