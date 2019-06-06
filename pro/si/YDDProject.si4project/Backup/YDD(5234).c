#include "YDD.h"
#include "Keypad.h"

typedef enum
{
    YDD_KEY_VALUE_F2 = 0,   //F2
    YDD_KEY_VALUE_CONFIG,   //ÉèÖÃ
    YDD_KEY_VALUE_PAGEDOWN, //·­Ò³
    YDD_KEY_VALUE_START,    //Æô¶¯
    YDD_KEY_VALUE_TOGGLE,   //ÇÐ»»
    YDD_KEY_VALUE_F1,       //F1
}YDDKeyValue_t;

typedef enum
{
    YDD_MENU_WELCOME = 0,
    YDD_MENU_1,
    YDD_MENU_2,
    YDD_MENU_3,
    YDD_MENU_COLLECTING,
}YDDMenu_t;

static YDDMenu_t g_menu = YDD_MENU_WELCOME;

static void yddKeyEventHandle(uint8_t keyval, KeyStatus_t status, bool longpress, uint32_t lastTime)
{
    if(lastTime > 20)
    {
        switch ((YDDKeyValue_t)keyval)
        {
            case YDD_KEY_VALUE_CONFIG:
                break;
            case YDD_KEY_VALUE_PAGEDOWN:
                break;
            case YDD_KEY_VALUE_START:
                break;
            case YDD_KEY_VALUE_TOGGLE:
                break;
            default:
                break;
        }
    }
}

void YDDInitialize(void)
{
    KeypadInit(yddKeyEventHandle);
}

void YDDPoll(void)
{
    KeypadPoll();
}

