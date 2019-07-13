#ifndef HAL_ADC_H
#define HAL_ADC_H

#include "HalCtype.h"

uint16_t HalADCGetCollectValue(uint8_t ch);
void HalADCMaxvalueClear(void);
uint16_t HalADCGetPowerValue(void);

//void HalADCGetData(uint16_t *buff);
//uint16_t HalADCGetValue(uint8_t ch);
void HalADCInitialize(void);
#endif

