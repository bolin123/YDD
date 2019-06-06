#include "Display.h"
#include "LCD.h"

#define DISP_POWER_TOP_X 415
#define DISP_POWER_TOP_Y 9
#define DISP_POWER_BOTTOM_X 465
#define DISP_POWER_BOTTOM_Y 30

#define DISP_DATE_X 15
#define DISP_DATE_Y 8

void DisplayPictureShow(SysPictureID_t id)
{
    //LCDClear();
    LCDPictureShow(id);
}

void DisplaySettingsSelect(uint8_t id)
{
    SysDisplayPosition_t top, bottom;
    switch(id)
    {
        case 0://测点
            top.x = 137;
            top.y = 70;
            bottom.x = top.x + 80;
            bottom.y = top.y + 26;
        break;
        case 1://测试时间
            top.x = 137;
            top.y = 110;
            bottom.x = top.x + 80;
            bottom.y = top.y + 26;
        break;
        case 2://蜂鸣器
            top.x = 137;
            top.y = 150;
            bottom.x = top.x + 80;
            bottom.y = top.y + 26;
        break;
        case 3://清除数据
            top.x = 56;
            top.y = 190;
            bottom.x = top.x + 80;
            bottom.y = top.y + 31;
        break;
        case 4: //信号阀值
            top.x = 326;
            top.y = 70;
            bottom.x = top.x + 80;
            bottom.y = top.y + 26;
        break;
        case 5: //强度报警值
            top.x = 326;
            top.y = 110;
            bottom.x = top.x + 80;
            bottom.y = top.y + 26;
        break;
        case 6: //振铃报警值
            top.x = 326;
            top.y = 150;
            bottom.x = top.x + 80;
            bottom.y = top.y + 26;
        break;
        case 7: //设置时间
            top.x = 239;
            top.y = 190;
            bottom.x = top.x + 80;
            bottom.y = top.y + 31;
        break;
        default:
            return;
    }
    LCDRectangle(2, DISPLAY_COLOR_GREEN, &top, &bottom);
}

void DisplayDateTimeUpdate(void)
{
    SysDateTime_t *time = SysDateTime();
    char buff[32] = {0};
    SysDisplayPosition_t pos;
    SysDisplayPosition_t top, bottom;

    top.x = 12;
    top.y = 9;
    bottom.x = 215;
    bottom.y = 30;
    LCDRectangle(1, DISPLAY_COLOR_BOTTOM, &top, &bottom);
    pos.x = DISP_DATE_X;
    pos.y = DISP_DATE_Y;
    sprintf(buff, "%d-%02d-%02d %02d:%02d", time->year, time->month, time->day, time->hour, time->minute);
    LCDStringsPrint(buff, strlen(buff), &pos, false, LCD_CHAR_SIZE_12x24, DISPLAY_COLOR_BLACK, DISPLAY_COLOR_BLUE);
}

void DisplayPowerPercent(uint8_t percent)
{
    SysDisplayPosition_t top, bottom;
    
    top.x = DISP_POWER_TOP_X;
    top.y = DISP_POWER_TOP_Y;
    bottom.x = DISP_POWER_BOTTOM_X;
    bottom.y = DISP_POWER_BOTTOM_Y;
    if(percent < 10)
    {
        LCDRectangle(1, DISPLAY_COLOR_RED, &top, &bottom);
    }
    else
    {
        LCDRectangle(1, DISPLAY_COLOR_GREEN, &top, &bottom);
    }
    LCDRectMove(1, (100 - percent) / 2, DISPLAY_COLOR_CYAN, &top, &bottom);
}

void DiplayStringPrint(char * data, uint8_t len, uint16_t color, SysDisplayPosition_t *pos, DisplayCharSize_t size)
{
    LCDCharSize_t lsize;

    switch (size)
    {
        case DISPLAY_CHAR_SIZE_SMALL:
            lsize = LCD_CHAR_SIZE_6x12;
            break;
        case DISPLAY_CHAR_SIZE_NORMAL:
            lsize = LCD_CHAR_SIZE_12x24;
            break;
        case DISPLAY_CHAR_SIZE_LARGE:
            lsize = LCD_CHAR_SIZE_24x48;
            break;
        default:
            break;
    }
    LCDStringsPrint(data, len, pos, false, lsize, color, DISPLAY_COLOR_BLUE);
}

void DisplayInitialize(void)
{
    LCDInitialize();
    
    //LCDClear();
    //LCDPictureShow(SYS_PICTURE_SETTING_ID);
    //DisplayPowerPercent(50);
    //DisplayDateTimeUpdate();
}

void DisplayPoll(void)
{
    LCDPoll();
}

