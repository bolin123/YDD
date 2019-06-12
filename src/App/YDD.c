#include "YDD.h"
#include "Keypad.h"
#include "DataCollect.h"
#include "Display.h"
#include "DisplayCoordinate.h"
#include "Menu/MenuSettings.h"
#include "Menu/MenuContact.h"
#include "Menu/MenuDetail.h"

typedef enum
{
    YDD_MENU_ID_DETAIL = 0,
    YDD_MENU_ID_SETTINGS,
    YDD_MENU_ID_CONTACTUS,
    YDD_MENU_ID_COUNT,
}YDDMenuId_t;

typedef struct
{
    void (*hide)(void);
    void (*keyHandle)(HalKeyValue_t key);
    void (*show)(void);
}YDDMenuHandle_t;

static DataCollect_st *g_dataCollect;

static YDDMenuId_t g_menuId = YDD_MENU_ID_DETAIL;
static YDDMenuHandle_t g_menuHandle[YDD_MENU_ID_COUNT];
static uint16_t g_lastCollectNum = 0;

static void updateTimeAndPower(void)
{
    static uint32_t oldTime = 0;
    
    if(SysTimeHasPast(oldTime, 10000))
    {
        Syslog("update");
        DisplayDateTimeUpdate(); //时间显示
        DisplayPowerPercent();
        oldTime = SysTime();
    }
}

#if 0

static void selectBox(bool selected, SysDisplayPosition_t *menuPos, uint8_t width, uint8_t height)
{
    SysDisplayPosition_t top, bottom;
    uint16_t color = selected ?  DISPLAY_COLOR_BLUE : DISPLAY_COLOR_BOTTOM;

    top.x = menuPos->x - 2;
    top.y = menuPos->y - 2;
    bottom.x = top.x + width;
    bottom.y = top.y + height;
    
    //DisplaySelectBox(color, &top, &bottom);
}


static void displayPictureSet(SysPictureID_t pid)
{
//    SysDisplayPosition_t top, bottom;

    DisplayPictureShow(pid);
    switch (pid)
    {
    case SYS_PICTURE_DETAILS_ID:
        showDetails();
    break;
    case SYS_PICTURE_SETTING_ID:
        showSettings();
        //DisplaySettingsSelect(0, DISPLAY_COLOR_BLUE);
        selectBox(true, &g_settingsPos[0], YDD_SELECT_SETTINGS_WIDTH, YDD_SELECT_SETTINGS_HEIGTH);
        g_settingMenuid  = YDD_SETTINGS_ITEMID_CSSJ;
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
    updateTimeAndPower(true);
    g_curPictureId = pid;
}

static void configHandle(void)
{
    //20S、60S、120S、240S、480S、600S、800S、999S
    static uint8_t cdnum = 2;
    SysDisplayPosition_t pos;
    char buff[16];
    pos.x = g_settingsPos[0].x;
    pos.y = g_settingsPos[0].y;
    buff[0] = '\0';
    sprintf(buff, "%d", cdnum++);
    SysDisplayPosition_t bottom;
    bottom.x = g_settingsPos[0].x + 12 * 4;
    bottom.y = g_settingsPos[0].y + 24;
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &pos, &bottom);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
}
#endif

static void idleDisplay(void)
{
    g_menuId = YDD_MENU_ID_DETAIL;
    MenuDetailShow();
    //updateTimeAndPower(true);
}

static void lastCollectNumDisplay(void)
{    
    char buff[4] = "";
    SysDisplayPosition_t pos, bottom;
    pos.x = 30;
    pos.y = 235;

    bottom.x = pos.x + 12 * 3;
    bottom.y = pos.y + 24;
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &pos, &bottom);
    
    sprintf(buff, "%03d", g_lastCollectNum);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_NORMAL);
}

static void collectDisplayShow(void)
{   

    DisplayPictureShow(SYS_PICTURE_CAPTURE_ID);
    DisplayDateTimeUpdate(); //时间显示
    DisplayPowerPercent();

    lastCollectNumDisplay();
}


static void yddKeyEventHandle(uint8_t keyval, KeyStatus_t status, bool longpress, uint32_t lastTime)
{
    SysDataRecord_t record;
    SysCollectArgs_t args;
    HalKeyValue_t value = (HalKeyValue_t)keyval;
    
    if(lastTime > 50)
    {
        switch (value)
        {
            case HAL_KEY_VALUE_CONFIG:
                Syslog("HAL_KEY_VALUE_CONFIG");
                
                if(!DataCollectIsStart(g_dataCollect))
                {
                    g_menuHandle[g_menuId].keyHandle(value);
                }
                break;
            case HAL_KEY_VALUE_PAGEDOWN:
                Syslog("HAL_KEY_VALUE_PAGEDOWN");
                if(status == KEY_STATUS_RELEASE && !DataCollectIsStart(g_dataCollect))
                {
                    g_menuHandle[g_menuId].hide();
                    g_menuId++;
                    if(g_menuId >= YDD_MENU_ID_COUNT)
                    {
                        g_menuId = YDD_MENU_ID_DETAIL;
                    }
                    g_menuHandle[g_menuId].show();
                    //updateTimeAndPower(true);
                }
                break;
            case HAL_KEY_VALUE_START:
                Syslog("HAL_KEY_VALUE_START");
                if(status == KEY_STATUS_RELEASE && !DataCollectIsStart(g_dataCollect))
                {
                    g_menuHandle[g_menuId].hide();
                    SysArgsGetRecord(&record);
                    SysCollectArgsGet(&args);
                    g_lastCollectNum = args.runTime;
                    DataCollectStart(record.size, g_dataCollect);
                    collectDisplayShow();
                }
                break;
            case HAL_KEY_VALUE_TOGGLE:
                Syslog("HAL_KEY_VALUE_TOGGLE");
                if(DataCollectIsStart(g_dataCollect)) //cancle
                {
                    DataCollectStop(g_dataCollect);
                    idleDisplay();
                }
                else
                {
                    g_menuHandle[g_menuId].keyHandle(value);
                }
                break;
            default:
                break;
        }
    }
}

