#include "YDD.h"
#include "Keypad.h"
#include "DataCollect.h"

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
static DataCollect_st *g_dataCollect;

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



void YDDStartCollect(void)
{
    
}

void YDDInitialize(void)
{
    KeypadInit(yddKeyEventHandle);
    g_dataCollect = DataCollectCreate();
}

void YDDPoll(void)
{
    KeypadPoll();
    DataCollectPoll(g_dataCollect);
}

