#include "MenuSettings.h"
#include "Display.h"
#include "DisplayCoordinate.h"

#define MENU_SETTINGS_CSSJ_NUM 8
#define MENU_SETTINGS_ZLBJ_NUM 9

typedef enum
{
    MSDIPLAY_MENU_MAIN = 0,
    MSDIPLAY_MENU_CLEAR,
    MSDIPLAY_MENU_TIMING,
}MSDisplayMenu_t;

static YDDSettingsItemID_t g_settingItemId = YDD_SETTINGS_ITEMID_CSSJ;
static SysCollectArgs_t g_collectArgs;
static const uint16_t CSSJ[MENU_SETTINGS_CSSJ_NUM] = {20, 60, 120, 240, 480, 600, 800, 999};
static const uint8_t ZLBJ[MENU_SETTINGS_ZLBJ_NUM] = {5, 20, 50, 80, 100, 120, 150, 180, 200};
//static bool g_argsModified = false;
static MSDisplayMenu_t g_displayMenu;
static SysDateTime_t g_date;
static YDDTimingItemID_t g_timmingItemId;
static YDDClearItemID_t g_clearItemId;

static void clearNextItem(void)
{
    //unselect front
    DisplaySelectBox(false, &g_clearPos[g_clearItemId++], YDD_SELECT_CLEAR_WIDTH, YDD_SELECT_TIMING_HEIGTH);
    if(g_clearItemId == YDD_CLEAR_ITEMID_COUNT)
    {
        g_clearItemId = YDD_CLEAR_ITEMID_SURE;
    }
    DisplaySelectBox(true, &g_clearPos[g_clearItemId], YDD_SELECT_CLEAR_WIDTH, YDD_SELECT_TIMING_HEIGTH); //select

}

static void clearItemSelectHandle(void)
{
    char *notice = "数据已清空";
    SysDisplayPosition_t top;
    switch (g_clearItemId)
    {
        case YDD_CLEAR_ITEMID_SURE:
            SysArgsClear();
            top.x = 190;
            top.y = 140;
            DiplayStringPrint(notice, strlen(notice), DISPLAY_COLOR_BLACK, &top, DISPLAY_CHAR_SIZE_NORMAL);
            HalWaitMs(500);
            MenuSettingsShow();
        break;
        case YDD_CLEAR_ITEMID_CANCLE:
            MenuSettingsShow();
        break;
        default:
        break;
    }
}

static void clearShow(void)
{
    DisplayPictureShow(SYS_PICTURE_CLEAR_ID);    
    g_displayMenu = MSDIPLAY_MENU_CLEAR;
    g_clearItemId = YDD_CLEAR_ITEMID_SURE;    
    DisplaySelectBox(true, &g_clearPos[g_clearItemId], YDD_SELECT_CLEAR_WIDTH, YDD_SELECT_TIMING_HEIGTH); //select
    DisplayDateTimeUpdate(); //时间显示
    DisplayPowerPercent();
}

