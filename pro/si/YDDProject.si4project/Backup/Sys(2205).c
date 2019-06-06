#include "Sys.h"
#include "Protocol.h"
#include "W25Q64.h"
#include "Keypad.h"

#define SYS_FIRMWARE_VERSION "1.0.0.1"
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
        if(record->num == 0xffff || record->size == 0xffffffff)
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
        HalFlashRead(addr, (uint8_t *)info, sizeof(SysDataInfo_t));
    }
}

void SysArgsClear(void)
{
    SysDataRecord_t record;
    record.num = 0;
    record.size = 0;
    SysArgsSetRecord(&record);
}

void SysCollectArgsGet(SysCollectArgs_t *args)
{
    if(args)
    {
        HalFlashRead(HAL_DEVICE_ARGS_ADDR, (uint8_t *)args, sizeof(SysCollectArgs_t));
        if(args->signalThreshold == 0xffff ||
            args->runTime == 0xffff ||
            args->intensityAlarm == 0xffff ||
            args->ringAlarm == 0xffff)
        {
            args->signalThreshold = 150;
            args->runTime = 120;
            args->intensityAlarm = 500;
            args->ringAlarm = 200;
            HalFlashWrite(HAL_DEVICE_ARGS_ADDR, (const uint8_t *)args, sizeof(SysCollectArgs_t));
        }
    }
}

void SysCollectArgsSet(SysCollectArgs_t *args)
{
    if(args)
    {
        HalFlashWrite(HAL_DEVICE_ARGS_ADDR, (const uint8_t *)args, sizeof(SysCollectArgs_t));
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
extern void TestCollectInit(void);

static void startupLog(void)
{
    SysDataRecord_t record;
    SysCollectArgs_t args;
    
    SysArgsSetRecord(&record);
    SysCollectArgsGet(&args);
    printf("\r\n-----------------------------------------------------------\r\n");
    printf("--Firmware version:%s\r\n", SYS_FIRMWARE_VERSION);
    printf("--Compile date:%s %s\r\n", __DATE__, __TIME__);
    printf("--Data record: num = %d, size = %d\r\n", record.num, record.size);
    printf("--XHFZ = %d, CSSJ = %d, QDBJ = %d, ZLBJ = %d\r\n", args.signalThreshold, args.runTime, 
                                                                args.intensityAlarm, args.ringAlarm);
    printf("-----------------------------------------------------------\r\n");
}

void SysInitalize(void)
{
    int err;
    HalCommonInitialize();
    ProtocolInitialize();
    err = W25Q64Initialize();
    //TestCollectInit();
    KeypadInit();
    printf("W25Q64 init %s\r\n", err < 0 ? "failed" : "success");

    startupLog();
}

extern void TestCollectPoll(void);
void SysPoll(void)
{
    HalCommonPoll();
    ProtocolPoll();
    W25Q64Poll();
    //TestCollectPoll();
    KeypadPoll();
}

