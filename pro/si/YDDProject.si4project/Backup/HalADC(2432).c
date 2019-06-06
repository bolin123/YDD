#include "HalADC.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#define ADC1_DR_ADDRESS ((uint32_t)0x4001244C)

static uint16_t g_ADCConvertedValue[10][4];

void HalADCGetValue(uint8_t ch, uint16_t *buff, uint16_t bufflen)
{
    uint8_t i;

    if(ch < 4)
    {
        for(i = 0; i < bufflen; i++)
        {
            buff[i] = g_ADCConvertedValue[i][ch];
        }
    }
}

void HalADCInitialize(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //DMA1ͨ��1����
    DMA_DeInit(DMA1_Channel1);
    //�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
    //�ڴ��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_ADCConvertedValue;
    //dma���䷽����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //����DMA�ڴ���ʱ�������ĳ���
    DMA_InitStructure.DMA_BufferSize = 40;
    //����DMA���������ģʽ��һ������
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //����DMA���ڴ����ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //���������ֳ�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    //�ڴ������ֳ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    //����DMA�Ĵ���ģʽ���������ϵ�ѭ��ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    //����DMA�����ȼ���
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //����DMA��2��memory�еı����������
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    //ʹ��ͨ��1
    DMA_Cmd(DMA1_Channel1, ENABLE);

    //ADC1����
    //��������ģʽ
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    //ɨ�跽ʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    //����ת��
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    //�ⲿ������ֹ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    //�����Ҷ���
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //����ת����ͨ����
    ADC_InitStructure.ADC_NbrOfChannel = 4;
    ADC_Init(ADC1, &ADC_InitStructure);

    //����ģʽͨ������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6,  1, ADC_SampleTime_71Cycles5);//PA6 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7,  2, ADC_SampleTime_71Cycles5);//PA7 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_71Cycles5);//PC4 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_71Cycles5);//PC5

    //ʹ��ADC1��DMA
    ADC_DMACmd(ADC1, ENABLE);

    //ʹ��ADC1
    ADC_Cmd(ADC1, ENABLE);

    //ʹ��ADC1��λУ׼�Ĵ���  
    ADC_ResetCalibration(ADC1);
    //���У׼�Ĵ����Ƿ�λ���
    while(ADC_GetResetCalibrationStatus(ADC1));

    //��ʼУ׼
    ADC_StartCalibration(ADC1);
    //����Ƿ�У׼���
    while(ADC_GetCalibrationStatus(ADC1));
     
    //����ADC1�����ת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