static bool isLeapYear(uint16_t year)
{
    if(year && year % 4 == 0)
    {
        if(year % 100 == 0)
        {
            if(year % 400 != 0)
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

static uint8_t dayInMonth(uint16_t year, uint8_t month)
{
    uint8_t monthDays;   
    bool isLeap = isLeapYear(year);
    
    switch(month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        monthDays = 31;
        break;

    case 2:
        monthDays = isLeap ? 29 : 28;
        break;
    default:
        monthDays = 30;
    }
    return monthDays;
}

static void timingItemValueUpdate(YDDTimingItemID_t id, uint16_t value)
{
    SysDisplayPosition_t top, bottom;
    char buff[8] = "";
    
    top.x = g_timingPos[id].x;
    top.y = g_timingPos[id].y;

    Syslog("ID=%d, value=%d", id, value);
    if(id == YDD_TIMING_ITEMID_YEAR)
    {
        bottom.x = g_timingPos[id].x + 12 * 4;
        sprintf(buff, "%d", value);
    }
    else
    {
        bottom.x = g_timingPos[id].x + 12 * 2;
        sprintf(buff, "%02d", value);
    }
    bottom.y = g_timingPos[id].y + 24;
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &top, &bottom);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &top, DISPLAY_CHAR_SIZE_NORMAL);    
}

static void timmingItemSelectHandle(void)
{
    switch (g_timmingItemId)
    {
    case YDD_TIMING_ITEMID_YEAR:
        g_date.year++;
        if(g_date.year > 2099)
        {
            g_date.year = 2019;
        }
        timingItemValueUpdate(YDD_TIMING_ITEMID_YEAR, g_date.year);
    break;
    case YDD_TIMING_ITEMID_MONTH:
        g_date.month++;
        if(g_date.month > 12)
        {
            g_date.month = 1;
        }
        timingItemValueUpdate(YDD_TIMING_ITEMID_MONTH, g_date.month);
    break;
    case YDD_TIMING_ITEMID_DAY:
        g_date.day++;
        if(g_date.day > dayInMonth(g_date.year, g_date.month))
        {
            g_date.day = 1;
        }
        timingItemValueUpdate(YDD_TIMING_ITEMID_DAY, g_date.day);
    break;
    case YDD_TIMING_ITEMID_HOUR:
        g_date.hour++;
        if(g_date.hour == 24)
        {
            g_date.hour = 0;
        }
        timingItemValueUpdate(YDD_TIMING_ITEMID_HOUR, g_date.hour);
    break;
    case YDD_TIMING_ITEMID_MINUTE:
        g_date.minute++;
        if(g_date.minute == 60)
        {
            g_date.minute = 0;
        }
        timingItemValueUpdate(YDD_TIMING_ITEMID_MINUTE, g_date.minute);
    break;
    case YDD_TIMING_ITEMID_SECOND:
         g_date.second++;
         if(g_date.second == 60)
         {
            g_date.second = 0;
         }
         timingItemValueUpdate(YDD_TIMING_ITEMID_SECOND, g_date.second);
    break;
    case YDD_TIMING_ITEMID_SURE:
        SysDateTimeSet(&g_date);
        MenuSettingsShow();
    break;
    case YDD_TIMING_ITEMID_CANCLE:
        //return to setting menu
        MenuSettingsShow();
    break;
    default:
    break;
    }
}

static void timmingNextItem(void)
{
    uint8_t width = YDD_SELECT_TIMING_WIDTH / 2;
    
    if(g_timmingItemId == YDD_TIMING_ITEMID_YEAR || 
        g_timmingItemId == YDD_TIMING_ITEMID_SURE ||
        g_timmingItemId == YDD_TIMING_ITEMID_CANCLE)
    {
        width = YDD_SELECT_TIMING_WIDTH;
    }
    //unselect front
    DisplaySelectBox(false, &g_timingPos[g_timmingItemId++], width, YDD_SELECT_TIMING_HEIGTH);
    if(g_timmingItemId == YDD_TIMING_ITEMID_COUNT)
    {
        g_timmingItemId = YDD_TIMING_ITEMID_YEAR;
    }
    
    width = YDD_SELECT_TIMING_WIDTH / 2;
    if(g_timmingItemId == YDD_TIMING_ITEMID_YEAR || 
        g_timmingItemId == YDD_TIMING_ITEMID_SURE ||
        g_timmingItemId == YDD_TIMING_ITEMID_CANCLE)
    {
        width = YDD_SELECT_TIMING_WIDTH;
    }
    DisplaySelectBox(true, &g_timingPos[g_timmingItemId], width, YDD_SELECT_TIMING_HEIGTH); //select

}

static void timmingShow(void)
{
    char buff[5];
    uint8_t i = 0;
    g_date = *SysDateTime();
    
    DisplayPictureShow(SYS_PICTURE_DATE_ID);

    if(g_date.year < 2019)
    {
        g_date.year = 2019;
    }
    buff[0] = '\0';
    sprintf(buff, "%d", g_date.year);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_timingPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    buff[0] = '\0';
    sprintf(buff, "%02d", g_date.month);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_timingPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    buff[0] = '\0';
    sprintf(buff, "%02d", g_date.day);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_timingPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    buff[0] = '\0';
    sprintf(buff, "%02d", g_date.hour);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_timingPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    buff[0] = '\0';
    sprintf(buff, "%02d", g_date.minute);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_timingPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    buff[0] = '\0';
    sprintf(buff, "%02d", g_date.second);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_timingPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    DisplaySelectBox(true, &g_timingPos[YDD_TIMING_ITEMID_YEAR], YDD_SELECT_TIMING_WIDTH, YDD_SELECT_TIMING_HEIGTH);
    g_displayMenu = MSDIPLAY_MENU_TIMING;
    g_timmingItemId = YDD_TIMING_ITEMID_YEAR;
    DisplayDateTimeUpdate(); //时间显示
    DisplayPowerPercent();
}

static void selectNextMenuItem(void)
{
    //unselect front
    DisplaySelectBox(false, &g_settingsPos[g_settingItemId++], YDD_SELECT_SETTINGS_WIDTH, YDD_SELECT_SETTINGS_HEIGTH);
    if(g_settingItemId == YDD_SETTINGS_ITEMID_COUNT)
    {
        g_settingItemId = YDD_SETTINGS_ITEMID_CSSJ;
    }
    DisplaySelectBox(true, &g_settingsPos[g_settingItemId], YDD_SELECT_SETTINGS_WIDTH, YDD_SELECT_SETTINGS_HEIGTH); //select
}

static void updateItemValue(YDDSettingsItemID_t id, uint16_t value, char *str)
{
    SysDisplayPosition_t top, bottom;
    char buff[8] = "";
    
    top.x = g_settingsPos[id].x;
    top.y = g_settingsPos[id].y;
    if(str == NULL)
    {
        sprintf(buff, "%d", value);
    }
    else
    {
        sprintf(buff, "%s", str);
    }
    Syslog("ID=%d, value=%d", id, value);
    bottom.x = g_settingsPos[id].x + 12 * 4;
    bottom.y = g_settingsPos[id].y + 24;
    DisplayDrawRect(DISPLAY_COLOR_BOTTOM, &top, &bottom);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &top, DISPLAY_CHAR_SIZE_NORMAL);
}

