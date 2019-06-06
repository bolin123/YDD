#ifndef SYS_H
#define SYS_H

#include "HalCommon.h"

#define SysTime() HalGetSysTimeCount()
#define SysTimeHasPast(oldTime, past) (SysTime() - (oldTime) > past)

#define SysDateTime_t HalRTCTime_t
#define SysDateTime() HalRTCGetTime()

#define Syslog(...) {printf("[%s]: ", __FUNCTION__); printf("%s\r\n", __VA_ARGS__);}



#if 0
typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}SysDateTime_t;
#endif 

typedef struct
{
    //uint16_t id;        //�����,��1��ʼ
    uint16_t threshold; //��ֵ
    uint16_t times;     //����ʱ��
    SysDateTime_t date; //��ʼ����
    uint16_t size;      //�ֽ���
    uint32_t startAddr;
}SysDataInfo_t;

typedef struct
{
    uint16_t num;   //������
    uint32_t size;  //�ܳ���
}SysDataRecord_t;

//SysDateTime_t *SysDateTime(void);
int SysDateTimeSet(SysDateTime_t *dateTime);

bool SysDataErase(void);
//SysDataInfo_t *SysDataGetInfo(uint16_t id);
void SysArgsGetRecord(SysDataRecord_t *record);
void SysArgsSetRecord(SysDataRecord_t *record);
void SysArgsGetPointInfo(uint16_t id, SysDataInfo_t *info);
void SysArgsSetPointInfo(uint16_t id, SysDataInfo_t *info);

void SysRawDataRead(uint32_t address, uint8_t *buff, uint16_t length);
void SysRawDataWrite(uint32_t addresss, uint8_t *buff, uint16_t length);

void SysInitalize(void);
void SysPoll(void);

#endif

