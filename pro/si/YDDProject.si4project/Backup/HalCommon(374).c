#include "HalCommon.h"

static volatile uint32_t g_sysTimeCount = 0;
uint8_t g_commonBuff[4096];
static bool g_beepStart = false;
static uint32_t g_beepStopTime;

static void halIOUartSendbyte(uint8_t val);

//redirect "printf()"
int fputc(int ch, FILE *f)
{
	halIOUartSendbyte((uint8_t )ch);
	return ch;
}

static void halIOUartSendbyte(uint8_t val)
{
    uint16_t delay = 2;
    HalGPIOSetLevel(HAL_IO_UART_PIN, 0);
    HalWaitUs(delay); //8.6us,115200bps

    for (int i = 0; i < 8; i++)
    {
        HalGPIOSetLevel(HAL_IO_UART_PIN, val & 0x01);
        HalWaitUs(delay);
        val >>= 1;
    }
    HalGPIOSetLevel(HAL_IO_UART_PIN, 1);
    HalWaitUs(delay);
}

void HalBeepSet(uint16_t ms)
{
    g_beepStopTime = g_sysTimeCount + ms;
    g_beepStart = true;
    
    HalGPIOSetLevel(HAL_BEEP_CTRL_PIN, 1);
}

static void beepCtrl(void)
{
    if(g_beepStart && g_sysTimeCount > g_beepStopTime)
    {
        g_beepStart = false;
        HalGPIOSetLevel(HAL_BEEP_CTRL_PIN, 0);
    }
}

void HalTimerPast1ms(void)
{
    g_sysTimeCount++;
}

uint32_t HalGetSysTimeCount(void)
{
    return g_sysTimeCount;
}

void HalInterruptSet(bool enable)
{
    if(enable)
    {
        __enable_irq();
    }
    else
    {
        __disable_irq();
    }
}

static void periphClockInit(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC , ENABLE);
}

void HalCommonReboot(void)
{
    __set_FAULTMASK(1); //ÖÕÖ¹ËùÓÐÖÐ¶Ï
    NVIC_SystemReset();
}
/*
static void testPrint(void)
{
    static uint32_t lastTime = 0;
    HalRTCTime_t *time;

    if(g_sysTimeCount - lastTime > 1000)
    {
        time = HalRTCGetTime();
        printf("%04d-%02d-%02d %02d:%02d:%02d\n", time->year, time->month, time->day, time->hour, time->minute, time->second);
        lastTime = g_sysTimeCount;
    }
}
*/
static void halInit(void)
{
    HalGPIOConfig(HAL_IO_UART_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_BEEP_CTRL_PIN, HAL_IO_OUTPUT);
    //beep 
    HalBeepSet(100);
}

void HalCommonInitialize(void)
{
    int err;
    SystemInit();
    periphClockInit();
    HalGPIOInitialize();
    HalUartInitialize();
    HalTimerInitialize();
    HalSPIInitialize();
    HalADCInitialize();
    HalDACInitialize();
    err = HalRTCInit();
    halInit();
    printf(" RTC init %s\n", err < 0 ? "failed" : "success");
}

void HalCommonPoll(void)
{
    HalUartPoll();
    HalGPIOPoll();
    HalTimerPoll();
    HalSPIPoll();
    beepCtrl();
}

