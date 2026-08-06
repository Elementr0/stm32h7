#include "stm32h723xx.h"
#include "main.h"



int main(void) {
    swo_printf("Hello from STM32H723 via SWO!\r\n");
    swo_printf("System started successfully!\r\n");

    while(1) {
        swo_printf("Running main loop...\r\n");
    }
}
