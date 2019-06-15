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
    
    if(lastTime > 85)//防抖
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

static void exceptionShow(uint16_t errcode)
{
    SysDisplayPosition_t pos;
    char buff[8] = "";
    DisplayPictureShow(SYS_PICTURE_EXCEPTION_ID);
    pos.x = 165;
    pos.y = 105;
    sprintf(buff, "%d", errcode);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &pos, DISPLAY_CHAR_SIZE_LARGE);
    HalGPIOSetLevel(KEYPAD_SCAN1_PIN, 0);
    HalGPIOSetLevel(KEYPAD_SCAN2_PIN, 0);
    HalGPIOSetLevel(KEYPAD_SCAN3_PIN, 0);
    while(1)
    {
        if(HalGPIOGetLevel(KEYPAD_CODE1_PIN) == 0 ||
            HalGPIOGetLevel(KEYPAD_CODE2_PIN) == 0)
        {
            break;
        }
        HalWaitMs(100);
    }
}

void YDDInitialize(uint16_t errcode)
{
    KeypadInit(yddKeyEventHandle);
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

    if(errcode)
    {
        exceptionShow(errcode);
    }
    //displayPictureSet(SYS_PICTURE_DETAILS_ID);
    idleDisplay();
}

void YDDPoll(void)
{
    KeypadPoll();
    DataCollectPoll(g_dataCollect);
    updateTimeAndPower();
}

