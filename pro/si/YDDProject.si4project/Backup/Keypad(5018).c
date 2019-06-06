#include "Keypad.h"
#include "HalGPIO.h"
#include "Sys.h"

#define KEYPAD_COLUMN_NUM 3
#define KEYPAD_RAW_NUM  2
#define KEYPAD_KEY_NUM  (KEYPAD_COLUMN_NUM * KEYPAD_RAW_NUM)
#define KEYPAD_LONG_PRESS_TRIGGER_TIME 500 //500ms

#define KEYPAD_SCAN1_PIN 0x32 //pd2
#define KEYPAD_SCAN2_PIN 0x08 //pa8
#define KEYPAD_SCAN3_PIN 0x0f //pa15
#define KEYPAD_CODE1_PIN 0x00 //row 0
#define KEYPAD_CODE2_PIN 0x01 //row 1

typedef enum
{
    KEY_STATUS_PRESS = 0,
    KEY_STATUS_RELEASE,
}KeyStatus_t;

typedef struct
{
    uint8_t status;
    uint8_t lastStatus;
    uint32_t lastTriggerTime;
    
//    bool supportLongPress;
//    uint16_t longPressInterval; //长按间隔触发时间
    uint32_t longpressIntervalTime;
}Keypad_t;

static const uint8_t g_columnPin[KEYPAD_COLUMN_NUM] = {KEYPAD_SCAN1_PIN, KEYPAD_SCAN2_PIN, KEYPAD_SCAN3_PIN};
static const uint8_t g_rawPin[KEYPAD_RAW_NUM] = {KEYPAD_CODE1_PIN, KEYPAD_CODE2_PIN};
static Keypad_t g_key[KEYPAD_KEY_NUM];

static void eventHandle(uint8_t keyval, KeyStatus_t status, bool longpress, uint32_t lastTime)
{
    Syslog("key = %d, status = %d, longpress = %d, time = %d", keyval, status, longpress, lastTime);
}

static void keypadScan(void)
{
    uint8_t i, j;
    uint8_t val;
    Keypad_t *key;

    for(i = 0; i < KEYPAD_COLUMN_NUM; i++)
    {
        HalGPIOSetLevel(g_columnPin[i], 0);
        for(j = 0; j < KEYPAD_RAW_NUM; j++)
        {
            val = j * KEYPAD_COLUMN_NUM + i;
            key = &g_key[j * KEYPAD_COLUMN_NUM + i];
            if(HalGPIOGetLevel(g_rawPin[j]) == 0) 
            {
                if(key->lastStatus != 0)
                {
                    eventHandle(val, KEY_STATUS_PRESS, false, 0);
                    key->lastStatus = 0;
                    key->lastTriggerTime = SysTime();
                }
                else
                {
                    if(SysTimeHasPast(key->lastTriggerTime, 2000)) //长按触发时间 2s
                    {
                        eventHandle(val, KEY_STATUS_PRESS, true, SysTime() - key->lastTriggerTime);
                        key->longpressIntervalTime = SysTime();
                        key->lastStatus = 0;
                    }
                }
            }
            else
            {
                if(key->lastStatus != 1) //release
                {
                    eventHandle(val, KEY_STATUS_RELEASE, false, SysTime() - key->lastTriggerTime);
                    key->lastTriggerTime = SysTime();
                    key->lastStatus = 1;
                }
            }
        }
        
        HalGPIOSetLevel(g_columnPin[i], 1);
    }
}

static void getKeyStatus(void)
{
    uint8_t i, j;
    Keypad_t *key;

    for(i = 0; i < KEYPAD_COLUMN_NUM; i++)
    {
        HalGPIOSetLevel(g_columnPin[i], 0);
        for(j = 0; j < KEYPAD_RAW_NUM; j++)
        {
            key = &g_key[j * KEYPAD_COLUMN_NUM + i];
            key->lastStatus = HalGPIOGetLevel(g_rawPin[j]);
        }
    }
}

void KeypadInit(void)
{
    HalGPIOConfig(KEYPAD_SCAN1_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(KEYPAD_SCAN2_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(KEYPAD_SCAN3_PIN, HAL_IO_OUTPUT);
    
    HalGPIOConfig(KEYPAD_CODE1_PIN, HAL_IO_INPUT);
    HalGPIOConfig(KEYPAD_CODE2_PIN, HAL_IO_INPUT);

		getKeyStatus();
	
    HalGPIOSetLevel(KEYPAD_SCAN1_PIN, 1);
    HalGPIOSetLevel(KEYPAD_SCAN2_PIN, 1);
    HalGPIOSetLevel(KEYPAD_SCAN3_PIN, 1);
}

void KeypadPoll(void)
{
    keypadScan();
}

