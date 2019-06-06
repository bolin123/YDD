#include "YDD.h"
#include "Keypad.h"
#include "DataCollect.h"
#include "Display.h"

typedef enum
{
    YDD_KEY_VALUE_F2 = 0,   //F2
    YDD_KEY_VALUE_CONFIG,   //����
    YDD_KEY_VALUE_PAGEDOWN, //��ҳ
    YDD_KEY_VALUE_F1,       //F1
    YDD_KEY_VALUE_TOGGLE,   //�л�
    YDD_KEY_VALUE_START,    //����
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
    static uint8_t picid = 1;
    static uint8_t menuid = 0;
    if(lastTime > 20)
    {
        switch ((YDDKeyValue_t)keyval)
        {
            case YDD_KEY_VALUE_CONFIG:
                Syslog("YDD_KEY_VALUE_CONFIG");
                break;
            case YDD_KEY_VALUE_PAGEDOWN:
                Syslog("YDD_KEY_VALUE_PAGEDOWN");
                DisplayPictureShow(picid++);
                if(picid == SYS_PICTURE_SETTING_ID)
                {
                    DisplaySettingsSelect(0);
                }
                if(picid == SYS_PICTURE_ID_COUNT)
                {
                    picid = 0;
                }
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
                DisplaySettingsSelect(menuid++);//unselect front
                if(menuid == 8)
                {
                    menuid = 0;
                }
                DisplaySettingsSelect(menuid);  //select
                g_mode = YDD_MODE_WELCOME;
                break;
            default:
            // TODO: test 
                
                break;
        }
    }
}

static void showSysSettings(void)
{
    SysDisplayPosition_t pos;
    char buff[32];
    uint8_t percent;
    uint16_t voltage;
    
    DisplayDateTimeUpdate(); //ʱ����ʾ
    voltage = HalADCGetValue(4); //ȡ����ֵ
    // TODO: calc percent
    percent = 88;//test
    DisplayPowerPercent(percent); //������ʾ

    /*�����*/
    SysDataRecord_t record;
    SysArgsGetRecord(&record);
    //138,72
    pos.x = 138;
    pos.y = 72;
    buff[0] = '\0';
    sprintf(buff, "%d", record.num);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*����ʱ��*/
    SysCollectArgs_t args;
    SysCollectArgsGet(&args);
    pos.y = 112;
    buff[0] = '\0';
    sprintf(buff, "%d", args.runTime);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*������*/
    pos.y = 152;
    buff[0] = '\0';
    sprintf(buff, "%s", args.beep ? "��":"��");
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*�ź���ֵ*/
    pos.x = 327;
    pos.y = 72;
    buff[0] = '\0';
    sprintf(buff, "%d", args.signalThreshold);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*ǿ�ȱ���ֵ*/
    pos.y = 112;
    buff[0] = '\0';
    sprintf(buff, "%d", args.intensityAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
    
    /*���屨��ֵ*/
    pos.y = 152;
    buff[0] = '\0';
    sprintf(buff, "%d", args.ringAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
}

static void showDetails(void)
{    
    DisplayPictureShow(SYS_PICTURE_DETAILS_ID); //�л�����ͼ
    showSysSettings();
}

static void showSettings(void)
{
//    char buff[32];
//    SysDisplayPosition_t pos;
    
    DisplayPictureShow(SYS_PICTURE_SETTING_ID); //�л�����ͼ
    showSysSettings();
}


static void displayUpdate(void)
{
    static uint32_t lastTime;
    static uint8_t powerpercent = 90;
    SysDisplayPosition_t pos;
    char buff[16] = {0};
    pos.x = 420;
    pos.y = 8;
    
    if(g_mode == YDD_MODE_WELCOME)
    {
        showDetails();
        g_mode = YDD_MODE_1;
        lastTime = SysTime();
    }

    if(SysTimeHasPast(lastTime, 2000))
    {
        powerpercent -= 10;
        DisplayPowerPercent(powerpercent);
        
        //sprintf(buff, "  %%", powerpercent);

        sprintf(buff, "%02d%%", powerpercent);
        DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
        
        
        if(powerpercent == 0)
        {
            powerpercent = 100;
            DisplayDateTimeUpdate();
        }
        
        lastTime = SysTime();
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
    displayUpdate();
}

