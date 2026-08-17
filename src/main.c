#include "stm32h7xx.h"

void USART1_Init(void);
void USART1_SendChar(char c);
void USART1_SendString(const char *str);

int main(void)
{
    USART1_Init();

    USART1_SendString("READY\r\n");

    while (1)
    {
        if (USART1->ISR & USART_ISR_RXNE_RXFNE)
        {
            char c = USART1->RDR;
            USART1_SendChar(c);
        }
    }
}

void USART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;

    GPIOA->MODER &= ~(3 << (9 * 2));
    GPIOA->MODER |= (2 << (9 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOA->AFR[1] |= (7 << ((9 - 8) * 4));

    GPIOA->MODER &= ~(3 << (10 * 2));
    GPIOA->MODER |= (2 << (10 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((10 - 8) * 4));
    GPIOA->AFR[1] |= (7 << ((10 - 8) * 4));

    USART1->CR1 &= ~USART_CR1_UE;
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    USART1->BRR = 0x22B9;

    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}

void USART1_SendChar(char c)
{
    while (!(USART1->ISR & USART_ISR_TXE_TXFNF)) {}
    USART1->TDR = c;
}

void USART1_SendString(const char *str)
{
    while (*str)
    {
        USART1_SendChar(*str++);
    }
}

void HardFault_Handler(void)
{
    while (1) {}
}

void MemManage_Handler(void)
{
    while (1) {}
}

void BusFault_Handler(void)
{
    while (1) {}
}

void UsageFault_Handler(void)
{
    while (1) {}
}