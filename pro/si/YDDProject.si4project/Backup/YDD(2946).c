#include "YDD.h"
#include "Keypad.h"
#include "DataCollect.h"
#include "Display.h"

#define YDD_LCD_CD_X   138
#define YDD_LCD_CD_Y   72
#define YDD_LCD_XHFZ_X 327
#define YDD_LCD_XHFZ_Y YDD_LCD_CD_Y
#define YDD_LCD_CSSJ_X YDD_LCD_CD_X
#define YDD_LCD_CSSJ_Y (YDD_LCD_CD_Y + 40)
#define YDD_LCD_QDBJ_X YDD_LCD_XHFZ_X
#define YDD_LCD_QDBJ_Y YDD_LCD_CSSJ_Y
#define YDD_LCD_BEEP_X YDD_LCD_CD_X
#define YDD_LCD_BEEP_Y (YDD_LCD_CSSJ_Y + 40)
#define YDD_LCD_ZLBJ_X YDD_LCD_XHFZ_X
#define YDD_LCD_ZLBJ_Y YDD_LCD_BEEP_Y

typedef enum
{
    YDD_KEY_VALUE_F2 = 0,   //F2
    YDD_KEY_VALUE_CONFIG,   //设置
    YDD_KEY_VALUE_PAGEDOWN, //翻页
    YDD_KEY_VALUE_F1,       //F1
    YDD_KEY_VALUE_TOGGLE,   //切换
    YDD_KEY_VALUE_START,    //启动
}YDDKeyValue_t;

static DataCollect_st *g_dataCollect;
static SysPictureID_t g_curPictureId = SYS_PICTURE_DETAILS_ID;
static uint8_t g_menuid = 0;

static void showSettings(void);
static void showDetails(void);

static void showDate(void)
{
}

static void showTimeAndPower(bool force)
{
    static uint32_t oldTime = 0;
    SysDisplayPosition_t pos;
    uint16_t voltage;
    
    if(SysTimeHasPast(oldTime, 5000) || force)
    {
        char buff[5] = {0};
        DisplayDateTimeUpdate(); //时间显示
        
        voltage = HalADCGetValue(4); //取电量值
        DisplayPowerPercent(65);
        sprintf(buff, "%02d%%", 65);
        pos.x = 420;
        pos.y = 8;
        DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
        oldTime = SysTime();
    }
}

static void displayPictureSet(SysPictureID_t pid)
{
    DisplayPictureShow(pid);
    switch (pid)
    {
    case SYS_PICTURE_DETAILS_ID:
        showDetails();
    break;
    case SYS_PICTURE_SETTING_ID:
        showSettings();
        DisplaySettingsSelect(0, DISPLAY_COLOR_NAVY);
        g_menuid  = 0;
    break;
    case SYS_PICTURE_CONTACT_ID:
    break;
    case SYS_PICTURE_DATE_ID:
        showDate();
    break;
    case SYS_PICTURE_CLEAR_ID:
    break;
    case SYS_PICTURE_CAPTURE_ID:
        // TODO:  capture value
    break;
    default:
    break;
    }
    showTimeAndPower(true);
    g_curPictureId = pid;
}

static void configHandle(void)
{
    //20S、60S、120S、240S、480S、600S、800S、999S
    static uint8_t cdnum = 2;
    SysDisplayPosition_t pos;
    char buff[16];
    pos.x = YDD_LCD_CD_X;
    pos.y = YDD_LCD_CD_Y;
    buff[0] = '\0';
    sprintf(buff, "%d", cdnum++);
    SysDisplayPosition_t bottom;
    bottom.x = YDD_LCD_CD_X + 12 * strlen(buff);
    bottom.y = YDD_LCD_CD_Y + 24;
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &pos, &bottom);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
}

