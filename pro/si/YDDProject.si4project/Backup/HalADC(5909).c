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

    //DMA1通道1配置
    DMA_DeInit(DMA1_Channel1);
    //外设地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_adcValue;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = HAL_ADC_CH_BUFF_LEN * HAL_ADC_CH_NUM;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    //设置DMA的传输模式：连续不断的循环模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    //使能通道1
    DMA_Cmd(DMA1_Channel1, ENABLE);

    //ADC1配置
    //独立工作模式
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    //扫描方式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    //连续转换
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    //外部触发禁止
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    //数据右对齐
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //用于转换的通道数
    ADC_InitStructure.ADC_NbrOfChannel = HAL_ADC_CH_NUM;
    ADC_Init(ADC1, &ADC_InitStructure);

    //规则模式通道配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6,  1, ADC_SampleTime_71Cycles5);//PA6 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7,  2, ADC_SampleTime_71Cycles5);//PA7 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_71Cycles5);//PC4 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_71Cycles5);//PC5

    //使能ADC1的DMA
    ADC_DMACmd(ADC1, ENABLE);

    //使能ADC1
    ADC_Cmd(ADC1, ENABLE);

    //使能ADC1复位校准寄存器  
    ADC_ResetCalibration(ADC1);
    //检查校准寄存器是否复位完毕
    while(ADC_GetResetCalibrationStatus(ADC1));

    //开始校准
    ADC_StartCalibration(ADC1);
    //检测是否校准完毕
    while(ADC_GetCalibrationStatus(ADC1));
     
    //开启ADC1的软件转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


