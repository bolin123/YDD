#include "HalDAC.h"

void HalDACLevelSet(uint8_t level)
{
	DAC_SetChannel1Data(DAC_Align_12b_R, 1892 + 62 * level);		//1892 base = 1.515v, 62 = 0.05v
	DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);   //�������,DAֵ����
}

void HalDACInitialize(void)
{
    DAC_InitTypeDef	DAC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software; 	         //�������,��ʹ�ö�ʱ�� �����ⲿ�жϵȴ���
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;   //���������ǲ� ���� ������
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable; 	  //��������������Դ򿪻���
    DAC_Init(DAC_Channel_1, &DAC_InitStructure); 

    DAC_DMACmd(DAC_Channel_1, DISABLE);		//��ʹ��DMA
    DAC_Cmd(DAC_Channel_1, ENABLE);
	
	HalDACLevelSet(0);
}

void HalDACPoll(void)
{
}



