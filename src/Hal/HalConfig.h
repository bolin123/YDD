#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

//#define HAL_BOARD_YDD_2CH 
//#define HAL_BOARD_YDD_4CH

#define HAL_UART_PC_PORT 0 //HAL_UART_PORT_1
#define HAL_UART_LCD_PORT 2 //HAL_UART_PORT_3

typedef enum
{
    SYS_PICTURE_WELCOME_ID = 0,     //欢迎页
    SYS_PICTURE_DETAILS_ID = 1,     //详情
    SYS_PICTURE_SETTING_ID = 2,     //设置
    SYS_PICTURE_CONTACT_ID = 3,     //联系我们
    SYS_PICTURE_DATE_ID = 4,        //日期设置
    SYS_PICTURE_CLEAR_ID = 5,       //清除
    SYS_PICTURE_CAPTURE_ID = 6,     //采集
    SYS_PICTURE_ID_COUNT,
}SysPictureID_t;

typedef enum
{
    HAL_KEY_VALUE_F2 = 0,   //F2
    HAL_KEY_VALUE_CONFIG,   //设置
    HAL_KEY_VALUE_PAGEDOWN, //翻页
    HAL_KEY_VALUE_F1,       //F1
    HAL_KEY_VALUE_TOGGLE,   //切换
    HAL_KEY_VALUE_START,    //启动
}HalKeyValue_t;


#if defined(HAL_YDD_2CH_BOARD)
#define HAL_BEEP_CTRL_PIN 0x02 //蜂鸣器
#define HAL_IO_UART_PIN  0x03  //模拟串口IO

#define KEYPAD_SCAN1_PIN 0x32 //column1   AJ3
#define KEYPAD_SCAN2_PIN 0x08 //column2   AJ4
#define KEYPAD_SCAN3_PIN 0x0f //column3  AJ5
#define KEYPAD_CODE1_PIN 0x00 //row 0 AJ1
#define KEYPAD_CODE2_PIN 0x01 //row 1 AJ2

#elif defined(HAL_BOARD_YDD_4CH)
#define HAL_BEEP_CTRL_PIN 0x2c //蜂鸣器
#define HAL_IO_UART_PIN  0x03  //模拟串口IO

#define KEYPAD_SCAN1_PIN 0x14 //column1   AJ3
#define KEYPAD_SCAN2_PIN 0x15 //column2   AJ4
#define KEYPAD_SCAN3_PIN 0x16 //column3  AJ5
#define KEYPAD_CODE1_PIN 0x32 //row 0 AJ1
#define KEYPAD_CODE2_PIN 0x13 //row 1 AJ2

#endif

#define HAL_ADC_CH_NUM 4
#define HAL_DAC_BASE_VALUE 1830 //1.51, 1.515v = 1892
#define HAL_DAC_STEP_VALUE 62   //0.05v


#define KB(x) ((x)*1024)
/*flash分区
* 0 ~ 5k     :boot
* 5k ~ 125k  :app
* 125k ~ 245k:ota
* 245k ~ 256k:args
*/
#define HAL_FLASH_SIZE (KB(256))  //256KB 120k(ota), 4k(boot) 4+240+12
#define HAL_FLASH_PAGE_SIZE (KB(2))
#define HAL_FLASH_OTA_SIZE (KB(120))

#define HAL_FLASH_BASE_ADDR  0x8000000
#define HAL_BOOT_FLASH_ADDR  (HAL_FLASH_BASE_ADDR + 0)
#define HAL_APP_FLASH_ADDR   (HAL_FLASH_BASE_ADDR + KB(4))
#define HAL_OTA_FLASH_ADDR   (HAL_FLASH_BASE_ADDR + KB(124))
#define HAL_ARGS_FLASH_ADDR  (HAL_FLASH_BASE_ADDR + KB(230))

#define HAL_DEVICE_ARGS_ADDR (HAL_ARGS_FLASH_ADDR + 0) //设备参数
#define HAL_DATA_RECORD_ADDR (HAL_ARGS_FLASH_ADDR + KB(2)) //总数记录
#define HAL_DATA_POINT_INFO_ADDR (HAL_ARGS_FLASH_ADDR + KB(4)) //点位信息

#define HAL_DATA_POINT_INFO_PAGE 11 //11 page
#endif

