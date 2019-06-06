#include "Display.h"
#include "LCD.h"

#define DISP_POWER_TOP_X 415
#define DISP_POWER_TOP_Y 9
#define DISP_POWER_BOTTOM_X 465
#define DISP_POWER_BOTTOM_Y 30

#define DISP_DATE_X 15
#define DISP_DATE_Y 19

void DisplayPictureShow(SysPictureID_t id)
{
    LCDPictureShow(id);
}

void DisplayDateTimeUpdate(void)
{
    SysDateTime_t *time = SysDateTime();
    char buff[32] = {0};
    LCDPosition_t pos;

    pos.x = DISP_DATE_X;
    pos.y = DISP_DATE_Y;
    sprintf(buff, "%d-%02d-%02d %02d:%02d", time->year, time->month, time->day, time->hour, time->minute);
    LCDStringsPrint(buff, strlen(buff), &pos, false, LCD_CHAR_SIZE_24x48, LCD_COLOR_BLACK, LCD_COLOR_BLUE);
}

void DisplayPowerPercent(uint8_t percent)
{
    LCDPosition_t top, bottom;
    
    top.x = DISP_POWER_TOP_X;
    top.y = DISP_POWER_TOP_Y;
    bottom.x = DISP_POWER_BOTTOM_X;
    bottom.y = DISP_POWER_BOTTOM_Y;
    if(percent < 10)
    {
        LCDRectangle(1, LCD_COLOR_RED, &top, &bottom);
    }
    else
    {
        LCDRectangle(1, LCD_COLOR_GREEN, &top, &bottom);
    }
    LCDRectMove(1, (100 - percent) / 2, LCD_COLOR_CYAN, &top, &bottom);
}

void DisplayInitialize(void)
{
    LCDInitialize();
    LCDPictureShow(SYS_PICTURE_SETTING_ID);
    DisplayPowerPercent(50);
    DisplayDateTimeUpdate();
}

void DisplayPoll(void)
{
    LCDPoll();
}

