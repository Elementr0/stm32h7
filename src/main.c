#include "stm32h723xx.h"

void SystemInit(void) {
    // Включить FPU
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
}

int main(void) {
    while(1) {
        // Ваш код
    }
}
