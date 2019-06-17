#include "Sys.h"
#include "Protocol.h"
#include "W25Q64.h"
#include "YDD.h"
#include "Display.h"

#define SYS_FIRMWARE_VERSION "1.0.0.1"

uint8_t SysPowerPercent(void)
{
    int16_t max = 2619, min = 2234;
    int8_t percent = (int8_t)((HalADCGetValue(4) - min) * 100 / (max - min));
    if (percent > 100)
    {
        percent = 100;
    }
    else if(percent < 0)
    {
        percent = 0;
    }
    return (uint8_t)percent;
    //return 87;
}

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
        Syslog("num = %d, size = %d", record->num, record->size);
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
        Syslog("id = %d, threshold = %d, times = %d, size = %d, address = %d", id, info->threshold, 
                                                                    info->times, info->size, info->startAddr);
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

void SysSignalThresholdSet(uint16_t value)
{
    //soft step 50, from 50~300
    uint16_t daValue = (value / 50) * HAL_DAC_STEP_VALUE + HAL_DAC_BASE_VALUE;
    HalDACSetValue(daValue);
}

void SysBeepEnable(bool enable)
{
    HalBeepEnable(enable);
}

void SysArgsClear(void)
{
    SysDataRecord_t record;

    Syslog("");
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
            args->beep = 1;
            args->brightness = 50;
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
        Syslog("threshold = %d, runTime = %d, alarm = %d, ring = %d", args->signalThreshold, args->runTime, 
                                                                args->intensityAlarm, args->ringAlarm);
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

static void startupInit(void)
{
    SysDataRecord_t record;
    SysCollectArgs_t args;

    SysArgsGetRecord(&record);
    SysCollectArgsGet(&args);
    HalBeepEnable(args.beep);
    SysSignalThresholdSet(args.signalThreshold);
    DisplayBrightnessSet(args.brightness);

    //log
    printf("\r\n-----------------------------------------------------------\r\n");
    printf("--Firmware version:%s\r\n", SYS_FIRMWARE_VERSION);
    printf("--Compile date:%s %s\r\n", __DATE__, __TIME__);
    printf("--Data record: num = %d, size = %d\r\n", record.num, record.size);
    printf("--XHFZ = %d, CSSJ = %d, QDBJ = %d, ZLBJ = %d\r\n", args.signalThreshold, args.runTime, 
                                                                args.intensityAlarm, args.ringAlarm);
    SysDateTime_t *time = SysDateTime();
    printf("--Now: %d-%02d-%02d %02d:%02d:%02d\r\n", time->year, time->month, time->day, 
                                          time->hour, time->minute, time->second);
    printf("-----------------------------------------------------------\r\n");
    
    HalBeepSet(100);
}

void SysReboot(void)
{
    Syslog("");
    HalCommonReboot();
}

void SysInitalize(void)
{
    uint16_t errcode;
    errcode = HalCommonInitialize();
    ProtocolInitialize();
    printf(".....Hardware init....\n");
    errcode |= W25Q64Initialize();
    DisplayInitialize();
    printf("errcode = %d\r\n", errcode);
    YDDInitialize(errcode);
    startupInit();
}

extern void TestCollectPoll(void);
void SysPoll(void)
{
    HalCommonPoll();
    ProtocolPoll();
    W25Q64Poll();
    DisplayPoll();
    //TestCollectPoll();
    YDDPoll();
}

