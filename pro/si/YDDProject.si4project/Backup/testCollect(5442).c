#include "Sys.h"

void TestCollectPoll(void)
{
    static uint32_t oldTime = 0;
    static uint8_t count = 0;
    uint16_t data[8];

    if(count < 120)
    {
        data[0] = 188;
        data[1] = 15;
        data[2] = 14;
        data[3] = 189;
        
        data[4] = 199;
        data[5] = 12;
        data[6] = 11;
        data[7] = 201;

        if(SysTimeHasPast(oldTime, 1000))
        {
            SysRawDataWrite(count * sizeof(data), (uint8_t *)data, sizeof(data));
            count++;
            printf("raw data: %d\n", count);
        }
    }
    
}

void TestCollectInit(void)
{
}

