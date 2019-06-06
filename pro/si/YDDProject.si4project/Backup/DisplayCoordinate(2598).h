#ifndef DISPLAY_COORDINATE_H
#define DISPLAY_COORDINATE_H

#include "Sys.h"

//选择框的长度
#define YDD_SELECT_SETTINGS_WIDTH 80
#define YDD_SELECT_SETTINGS_HEIGTH 28
#define YDD_SELECT_CLEAR_WIDTH 60
#define YDD_SELECT_TIMING_WIDTH 60

typedef enum
{
    YDD_SETTINGS_MENUID_CSSJ = 0,
    YDD_SETTINGS_MENUID_QDBJ,
    YDD_SETTINGS_MENUID_XHFZ,
    YDD_SETTINGS_MENUID_ZLBJ,
    YDD_SETTINGS_MENUID_BEEP,
    YDD_SETTINGS_MENUID_CLEAR,
    YDD_SETTINGS_MENUID_TIMING,
    YDD_SETTINGS_MENUID_COUNT,
}YDDSettingsMenuID_t;

typedef enum
{
    YDD_TIMING_MENUID_YEAR,
    YDD_TIMING_MENUID_MONTH,
    YDD_TIMING_MENUID_DAY,
    YDD_TIMING_MENUID_HOUR,
    YDD_TIMING_MENUID_MINUTE,
    YDD_TIMING_MENUID_SECOND,
    YDD_TIMING_MENUID_SURE,
    YDD_TIMING_MENUID_CANCLE,
}YDDTimingMenuID_t;

typedef enum
{
    YDD_CLEAR_MENUID_SURE,
    YDD_CLEAR_MENUID_CANCLE,
}YDDClearMenuID_t;

typedef enum
{
    YDD_DATE_MENUID_YEAR = 0,
    YDD_DATE_MENUID_MONTH,
    YDD_DATE_MENUID_DAY,
    YDD_DATE_MENUID_HOUR,
    YDD_DATE_MENUID_MINUTE,
    YDD_DATE_MENUID_SECOND,
}YDDDateMenuID_t;

// TODO: 完善坐标，修改设置界面
static const SysDisplayPosition_t g_settingsPos[7] = {
    {138, 72}, //测试时间
    {327, 72}, //强度报警
    {138, 112},//信号阀值
    {327, 112},
    {138, 152},
    //{327, 152},
    {56, 190},
    {239, 190},
};

static const SysDisplayPosition_t g_detailsPos[6] = {
    {138, 72},
    {327, 72},
    {138, 112},
    {327, 112},
    {138, 152},
    {327, 152},
};

static const SysDisplayPosition_t g_timingPos[8] = {
    {37, 128},
    {117, 128},
    {188, 128},
    {260, 128},
    {330, 128},
    {403, 128},
};

static const SysDisplayPosition_t g_clearPos[2] = {
    {173, 173},
    {255, 173},
};

typedef struct
{
    SysDisplayPosition_t vltg;
    SysDisplayPosition_t freq;
}YDDCapturePosition_t;

static const YDDCapturePosition_t g_capturePos[4] = {
    {
        {120, 115},
        {120, 166},

    },
    {
        {210, 115},
        {210, 166},
    },
    {
        {300, 115},
        {300, 166},
    },
    {
        {390, 115},
        {390, 166},
    },
};



#endif

