#ifndef DATA_COLLECT_H
#define DATA_COLLECT_H

#include "Sys.h"

#define DATA_COLLECT_PERIOD_NUM 10
#define DATA_COLLECT_TIME_MIN 20 //最小测试时间 20s

typedef struct
{
    uint16_t amplitude;
    uint16_t frequency;
}DataCollectContext_t;

typedef struct 
{
    bool start;
    uint8_t periodCount;   //周期计数（1秒为一个周期）
    uint16_t threshold; //阈值
    uint16_t periodData[DATA_COLLECT_PERIOD_NUM][HAL_ADC_CH_NUM]; //1秒钟采集的数据，然后取平均值
    uint16_t dataCount;       //当前采集到的数据点数
    uint16_t collectCount;    //总共需要采集的点数
    uint32_t flashAddress;    //保存数据的flash地址
    uint32_t lastCollectTime; //上一次采集的时间
    SysDateTime_t collectDate;//开始日期
}DataCollect_st;

DataCollect_st *DataCollectCreate(void);
void DataCollectStop(DataCollect_st *dcollect);
void DataCollectStart(uint32_t address, DataCollect_st *dcollect);
void DataCollectPoll(DataCollect_st *dcollect);

#endif

