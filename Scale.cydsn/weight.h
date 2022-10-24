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

// Returns a float mean measurement in kilograms
float convertV2G(void)
{
    uint16_t res16 = 0; // 16-bit to fetch measurement
    float temp32 = 0; // 32-bit to add all measurements
    float res32 = 0; // 32-bit to return mean of all measurements
    
    // Loop measurement ten times to get a more stable result
    for (uint8_t i = 0; i < loop; i++)
    {
        // Await to fetch measurement until last conversion is complete
        if (ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT))
        {
            res16 = ADC_SAR_1_GetResult16(); // Fetch measurement in 16-bit
            if (res16 & 0x0800) // Check for negative value in 2s-complement
            {
                temp32 = ~res16; // Bit-flip
                temp32++; // Add one
                temp32 *= -1; // Multiply by -1 to create negative value
            }
        }
        temp32 = res16; // Push 16-bit to 32-bit float if not negative 2s-complement
        res32 += temp32; // Add one measurement to 32-bit float
    }
    
    globalWeight = res32/loop; // Set globalweight to enable TARE functionality
    
    res32 = (res32/loop) - globalTARE; // Adjust return value to TARE null-point value
    
    // Divide by 3.245 to get precise conversion to grams
    // Divide by 1000 to get conversion to kilograms
    return res32/3.245/1000;
}
