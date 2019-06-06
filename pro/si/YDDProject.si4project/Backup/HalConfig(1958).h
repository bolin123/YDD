#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

//#define HAL_BOARD_YDD_2CH 
//#define HAL_BOARD_YDD_4CH

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
#define HAL_DAC_BASE_VALUE 1892 //1.515v
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

