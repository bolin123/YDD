#include "YDD.h"
#include "Keypad.h"
#include "DataCollect.h"
#include "Display.h"

typedef enum
{
    YDD_KEY_VALUE_F2 = 0,   //F2
    YDD_KEY_VALUE_CONFIG,   //ÉèÖÃ
    YDD_KEY_VALUE_PAGEDOWN, //·­Ò³
    YDD_KEY_VALUE_F1,       //F1
    YDD_KEY_VALUE_TOGGLE,   //ÇÐ»»
    YDD_KEY_VALUE_START,    //Æô¶¯
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
                Syslog("YDD_KEY_VALUE_CONFIG");
                break;
            case YDD_KEY_VALUE_PAGEDOWN:
                Syslog("YDD_KEY_VALUE_PAGEDOWN");
                break;
            case YDD_KEY_VALUE_START:
                Syslog("YDD_KEY_VALUE_START");
                SysDataRecord_t record;
                SysArgsGetRecord(&record);
                DataCollectStart(record.size, g_dataCollect);
                g_mode = YDD_MODE_COLLECTING;
                break;
            case YDD_KEY_VALUE_TOGGLE:
                Syslog("YDD_KEY_VALUE_TOGGLE");
                DataCollectStop(g_dataCollect);
                g_mode = YDD_MODE_WELCOME;
                break;
            default:
                //DisplayClear();
                break;
        }
    }
}

void YDDInitialize(void)
{
    KeypadInit(yddKeyEventHandle);
    DisplayInitialize();
    g_dataCollect = DataCollectCreate();
}

void YDDPoll(void)
{
    KeypadPoll();
    DisplayPoll();
    DataCollectPoll(g_dataCollect);
}

