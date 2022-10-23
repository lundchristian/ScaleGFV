// Headers and libraries
#include "project.h"
#include "ssd1306.h"
#include <stdio.h>

// Prototypes and global variables
CY_ISR_PROTO(ISR_UART_rx_handler);
void handleByteReceived(uint8_t byteReceived);
void initProject();
void printTerminal(float);
void printOLED(float);
void clearTerminal(void);
void startMessage(void);
float convertV2F(void);
const int addressOLED = 0x3C; //SSD1306 address: 0011 1100

int main(void)
{
    initProject();

    while(1)
    {
        uint16_t res16 = 0;
        if (ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT))
        {
            // Returns 16-bit value of 12-bit resolution in 2's complement
            res16 = ADC_SAR_1_GetResult16();
            // result = 0x 0000SXXX XXXXXXXX (WHERE IS MSB?)
            // if result is positive
            // ~result += 1;
            // now to float to get grams ??
            char uartBuffer[256];
            snprintf(uartBuffer, sizeof(uartBuffer), "ADC result: %X \r\n", res16);
            UART_1_PutString(uartBuffer);
        }
        CyDelay(500);
        
        
        /*
        printTerminal(convertV2F());
        CyDelay(500);
        printOLED(convertV2F());
        CyDelay(500);
        */
    }
}

CY_ISR(ISR_UART_rx_handler)
{
    uint8_t bytesToRead = UART_1_GetRxBufferSize();
    while (bytesToRead > 0)
    {
        uint8_t byteReceived = UART_1_ReadRxData();
        UART_1_WriteTxData(byteReceived);
        handleByteReceived(byteReceived);
        bytesToRead--;
    }
}

void handleByteReceived(uint8_t byteReceived)
{
    switch(byteReceived)
    {
        case '1' :
        {
        }
        break;
        case '2' :
        {
        }
        break;
        default:
        break;
    }
}

void initProject()
{
    CyGlobalIntEnable; // Enable global interrupt
    isr_uart_rx_StartEx(ISR_UART_rx_handler); // Start UART interrupt
    UART_1_Start(); // Start UART for Terminal setup
    ADC_SAR_1_Start(); // Start ADC setup
    ADC_SAR_1_StartConvert(); // Start ADC conversion (free running mode)
    I2COLED_Start(); // Start I2C for OLED setup
    display_init(addressOLED); // Initialize OLED display
}

float convertV2F(void)
{
    uint16_t res16 = 0;
    float res32 = 0;
    if (ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT))
    {
        // Returns 16-bit value of 12-bit resolution in 2's complement
        res16 = ADC_SAR_1_GetResult16();
        // result = 0x 0000SXXX XXXXXXXX (WHERE IS MSB?)
        // if result is positive
        // ~result += 1;
        // now to float to get grams ??
        return res32; // Return weight as float
    }
    else return 404; // Return error value if conversion N/A
}

void printTerminal(float res32)
{
    char uartBuffer[256];
    snprintf(uartBuffer, sizeof(uartBuffer), "ADC result: %.1f \r\n", res32);
    UART_1_PutString(uartBuffer);
}

void printOLED(float res32)
{
    // Prepare OLED
    display_clear();
    display_update();
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(5,0);
    gfx_print("Digital Weight");
    gfx_setCursor(5,10);
    gfx_print("-------------------");
    gfx_setCursor(5,50);
    gfx_print("-------------------");
    
    // Store weight in an outputbuffer
    char outputBuffer[256];
    snprintf(outputBuffer, sizeof(outputBuffer), "%.1f", res32);
    
    // Display the outputbuffer
    gfx_setCursor(20,20);
    gfx_print("Weight => ");
    gfx_setCursor(80,20);
    gfx_print(outputBuffer);
    
    // Send to OLED
    display_update();
}

void startMessage(void)
{
    char outputBuffer[256];
    sprintf(outputBuffer, " Weight application initialized at (%s %s)\r\n", __DATE__, __TIME__);
    UART_1_PutString(outputBuffer);
    UART_1_PutString("==============================\r\n");
    UART_1_PutString("Press 1 to do X\r\n");
    UART_1_PutString("Press 2 to do Y\r\n");
    UART_1_PutString("==============================\r\n");
}

void clearTerminal(void)
{
    UART_1_PutString("\033c"); // Reset terminal screen
    CyDelay(20);
    UART_1_PutString("\033[2J"); // Clear terminal screen
    CyDelay(20);
}