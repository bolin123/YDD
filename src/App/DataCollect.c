#include "DataCollect.h"
#include "W25Q64.h"

#define DATA_COLLECT_INTERVAL (SYS_TIME_COUNT_1S / DATA_COLLECT_PERIOD_NUM)
//#define DATA_COLLECT_FLASH_BUFF_LEN (DATA_COLLECT_TIME_MIN * sizeof(DataCollectContext_t) * HAL_ADC_CH_NUM)

//static uint8_t g_flashBuff[DATA_COLLECT_FLASH_BUFF_LEN];
static DataCollectContext_t g_dcContext[DATA_COLLECT_TIME_MIN][HAL_ADC_CH_NUM];
static uint8_t g_contextCount = 0;
static volatile uint32_t g_freqCount[HAL_ADC_CH_NUM];
static DataCollectEventHandle_t g_dataEventHandle = NULL;
static uint16_t g_pressBuff[DATA_COLLECT_PERIOD_NUM]; 
static void eventEmitt(DataCollectEvent_t event, uint8_t chn, void *args)
{
    if(g_dataEventHandle != NULL)
    {
        g_dataEventHandle(event, chn, args);
    }
}

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

static uint16_t calcPressureValue(uint16_t *data, uint16_t len)
{
    uint16_t i;
    uint32_t result = 0;
    uint32_t count = 0, average = 0;

    for(i = 0; i < len; i++)
    {
        count += data[i];
    }

    average = (count / len);
    result = average * 10 * 25 / 4096; //157.5 = 4096 /25MPa ,10倍存储小数点后一位，显示会自动除10
    return result;
}

static uint16_t amplitudeExchange(uint16_t value)
{
    uint32_t result = 0;
    uint32_t mv = value * 3300 / 4096;

    if(mv < 1500)
    {
        result = 0;
    }
    else
    {
        result = mv - 1500;
    }
    return result;
}
#if 0
static uint16_t  calcRawdataValue(uint8_t ch, uint16_t (*rawData)[HAL_ADC_CH_NUM], uint16_t len)
{
    uint8_t i;
    uint32_t result = 0;
    uint32_t count = 0;

    for(i = 0; i < len; i++)
    {
        count += rawData[i][ch];
    }

    result = (count / len);
    
    uint32_t mv = result * 3300 / 4096;
    /*
    if(ch < 3)
    {
        if(result > 1861)
        {
            result -= 1861; //1861 = 1.5/3.3*4096;
        }
        else
        {
            result = 0;
        }
    }*/
    if(ch < 3)
    {
        if(mv < 1500)
        {
            result = 0;
        }
        else
        {
            result = mv - 1500;
        }
    }
    else
    {
        result = result * 10 / 157.5; //157.5 = 4096 /26MPa
    }
    return (uint16_t)result;
}
#endif

static void collectionHandle(DataCollect_st *dcollect)
{
    uint8_t ch;
    /*1秒取10个数*/
    if(dcollect != NULL && dcollect->start && SysTimeHasPast(dcollect->lastCollectTime, DATA_COLLECT_INTERVAL)) 
    {
    HalInterruptSet(false);// disable interrupt
        //HalADCGetData(dcollect->periodData[dcollect->periodCount++]);
        //dcollect->periodData[dcollect->periodCount++][3] = HalADCGetCollectValue(3);
        g_pressBuff[dcollect->periodCount++] = HalADCGetCollectValue(3);
        
        if(dcollect->periodCount >= DATA_COLLECT_PERIOD_NUM) //生成一个测点
        {   
            printf("------------------------------------------\r\n");
            for(ch = 0; ch < HAL_ADC_CH_NUM; ch++)
            {
                if(ch < 3)
                {
                    g_dcContext[g_contextCount][ch].amplitude = amplitudeExchange(HalADCGetCollectValue(ch));
                }
                else
                {
                    g_dcContext[g_contextCount][ch].amplitude = calcPressureValue(g_pressBuff, dcollect->periodCount);
                }
                // freq / (SysTime() - dcollect->periodStartTime)
                //float freq = (float)g_freqCount[ch] / (SysTime() - dcollect->periodStartTime);
                g_dcContext[g_contextCount][ch].frequency = (g_freqCount[ch] * 10)/ (SysTime() - dcollect->periodStartTime); 
                //g_dcContext[g_contextCount][ch].frequency = g_freqCount[ch];
                freqCountClear(ch);

                eventEmitt(DATA_COLLECT_EVENT_UPDATE, ch, &g_dcContext[g_contextCount][ch]);
                printf("capture %d: CH%d, amplitude = %d, frequency = %d\r\n", dcollect->dataCount, ch, 
                                                                        g_dcContext[g_contextCount][ch].amplitude, 
                                                                        g_dcContext[g_contextCount][ch].frequency);
            }
            HalADCMaxvalueClear();
            g_contextCount++;
            if(g_contextCount >= DATA_COLLECT_TIME_MIN) //20个点位存一次flash
            {
                printf("flash update\r\n");
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
                HalBeepSet(200);
            }
            dcollect->periodCount = 0;
            dcollect->periodStartTime = SysTime();
        }
    
        dcollect->lastCollectTime = SysTime();
    HalInterruptSet(true);// enable interrupt
    }
}

void DataCollectStop(DataCollect_st *dcollect)
{    
    Syslog("");
    if(dcollect != NULL)
    {
        dcollect->start = false;
        eventEmitt(DATA_COLLECT_EVENT_DONE, 0, NULL);
    }
}

bool DataCollectIsStart(DataCollect_st *dcollect)
{
    return dcollect->start;
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
        dcollect->lastCollectTime = SysTime();
        dcollect->periodStartTime = SysTime();
        g_contextCount = 0;
    HalInterruptSet(false);// disable interrupt
        HalADCMaxvalueClear();
        memset((void *)g_freqCount, 0, sizeof(g_freqCount)); //clear freqcount
    HalInterruptSet(true);// disable interrupt
    
        Syslog("Start capture, threshold:%d, count:%d, %d-%02d-%02d %02d:%02d:%02d", dcollect->threshold, dcollect->collectCount,
                                                        dcollect->collectDate.year, dcollect->collectDate.month,
                                                        dcollect->collectDate.day, dcollect->collectDate.hour, 
                                                        dcollect->collectDate.minute, dcollect->collectDate.second);
    }
}

static DataCollect_st g_dataCollect;
DataCollect_st *DataCollectCreate(void)
{
    g_dataCollect.start = false;
    g_dataCollect.periodCount = 0;
    return &g_dataCollect;
}

/*
static void testFreq(void)
{
    uint8_t i;
    static uint32_t lastTime;

    if(SysTimeHasPast(lastTime, 1000))
    {
    
    HalInterruptSet(false);// disable interrupt
        for(i = 0; i < HAL_ADC_CH_NUM; i++)
        {
            printf("CH[%d] = %dHz\r\n", i+1, g_freqCount[i]);
            freqCountClear(i);
        }
        
    HalInterruptSet(true);// disable interrupt
        lastTime = SysTime();
    }
}
*/

void DataCollectInitialize(DataCollectEventHandle_t eventHandle)
{
    g_dataEventHandle = eventHandle;
}

void DataCollectPoll(DataCollect_st *dcollect)
{
    collectionHandle(dcollect);
    //testFreq();
}

