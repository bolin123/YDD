#include "HalExti.h"

extern void DataCollectFreqCount(uint8_t ch);
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line9) != RESET){
        DataCollectFreqCount(0);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }else if(EXTI_GetITStatus(EXTI_Line8)!=RESET){
        DataCollectFreqCount(1);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }else{
        DataCollectFreqCount(2);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}

void HalExtiInitialize(void)
{
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource7 | GPIO_PinSource8 | GPIO_PinSource9);
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_PinSource7 | GPIO_PinSource8 | GPIO_PinSource9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7 | GPIO_PinSource8 | GPIO_PinSource9);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line7 | EXTI_Line8 | EXTI_Line9; 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
    EXTI_Init(&EXTI_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line7 | EXTI_Line8 | EXTI_Line9);

    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USARTx Interrupt */ 
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 
}

void HalExtiPoll(void)
{
}


