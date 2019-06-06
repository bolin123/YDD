#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H


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

#define HAL_DEVICE_ARGS_ADDR (HAL_ARGS_FLASH_ADDR + 0) //总数记录
#define HAL_DATA_RECORD_ADDR (HAL_ARGS_FLASH_ADDR + KB(2)) //总数记录
#define HAL_DATA_POINT_INFO_ADDR (HAL_ARGS_FLASH_ADDR + KB(4)) //点位信息


#endif

