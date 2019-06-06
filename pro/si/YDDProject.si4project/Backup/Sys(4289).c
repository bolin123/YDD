#include "Sys.h"
#include "Protocol.h"
#include "W25Q64.h"

/*
SysDateTime_t *SysDateTime(void)
{

    static SysDateTime_t date;
    date.year = 19;
    date.month = 5;
    date.day = 18;
    date.hour = 10;
    date.minute = 41;
    date.second = 45;
    return &date;

    return (SysDateTime_t *)HalRTCGetTime();
}
*/

int SysDateTimeSet(SysDateTime_t *dateTime)
{
    return HalRTCSetTime((HalRTCTime_t *)dateTime);
}

void SysArgsGetRecord(SysDataRecord_t *record)
{
    if(record != NULL)
    {
        HalFlashRead(HAL_DATA_RECORD_ADDR, (uint8_t *)record, sizeof(SysDataRecord_t));
        if(record->num == 0xffff)
        {
            record->num = 0;
            record->size = 0;
            HalFlashWrite(HAL_DATA_RECORD_ADDR, (const uint8_t *)record, sizeof(SysDataRecord_t));
        }
    }
}

void SysArgsSetRecord(SysDataRecord_t *record)
{
    if(record != NULL)
    {
        HalFlashWrite(HAL_DATA_RECORD_ADDR, (const uint8_t *)record, sizeof(SysDataRecord_t));
    }
}

//id start 1,2,3...
void SysArgsSetPointInfo(uint16_t id, SysDataInfo_t *info)
{
    uint32_t addr;
    //HAL_DATA_POINT_INFO_ADDR
    if(info != NULL)
    {
        addr = HAL_DATA_POINT_INFO_ADDR + ((id - 1) * sizeof(SysDataInfo_t));
        HalFlashWrite(addr, (const uint8_t *)info, sizeof(SysDataInfo_t));
    }
}

void SysArgsGetPointInfo(uint16_t id, SysDataInfo_t *info)
{
    uint32_t addr;
    //HAL_DATA_POINT_INFO_ADDR
    if(info != NULL)
    {
        addr = HAL_DATA_POINT_INFO_ADDR + ((id - 1) * sizeof(SysDataInfo_t));
        HalFlashRead(addr, (const uint8_t *)info, sizeof(SysDataInfo_t));
    }
}

void SysRawDataRead(uint32_t addresss, uint8_t *buff, uint16_t length)
{
    W25Q64Read(buff, addresss, length);
}

void SysRawDataWrite(uint32_t addresss, uint8_t *buff, uint16_t length)
{
    W25Q64Write(buff, addresss, length);
}

#if 0
SysDataInfo_t *SysDataGetInfo(uint16_t id)
{
    static SysDataInfo_t dataInfo;
    dataInfo.id = 1;
    dataInfo.threshold = 150;
    dataInfo.times = 120;
    dataInfo.size = dataInfo.times * 16;
    dataInfo.date.year = 2019;
    dataInfo.date.month = 5;
    dataInfo.date.day = 17;
    dataInfo.date.hour = 12;
    dataInfo.date.minute = 52;
    dataInfo.date.second = 32;
    return &dataInfo;
}
#endif
bool SysDataErase(void)
{
    return true;
}

void SysInitalize(void)
{
    HalCommonInitialize();
    ProtocolInitialize();
    W25Q64Initialize();
}

void SysPoll(void)
{
    HalCommonPoll();
    ProtocolPoll();
    W25Q64Poll();
}

