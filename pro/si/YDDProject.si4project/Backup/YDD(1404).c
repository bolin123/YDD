#include "YDD.h"
#include "Keypad.h"

#define YDD_ADC_COLLECT_NUM 10

typedef enum
{
    YDD_KEY_VALUE_F2 = 0,   //F2
    YDD_KEY_VALUE_CONFIG,   //ÉèÖÃ
    YDD_KEY_VALUE_PAGEDOWN, //·­Ò³
    YDD_KEY_VALUE_START,    //Æô¶¯
    YDD_KEY_VALUE_TOGGLE,   //ÇÐ»»
    YDD_KEY_VALUE_F1,       //F1
}YDDKeyValue_t;

typedef enum
{
    YDD_MODE_WELCOME = 0,
    YDD_MODE_1,
    YDD_MODE_2,
    YDD_MODE_3,
    YDD_MODE_COLLECTING,
}YDDMode_t;

static YDDMode_t g_mode = YDD_MODE_WELCOME;

static uint8_t g_collectCh = 0;
static uint16_t g_collectData[YDD_ADC_COLLECT_NUM][HAL_ADC_CH_NUM];

static void yddKeyEventHandle(uint8_t keyval, KeyStatus_t status, bool longpress, uint32_t lastTime)
{
    if(lastTime > 20)
    {
        switch ((YDDKeyValue_t)keyval)
        {
            case YDD_KEY_VALUE_CONFIG:
                break;
            case YDD_KEY_VALUE_PAGEDOWN:
                break;
            case YDD_KEY_VALUE_START:
                break;
            case YDD_KEY_VALUE_TOGGLE:
                break;
            default:
                break;
        }
    }
}

static void dataCollection(void)
{
    static uint32_t lastCollectTime = 0;
    uint8_t i, j;
    uint32_t count = 0;

    if(g_mode == YDD_MODE_COLLECTING)
    {
        if(SysTimeHasPast(lastCollectTime, 100))
        {
            HalADCGetData(g_collectData[g_collectCh++]);
            lastCollectTime = SysTime();
            if(YDD_ADC_COLLECT_NUM <= g_collectCh)
            {
                for(i = 0; i < HAL_ADC_CH_NUM; i++)
                {
                    count = 0;
                    for(j = 0; j < YDD_ADC_COLLECT_NUM; j++)
                    {
                        count += g_collectData[j][i];
                    }
                    Syslog("ch[%d] = %d", i + 1, count / YDD_ADC_COLLECT_NUM);
                }
                
                g_collectCh = 0;
            }
        }
    }
    
}

void YDDInitialize(void)
{
    KeypadInit(yddKeyEventHandle);
}

void YDDPoll(void)
{
    KeypadPoll();
    dataCollection();
}