static void itemSelectHandle(void)
{
    uint8_t i, j;
    
    switch (g_settingItemId)
    {
    case YDD_SETTINGS_ITEMID_CSSJ:
        for(i = 0; i < MENU_SETTINGS_CSSJ_NUM; i++)
        {
            if(g_collectArgs.runTime == CSSJ[i])
            {
                j = i + 1;
                if(j >= MENU_SETTINGS_CSSJ_NUM)
                {
                    j = 0;
                }
                g_collectArgs.runTime = CSSJ[j];
                updateItemValue(YDD_SETTINGS_ITEMID_CSSJ, g_collectArgs.runTime, NULL);
                break;
            }
        }
        break;
    case YDD_SETTINGS_ITEMID_QDBJ: //500~1500,step = 500
        g_collectArgs.intensityAlarm += 500;
        if(g_collectArgs.intensityAlarm > 1500)
        {
            g_collectArgs.intensityAlarm = 500;
        }
        updateItemValue(YDD_SETTINGS_ITEMID_QDBJ, g_collectArgs.intensityAlarm, NULL);
        break;
    case YDD_SETTINGS_ITEMID_XHFZ: //50~300,step=50
        g_collectArgs.signalThreshold += 50;
        if(g_collectArgs.signalThreshold > 300)
        {
            g_collectArgs.signalThreshold = 50;
        }
        SysSignalThresholdSet(g_collectArgs.signalThreshold);
        updateItemValue(YDD_SETTINGS_ITEMID_XHFZ, g_collectArgs.signalThreshold, NULL);
        break;
    case YDD_SETTINGS_ITEMID_ZLBJ: //5, 20, 50, 80, 100, 120, 150, 180, 200K
        for(i = 0; i < MENU_SETTINGS_ZLBJ_NUM; i++)
        {
            if(g_collectArgs.ringAlarm == ZLBJ[i])
            {
                j = i + 1;
                if(j >= MENU_SETTINGS_ZLBJ_NUM)
                {
                    j = 0;
                }
                g_collectArgs.ringAlarm = ZLBJ[j];
                updateItemValue(YDD_SETTINGS_ITEMID_ZLBJ, g_collectArgs.ringAlarm, NULL);
                break;
            }
        }
        break;
    case YDD_SETTINGS_ITEMID_BEEP:
        g_collectArgs.beep = !g_collectArgs.beep;
        SysBeepEnable(g_collectArgs.beep);
        updateItemValue(YDD_SETTINGS_ITEMID_BEEP, 0, g_collectArgs.beep ? "开" : "关");
        break;
    case YDD_SETTINGS_ITEMID_CLEAR:
        clearShow();
        break;
    case YDD_SETTINGS_ITEMID_TIMING:
        timmingShow();
        break;
    default:
        break;
    }
    //修改flash保存标志位
    if(g_settingItemId < YDD_SETTINGS_ITEMID_CLEAR)
    {
        SysCollectArgsSet(&g_collectArgs);
    }
}

