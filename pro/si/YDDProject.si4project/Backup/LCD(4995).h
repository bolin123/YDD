#ifndef LCD_H
#define LCD_H

#include "Sys.h"

#define LCD_PIXEL_WIDTH  480
#define LCD_PIXEL_HEIGHT 272

typedef struct
{
    uint16_t x;
    uint16_t y;
}LCDPosition_t;

typedef enum
{
    LCD_CHAR_SIZE_6x12 = 0,
    LCD_CHAR_SIZE_8x16,
    LCD_CHAR_SIZE_10x20,
    LCD_CHAR_SIZE_12x24,
    LCD_CHAR_SIZE_14x28,
    LCD_CHAR_SIZE_16x32,
    LCD_CHAR_SIZE_20x40,
    LCD_CHAR_SIZE_24x48,
    LCD_CHAR_SIZE_28x56,
    LCD_CHAR_SIZE_32x64,
}LCDCharSize_t;

//显示颜色RGB565
#define LCD_COLOR_BLACK     0x0000  //黑色 ?
#define LCD_COLOR_NAVY      0x000F  //深蓝色
#define LCD_COLOR_DGREEN    0x03E0  //深绿色
#define LCD_COLOR_DCYAN     0x03EF  //深青色
#define LCD_COLOR_MAROON    0x7800  //深红色
#define LCD_COLOR_PURPLE    0x780F  //紫色 ?
#define LCD_COLOR_OLIVE     0x7BE0  //橄榄绿
#define LCD_COLOR_LGRAY     0xC618  //灰白色
#define LCD_COLOR_DGRAY     0x7BEF  //深灰色
#define LCD_COLOR_BLUE      0x001F  //蓝色 ?
#define LCD_COLOR_GREEN     0x07E0  //绿色 ?
#define LCD_COLOR_CYAN      0x07FF  //青色 ?
#define LCD_COLOR_RED       0xF800  //红色 ?
#define LCD_COLOR_MAGENTA   0xF81F  //品红 ?
#define LCD_COLOR_YELLOW    0xFFE0  //黄色 ?
#define LCD_COLOR_WHITE     0xFFFF  //白色 ?

void LCDStringsPrint(char *data, uint8_t len, LCDPosition_t *pos, bool hasBgColor, 
                                LCDCharSize_t size, uint16_t color, uint16_t bgcolor);
void LCDPictureShow(SysPictureID_t id);
void LCDRectangle(uint8_t mode, uint16_t color, LCDPosition_t *top, LCDPosition_t *bottom);
void LCDRectMove(uint8_t dir, uint8_t distance, uint16_t color, LCDPosition_t *top, LCDPosition_t *bottom);
void LCDClear(void);
void LCDInitialize(void);
void LCDPoll(void);

#endif

