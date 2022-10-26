// Headers and libraries
#include "project.h"
#include "weight.h"
#include "print.h"
#include <stdio.h>

int main(void)
{
    initPrint();
    initWeight();
    
    // Initial codeblock to set scale to zero
    CyDelay(500);
    convertV2G();
    initTARE();

    while(1)
    {
        printTerminal(convertV2G());
        printOLED(convertV2G());
        CyDelay(500);
    }
}