// Headers and libraries
#include "project.h"
#include "ssd1306.h"
#include <stdio.h>

// Global variables
const int addressOLED = 0x3C; //SSD1306 address: 0011 1100

CY_ISR_PROTO(ISR_UART_rx_handler);

void initPrint(void)
{
    CyGlobalIntEnable; // Enable global interrupt
    I2COLED_Start(); // Start I2C for OLED setup
    display_init(addressOLED); // Initialize OLED display
    isr_uart_rx_StartEx(ISR_UART_rx_handler); // Start UART interrupt
    UART_1_Start(); // Start UART for Terminal setup
}

void handleByteReceived(uint8_t byteReceived) // Not currently in use
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

CY_ISR(ISR_UART_rx_handler) // UART interrupt routine
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

// Float parameter displayed with .3 precision i kg
void printTerminal(float res32)
{
    char uartBuffer[256];
    snprintf(uartBuffer, sizeof(uartBuffer), "%.3f", res32);
    UART_1_PutString("Weight: ");
    UART_1_PutString(uartBuffer);
    UART_1_PutString(" kg \r\n");
}

// Float parameter displayed with .3 precision i kg
void printOLED(float res32)
{
    // Prepare OLED
    display_clear();
    display_update();
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(5,0);
    gfx_print("-------------------");
    gfx_setCursor(5,50);
    gfx_print("-------------------");
    
    // Store weight in outputbuffer
    char outputBuffer[256];
    snprintf(outputBuffer, sizeof(outputBuffer), "%.3f kg", res32);
    
    // Display the outputbuffer
    gfx_setTextSize(2);
    gfx_setCursor(20,20);
    gfx_print(outputBuffer);
    
    // Send to OLED
    display_update();
}

void startMessage(void) // Not currently in use
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