static void yddKeyEventHandle(uint8_t keyval, KeyStatus_t status, bool longpress, uint32_t lastTime)
{
    SysDataRecord_t record;
    
    if(lastTime > 20)
    {
        switch ((YDDKeyValue_t)keyval)
        {
            case YDD_KEY_VALUE_CONFIG:
                Syslog("YDD_KEY_VALUE_CONFIG");
                
                if(g_curPictureId == SYS_PICTURE_SETTING_ID)
                {
                    configHandle();
                }
                break;
            case YDD_KEY_VALUE_PAGEDOWN:
                Syslog("YDD_KEY_VALUE_PAGEDOWN");
                if(status == KEY_STATUS_RELEASE)
                {
                    if(g_curPictureId >= SYS_PICTURE_DETAILS_ID && g_curPictureId <= SYS_PICTURE_CONTACT_ID)
                    {
                        g_curPictureId++;
                        if(g_curPictureId > SYS_PICTURE_CONTACT_ID)
                        {
                            g_curPictureId = SYS_PICTURE_DETAILS_ID;
                        }
                        displayPictureSet(g_curPictureId);
                    }
                }
                break;
            case YDD_KEY_VALUE_START:
                Syslog("YDD_KEY_VALUE_START");
                if(status == KEY_STATUS_RELEASE)
                {
                    SysArgsGetRecord(&record);
                    DataCollectStart(record.size, g_dataCollect);
                    displayPictureSet(SYS_PICTURE_CAPTURE_ID);
                }
                break;
            case YDD_KEY_VALUE_TOGGLE:
                Syslog("YDD_KEY_VALUE_TOGGLE");
                if(g_curPictureId == SYS_PICTURE_SETTING_ID)
                {
                    DisplaySettingsSelect(g_menuid++, DISPLAY_COLOR_BOTTOM);//unselect front
                    if(g_menuid == 8)
                    {
                        g_menuid = 0;
                    }
                    DisplaySettingsSelect(g_menuid, DISPLAY_COLOR_NAVY);  //select
                }else if(g_curPictureId == SYS_PICTURE_CAPTURE_ID)
                {
                    DataCollectStop(g_dataCollect);
                }
                break;
            default:
                break;
        }
    }
}

static void showSysSettings(void)
{
    SysDisplayPosition_t pos;
    char buff[32];
    /*
    uint8_t percent;
    uint16_t voltage;
    
    DisplayDateTimeUpdate(); //时间显示
    voltage = HalADCGetValue(4); //取电量值
    // TODO: calc percent
    percent = 88;//test
    DisplayPowerPercent(percent); //电量显示
    */
    /*测点数*/
    SysDataRecord_t record;
    SysArgsGetRecord(&record);
    //138,72
    pos.x = YDD_LCD_CD_X;
    pos.y = YDD_LCD_CD_Y;
    buff[0] = '\0';
    sprintf(buff, "%d", record.num);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*测试时间*/
    SysCollectArgs_t args;
    SysCollectArgsGet(&args);
    pos.y = YDD_LCD_CSSJ_Y;
    buff[0] = '\0';
    sprintf(buff, "%d", args.runTime);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*蜂鸣器*/
    pos.y = YDD_LCD_BEEP_Y;
    buff[0] = '\0';
    sprintf(buff, "%s", args.beep ? "开":"关");
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*信号阈值*/
    pos.x = YDD_LCD_XHFZ_X;
    pos.y = YDD_LCD_XHFZ_Y;
    buff[0] = '\0';
    sprintf(buff, "%d", args.signalThreshold);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);

    /*强度报警值*/
    pos.y = YDD_LCD_QDBJ_Y;
    buff[0] = '\0';
    sprintf(buff, "%d", args.intensityAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
    
    /*振铃报警值*/
    pos.y = YDD_LCD_ZLBJ_Y;
    buff[0] = '\0';
    sprintf(buff, "%d", args.ringAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
}

static void showDetails(void)
{    
    //DisplayPictureShow(SYS_PICTURE_DETAILS_ID); //切换背景图
    showSysSettings();
}

static void showSettings(void)
{
//    char buff[32];
//    SysDisplayPosition_t pos;
    
    //DisplayPictureShow(SYS_PICTURE_SETTING_ID); //切换背景图
    showSysSettings();
}

/*
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
*/

void YDDInitialize(void)
{
    KeypadInit(yddKeyEventHandle);
    DisplayInitialize();
    g_dataCollect = DataCollectCreate();
    displayPictureSet(SYS_PICTURE_DETAILS_ID);
    showTimeAndPower(true);
}

void YDDPoll(void)
{
    KeypadPoll();
    DisplayPoll();
    DataCollectPoll(g_dataCollect);
    showTimeAndPower(false);
}

