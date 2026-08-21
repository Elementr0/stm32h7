#include "stm32h7xx.h"

#define PIN 3

void delay_ms(uint32_t ms);


int main(void)
{
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    
    GPIOE->MODER &= ~(0x3 << (PIN * 2));
    GPIOE->MODER |= (0x1 << (PIN * 2));
    GPIOE->OSPEEDR &= ~(0x3 << (PIN * 2));
    GPIOE->OSPEEDR |= (0x2 << (PIN * 2));
    GPIOE->PUPDR &= ~(0x3 << (PIN * 2));
    
    GPIOE->BSRR = (0x1 << (PIN + 16)); // Выключен

    while (1)
    {
        GPIOE->BSRR = (0x1 << PIN);
        delay_ms(1500); // 500 мс
        
        GPIOE->BSRR = (0x1 << (PIN + 16));
        delay_ms(1500); // 500 мс
    }
};