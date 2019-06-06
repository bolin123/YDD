#include "HalADC.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

//#define HAL_ADC_CH_NUM 4
#define HAL_ADC_CH_BUFF_LEN 12
#define ADC1_DR_ADDRESS ((uint32_t)0x4001244C)

static uint16_t g_adcValue[HAL_ADC_CH_BUFF_LEN][HAL_ADC_CH_NUM];

uint16_t HalADCGetValue(uint8_t ch)
{
    uint8_t i;
    uint16_t max = 0, min = 0xffff;
    uint32_t count = 0;

    if(ch < HAL_ADC_CH_NUM)
    {
        for(i = 0; i < HAL_ADC_CH_BUFF_LEN; i++)
        {
            if(max < g_adcValue[i][ch])
            {
                max = g_adcValue[i][ch];
            }
            
            if(min > g_adcValue[i][ch])
            {
                min = g_adcValue[i][ch];
            }
            count += g_adcValue[i][ch];
        }

        return (uint16_t)((count - max - min) / (HAL_ADC_CH_BUFF_LEN - 2));
    }
    return 0;
}

void HalADCGetData(uint16_t *buff)
{
    uint8_t i, ch;
    uint16_t max = 0, min = 0xffff;
    uint32_t count = 0;

    for(ch = 0; ch < HAL_ADC_CH_NUM; ch++)
    {
        for(i = 0; i < HAL_ADC_CH_BUFF_LEN; i++)
        {
            if(max < g_adcValue[i][ch])
            {
                max = g_adcValue[i][ch];
            }
            
            if(min > g_adcValue[i][ch])
            {
                min = g_adcValue[i][ch];
            }
            count += g_adcValue[i][ch];
        }
        buff[ch] = (uint16_t)((count - max - min) / (HAL_ADC_CH_BUFF_LEN - 2));
    }
    
}

void HalADCInitialize(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
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
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_adcValue;
    //dma���䷽����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //����DMA�ڴ���ʱ�������ĳ���
    DMA_InitStructure.DMA_BufferSize = HAL_ADC_CH_BUFF_LEN * HAL_ADC_CH_NUM;
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
    ADC_InitStructure.ADC_NbrOfChannel = HAL_ADC_CH_NUM;
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
     
    //����ADC1������ת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

