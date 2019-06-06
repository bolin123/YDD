#ifndef HAL_ADC_H
#define HAL_ADC_H

#include "HalCtype.h"

void HalADCGetData(uint16_t *buff);
uint16_t HalADCGetValue(uint8_t ch);
void HalADCInitialize(void);
#endif