#if 0
static void showDetails(void)
{
    char buff[10];
    uint8_t i = 0;

    /*测点数*/
    SysDataRecord_t record;
    SysArgsGetRecord(&record);
    buff[0] = '\0';
    sprintf(buff, "%d", record.num);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_detailsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    /*测试时间*/
    SysCollectArgs_t args;
    SysCollectArgsGet(&args);
    buff[0] = '\0';
    sprintf(buff, "%d", args.runTime);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_detailsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    /*蜂鸣器*/
    buff[0] = '\0';
    sprintf(buff, "%s", args.beep ? "开":"关");
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_detailsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    /*信号阈值*/
    buff[0] = '\0';
    sprintf(buff, "%d", args.signalThreshold);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_detailsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    /*强度报警值*/
    buff[0] = '\0';
    sprintf(buff, "%d", args.intensityAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_detailsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*振铃报警值*/
    buff[0] = '\0';
    sprintf(buff, "%d", args.ringAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_detailsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
}



static void showSettings(void)
{   
    char buff[10];
    uint8_t i = 0;

    /*测试时间*/
    SysCollectArgs_t args;
    SysCollectArgsGet(&args);
    buff[0] = '\0';
    sprintf(buff, "%d", args.runTime);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*强度报警值*/
    buff[0] = '\0';
    sprintf(buff, "%d", args.intensityAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*信号阈值*/
    buff[0] = '\0';
    sprintf(buff, "%d", args.signalThreshold);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*振铃报警值*/
    buff[0] = '\0';
    sprintf(buff, "%d", args.ringAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    /*蜂鸣器*/
    buff[0] = '\0';
    sprintf(buff, "%s", args.beep ? "开":"关");
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
}
#endif
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

static void updateCollectDisplay(uint8_t chn, DataCollectContext_t *value)
{
    SysDisplayPosition_t top, bottom;
    char buff[8] = "";
    
    top.x = g_capturePos[chn].vltg.x;
    top.y = g_capturePos[chn].vltg.y;

    bottom.x = g_capturePos[chn].vltg.x + 12 * 4;
    bottom.y = g_capturePos[chn].vltg.y + 24;
    
    sprintf(buff, "%d", value->amplitude);
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &top, &bottom);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &top, DISPLAY_CHAR_SIZE_NORMAL);

    top.x = g_capturePos[chn].freq.x;
    top.y = g_capturePos[chn].freq.y;

    bottom.x = g_capturePos[chn].freq.x + 12 * 4;
    bottom.y = g_capturePos[chn].freq.y + 24;

    uint16_t integer, decimal;
    buff[0] = '\0';
    integer = value->frequency / 10;
    decimal = value->frequency % 10;
    sprintf(buff, "%d.%d", integer, decimal);
    //sprintf(buff, "%d", value->frequency);
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &top, &bottom);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &top, DISPLAY_CHAR_SIZE_NORMAL);

    if(chn == (HAL_ADC_CH_NUM - 1))
    {
        g_lastCollectNum--;
        lastCollectNumDisplay();
    }
}

static void dcEventHandle(DataCollectEvent_t event, uint8_t chn, void *args)
{
    DataCollectContext_t *context;
    SysCollectArgs_t cargs;
    if(event == DATA_COLLECT_EVENT_DONE)
    {
        idleDisplay();
    }
    else
    {
        context = (DataCollectContext_t *)args;
        SysCollectArgsGet(&cargs);
        if(chn != 3 && context->amplitude >= cargs.intensityAlarm || context->frequency >= cargs.ringAlarm * 10)
        {
            HalBeepSet(100);
        }
        updateCollectDisplay(chn, context);
    }
}

void YDDInitialize(void)
{
    KeypadInit(yddKeyEventHandle);
    DisplayInitialize();
    DataCollectInitialize(dcEventHandle);
    g_dataCollect = DataCollectCreate();

    g_menuHandle[YDD_MENU_ID_DETAIL].show = MenuDetailShow;
    g_menuHandle[YDD_MENU_ID_DETAIL].keyHandle = MenuDetailKeyHanlde;
    g_menuHandle[YDD_MENU_ID_DETAIL].hide = MenuDetailHide;
    g_menuHandle[YDD_MENU_ID_SETTINGS].show = MenuSettingsShow;
    g_menuHandle[YDD_MENU_ID_SETTINGS].keyHandle = MenuSettingsKeyHanlde;
    g_menuHandle[YDD_MENU_ID_SETTINGS].hide = MenuSettingsHide;
    g_menuHandle[YDD_MENU_ID_CONTACTUS].show = MenuContactShow;
    g_menuHandle[YDD_MENU_ID_CONTACTUS].keyHandle = MenuContactKeyHanlde;
    g_menuHandle[YDD_MENU_ID_CONTACTUS].hide = MenuContactHide;
    
    //displayPictureSet(SYS_PICTURE_DETAILS_ID);
    idleDisplay();
}

void YDDPoll(void)
{
    KeypadPoll();
    DisplayPoll();
    DataCollectPoll(g_dataCollect);
    updateTimeAndPower();
}

