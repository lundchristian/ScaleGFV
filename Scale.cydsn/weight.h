// Headers and libraries
#include "project.h"
#include <stdio.h>

// Global variables
float globalTARE = 0;
float globalWeight = 0;
const int loop = 10;

// Interrupt prototypes
CY_ISR_PROTO(ISR_pin_1_handler);

void initWeight(void)
{
    CyGlobalIntEnable; // Enable global interrupt
    isr_pin_1_StartEx(ISR_pin_1_handler); // Start Pin interrupt
    ADC_SAR_1_Start(); // Start ADC setup
    ADC_SAR_1_StartConvert(); // Start ADC conversion (free running mode)
}

// PSoC button interrupt to set TARE function value
CY_ISR(ISR_pin_1_handler)
{
    globalTARE = globalWeight;
}

// Returns a mean measurement
float convertV2G(void)
{
    uint16_t res16 = 0;
    float temp32 = 0;
    float res32 = 0;
    
    for (uint8_t i = 0; i < loop; i++)
    {
        if (ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT))
        {
            res16 = ADC_SAR_1_GetResult16();
            if (res16 & 0x0800)
            {
                temp32 = ~res16;
                temp32++;
                temp32 *= -1;
            }
        }
        temp32 = res16;
        res32 += temp32;
    }
    
    
    globalWeight = res32/loop;
    
    res32 = (res32/loop) - globalTARE;
    
    return res32/3.245/1000;
}