#include "DataCollect.h"
#include "W25Q64.h"

#define DATA_COLLECT_INTERVAL (SYS_TIME_COUNT_1S / DATA_COLLECT_PERIOD_NUM)
//#define DATA_COLLECT_FLASH_BUFF_LEN (DATA_COLLECT_TIME_MIN * sizeof(DataCollectContext_t) * HAL_ADC_CH_NUM)

//static uint8_t g_flashBuff[DATA_COLLECT_FLASH_BUFF_LEN];
static DataCollectContext_t g_dcContext[DATA_COLLECT_TIME_MIN][HAL_ADC_CH_NUM];
static uint8_t g_contextCount = 0;
static volatile uint32_t g_freqCount[HAL_ADC_CH_NUM];

void DataCollectFreqCount(uint8_t ch)
{
    g_freqCount[ch]++;
}

static void freqCountClear(uint8_t ch)
{
    //HalInterruptSet(false);// disable interrupt
    g_freqCount[ch] = 0;
    //HalInterruptSet(true);// enable interrupt
}

static uint16_t  calcRawdataValue(uint8_t ch, uint16_t (*rawData)[HAL_ADC_CH_NUM], uint16_t len)
{
    uint8_t i;
    uint32_t count = 0;

    for(i = 0; i < len; i++)
    {
        count += rawData[i][ch];
    }

    return (count / DATA_COLLECT_PERIOD_NUM);
}

static void collectionHandle(DataCollect_st *dcollect)
{
    uint8_t ch;
    /*1秒取10个数*/
    if(dcollect != NULL && dcollect->start && SysTimeHasPast(dcollect->lastCollectTime, DATA_COLLECT_INTERVAL)) 
    {
    HalInterruptSet(false);// disable interrupt
        HalADCGetData(dcollect->periodData[dcollect->periodCount++]);
        
        if(dcollect->periodCount >= DATA_COLLECT_PERIOD_NUM) //生成一个测点
        {   
            for(ch = 0; ch < HAL_ADC_CH_NUM; ch++)
            {
                //dcollect->dataBuff[ch][dcollect->dataCount] = calcRawdataValue(ch, dcollect->periodData, dcollect->periodCount);
                g_dcContext[g_contextCount][ch].amplitude = calcRawdataValue(ch, dcollect->periodData, dcollect->periodCount);
                g_dcContext[g_contextCount][ch].frequency = g_freqCount[ch] / 1000;// TODO:
                freqCountClear(ch);
            }
            g_contextCount++;
            if(g_contextCount >= DATA_COLLECT_TIME_MIN) //20个点位存一次flash
            {
                W25Q64Write((uint8_t *)g_dcContext, dcollect->flashAddress, sizeof(g_dcContext));
                dcollect->flashAddress += sizeof(g_dcContext);
                g_contextCount = 0;
            }
            dcollect->dataCount++;
            //all done 
            if(dcollect->dataCount >= dcollect->collectCount) //采集完成
            {
                //...save data
                if(g_contextCount) //last data save
                {
                    uint32_t length = sizeof(DataCollectContext_t) * HAL_ADC_CH_NUM * g_contextCount;
                    W25Q64Write((uint8_t *)g_dcContext, dcollect->flashAddress, length);
                }
                
                //...update record & args
                SysDataRecord_t record;
                SysDataInfo_t info;
                
                SysArgsGetRecord(&record);
                info.size = dcollect->collectCount * HAL_ADC_CH_NUM * sizeof(DataCollectContext_t);
                info.threshold = dcollect->threshold;
                info.times = dcollect->collectCount;
                info.startAddr = record.size;
                info.date = dcollect->collectDate;
                SysArgsSetPointInfo(record.num + 1, &info);
                
                record.num++;
                record.size += dcollect->collectCount * HAL_ADC_CH_NUM * sizeof(DataCollectContext_t);
                SysArgsSetRecord(&record);

                DataCollectStop(dcollect);
            }
            dcollect->periodCount = 0;
        }
    
        dcollect->lastCollectTime = SysTime();
    HalInterruptSet(true);// enable interrupt
    }
}

void DataCollectStop(DataCollect_st *dcollect)
{    
    if(dcollect != NULL)
    {
        dcollect->start = false;
    }
}

void DataCollectStart(uint32_t address, DataCollect_st *dcollect)
{
    SysCollectArgs_t args;

    if(dcollect != NULL)
    {
        SysCollectArgsGet(&args);

        dcollect->flashAddress = address;
        dcollect->periodCount  = 0;
        dcollect->threshold = args.signalThreshold;
        dcollect->collectCount = args.runTime;        
        dcollect->dataCount = 0;
        dcollect->start = true;
        dcollect->collectDate = *SysDateTime();
        g_contextCount = 0;
        memset((void *)g_freqCount, 0, sizeof(g_freqCount));
    }
}

static DataCollect_st g_dataCollect;
DataCollect_st *DataCollectCreate(void)
{
    g_dataCollect.start = false;
    g_dataCollect.periodCount = 0;
    return &g_dataCollect;
}

static void testFreq(void)
{
    uint8_t i;
    static uint32_t lastTime;

    if(SysTimeHasPast(lastTime, 1000))
    {
    
    HalInterruptSet(false);// disable interrupt
        for(i = 0; i < 3; i++)
        {
            printf("CH[%d] = %dHz\r\n", i+1, g_freqCount[i] / 1000);
            freqCountClear(i);
        }
        
    HalInterruptSet(true);// disable interrupt
        lastTime = SysTime();
    }
}

void DataCollectPoll(DataCollect_st *dcollect)
{
    collectionHandle(dcollect);
    testFreq();
}

