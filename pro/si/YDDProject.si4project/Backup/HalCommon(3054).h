#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include "HalCtype.h"
#include "HalUart.h"
#include "HalGPIO.h"
#include "HalTimer.h"
#include "HalSPI.h"
#include "HalFlash.h"
#include "HalWait.h"
#include "HalRTC.h"

void HalCommonStatusLedSet(uint8_t blink);
uint32_t HalGetSysTimeCount(void);
void HalInterruptSet(bool enable);
void HalTimerPast1ms(void);
void HalCommonReboot(void);
void HalCommonInitialize(void);
void HalCommonPoll(void);

#endif

