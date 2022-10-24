// Headers and libraries
#include "project.h"
#include "weight.h"
#include "print.h"
#include <stdio.h>

int main(void)
{
    initPrint();
    initWeight();

    while(1)
    {
        printTerminal(convertV2G());
        printOLED(convertV2G());
        CyDelay(500);
    }
}