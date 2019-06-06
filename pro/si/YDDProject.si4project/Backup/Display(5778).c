#include "Display.h"

#define DISPLAY_HIGH_BOARDRATE_PIN 0x11
#define DISPLAY_BUSY_PIN 0x12

#define DISPLAY_PROTOCOL_HEAD 0xAA

#define DISPLAY_CMD_CLEAR  0x01 //清屏
#define DISPLAY_CMD_PONIT  0x02 //画点
#define DISPLAY_CMD_LINE   0x03 //画线
#define DISPLAY_CMD_RECTANGLE 0x05 //画矩形
#define DISPLAY_CMD_MOVE   0x09 //区域移动
#define DISPLAY_CMD_STRING 0x11 //显示字符串
#define DISPLAY_CMD_NUMBER 0x14 //显示数值
#define DISPLAY_CMD_JPEG   0x22 //显示图片
#define DISPLAY_CMD_BRIGHTNESS 0x30 //亮度设置

static void dispDataSend(uint8_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t i = 0;
    uint8_t buff[256];

    if(len < 250)
    {
        buff[i++] = DISPLAY_PROTOCOL_HEAD;
        buff[i++] = cmd;
        memcpy(&buff[i], data, len);
        i += len;
        buff[i++] = 0xCC;
        buff[i++] = 0x33;
        buff[i++] = 0xC3;
        buff[i++] = 0x3C;

        HalUartWrite(HAL_UART_DISPLAY_PORT, buff, i);
    }
}

void DisplayClear(void)
{
    uint8_t blue[2] = {0x00, 0x1F};

    Syslog("");
    dispDataSend(DISPLAY_CMD_CLEAR, blue, sizeof(blue));
}

static void uartInit(void)
{
    HalUartConfig_t config;
    
    config.baudrate = 115200;
    config.flowControl = 0;
    config.parity = 0;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = NULL;
    HalUartConfig(HAL_UART_DISPLAY_PORT, &config);
}

void DisplayInitialize(void)
{
    //HalGPIOConfig(DISPLAY_HIGH_BOARDRATE_PIN, HAL_IO_OUTPUT);
    //HalGPIOConfig(DISPLAY_BUSY_PIN, HAL_IO_INPUT);
    //HalGPIOSetLevel(DISPLAY_HIGH_BOARDRATE_PIN, 0);
    uartInit();
}

void DisplayPoll(void)
{
}

