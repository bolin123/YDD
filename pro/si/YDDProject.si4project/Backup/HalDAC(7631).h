#ifndef HAL_DAC_H
#define HAL_DAC_H

#include "HalCtype.h"

void HalDACLevelSet(uint8_t level);
void HalDACInitialize(void);
void HalDACPoll(void);

#endif
