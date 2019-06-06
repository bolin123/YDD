#include "YDD.h"
#include "Keypad.h"
#include "DataCollect.h"
#include "Display.h"

typedef enum
{
    YDD_KEY_VALUE_F2 = 0,   //F2
    YDD_KEY_VALUE_CONFIG,   //设置
    YDD_KEY_VALUE_PAGEDOWN, //翻页
    YDD_KEY_VALUE_F1,       //F1
    YDD_KEY_VALUE_TOGGLE,   //切换
    YDD_KEY_VALUE_START,    //启动
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
                DisplayPictureShow(SYS_PICTURE_CONTACT_ID);
                break;
        }
    }
}

static void displayUpdate(void)
{
    static uint32_t lastTime;
    static uint8_t powerpercent = 90;
    SysDisplayPosition_t pos;
    char buff[5] = {0};
    pos.x = 350;
    pos.y = 8;
    
    if(g_mode == YDD_MODE_WELCOME)
    {
        DisplayPictureShow(SYS_PICTURE_SETTING_ID);
        DisplayDateTimeUpdate();
        DisplayPowerPercent(powerpercent);
        DiplayStringPrint("90%%", strlen("90%%"), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
        g_mode = YDD_MODE_1;
        lastTime = SysTime();
    }

    if(SysTimeHasPast(lastTime, 5000))
    {
        powerpercent -= 10;
        sprintf(buff, "%02d%%", powerpercent);
        DisplayPowerPercent(powerpercent);
        DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
        if(powerpercent == 0)
        {
            powerpercent = 100;
        }
        
        lastTime = SysTime();
    }
}

void YDDInitialize(void)
{
    char *str = "正在初始化...";
    KeypadInit(yddKeyEventHandle);
    DisplayInitialize();

    SysDisplayPosition_t pos;
    pos.x = 180;
    pos.y = 220;
    DiplayStringPrint(str, strlen(str), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
    g_dataCollect = DataCollectCreate();
}

void YDDPoll(void)
{
    KeypadPoll();
    DisplayPoll();
    DataCollectPoll(g_dataCollect);
    displayUpdate();
}