void MenuSettingsKeyHanlde(HalKeyValue_t key)
{
    if(key == HAL_KEY_VALUE_TOGGLE)
    {
        if(g_displayMenu == MSDIPLAY_MENU_MAIN)
        {
            selectNextMenuItem();
        }else if(g_displayMenu == MSDIPLAY_MENU_TIMING)
        {
            timmingNextItem();
        }
        else
        {
            clearNextItem();
        }
    }
    else if(key == HAL_KEY_VALUE_CONFIG)
    {
        if(g_displayMenu == MSDIPLAY_MENU_MAIN)
        {
            itemSelectHandle();
        }else if(g_displayMenu == MSDIPLAY_MENU_TIMING)
        {
            timmingItemSelectHandle();
        }
        else
        {
            clearItemSelectHandle();
        }
    }
    else
    {
    }
}

void MenuSettingsHide(void)
{
/*
    if(g_argsModified)
    {
        SysCollectArgsSet(&g_collectArgs);
        g_argsModified = false;
    }
    */
}

void MenuSettingsShow(void)
{   
    char buff[10];
    uint8_t i = 0;

    /*背景图片*/
    DisplayPictureShow(SYS_PICTURE_SETTING_ID);

    /*测试时间*/
    SysCollectArgsGet(&g_collectArgs);
    buff[0] = '\0';
    sprintf(buff, "%d", g_collectArgs.runTime);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*强度报警值*/
    buff[0] = '\0';
    sprintf(buff, "%d", g_collectArgs.intensityAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*信号阈值*/
    buff[0] = '\0';
    sprintf(buff, "%d", g_collectArgs.signalThreshold);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);
    
    /*振铃报警值*/
    buff[0] = '\0';
    sprintf(buff, "%d", g_collectArgs.ringAlarm);
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    /*蜂鸣器*/
    buff[0] = '\0';
    sprintf(buff, "%s", g_collectArgs.beep ? "开":"关");
    DiplayStringPrint(buff, strlen(buff), DISPLAY_COLOR_BLACK, &g_settingsPos[i++], DISPLAY_CHAR_SIZE_NORMAL);

    DisplaySelectBox(true, &g_settingsPos[YDD_SETTINGS_ITEMID_CSSJ], YDD_SELECT_SETTINGS_WIDTH, YDD_SELECT_SETTINGS_HEIGTH);
    g_settingItemId  = YDD_SETTINGS_ITEMID_CSSJ;
    g_displayMenu = MSDIPLAY_MENU_MAIN;

    DisplayDateTimeUpdate(); //时间显示
    DisplayPowerPercent();
}



