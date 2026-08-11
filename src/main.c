#include "stm32h7xx.h"
#include <string.h>

#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 300

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t rx_index = 0;
volatile uint8_t rx_complete = 0;

const char* prefix = "Stm: ";
const uint8_t prefix_len = 5;

// Инициализация USART1 на PA9(TX) и PA10(RX)
void UART_Init(void) {
    // 1. Включаем тактирование
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;    // GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // USART1 clock
    
    // 2. Настройка пинов PA9 и PA10 как AF7 (USART1)
    // Очищаем MODER для PA9 и PA10
    GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
    // Устанавливаем альтернативную функцию (0b10)
    GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
    
    // Настройка AF для PA9 и PA10 (AF7 = USART1)
    GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4)); // PA9
    GPIOA->AFR[1] |= (7 << ((9 - 8) * 4));    // AF7 для PA9
    
    GPIOA->AFR[1] &= ~(0xF << ((10 - 8) * 4)); // PA10
    GPIOA->AFR[1] |= (7 << ((10 - 8) * 4));    // AF7 для PA10
    
    // Push-pull, высокая скорость
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9_Msk | GPIO_OTYPER_OT10_Msk);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10);
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9_Msk | GPIO_PUPDR_PUPD10_Msk);
    
    // 3. Настройка USART1
    // Отключаем USART для настройки
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    
    // 115200 бод при частоте 64MHz (APB2)
    // BRR = 64_000_000 / 115200 = 555
    USART1->BRR = 555;
    
    // Настройка: 8 бит, 1 стоп, без четности
    USART1->CR1 = USART_CR1_UE |      // Включаем USART
                  USART_CR1_TE |      // Включаем передатчик
                  USART_CR1_RE |      // Включаем приемник
                  USART_CR1_RXNEIE;   // Включаем прерывание по приему
    
    // 4. Настройка прерываний
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
}

// Отправка строки
void UART_SendString(const char* str) {
    while (*str) {
        while (!(USART1->ISR & USART_ISR_TXE_TXFNF));
        USART1->TDR = *str++;
    }
}

// Отправка буфера
void UART_SendBuffer(const uint8_t* buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        while (!(USART1->ISR & USART_ISR_TXE_TXFNF));
        USART1->TDR = buffer[i];
    }
}

// Обработчик прерывания
void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE_RXFNE) {
        uint8_t received = USART1->RDR;
        
        if (received == '\r' || received == '\n') {
            rx_buffer[rx_index] = '\0';
            rx_complete = 1;
            rx_index = 0;
        } else if (rx_index < RX_BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = received;
        }
    }
}

// Обработка эхо
void ProcessEcho(void) {
    if (rx_complete) {
        rx_complete = 0;
        
        uint16_t msg_len = strlen((char*)rx_buffer);
        uint16_t total_len = prefix_len + msg_len + 2;
        
        if (total_len < TX_BUFFER_SIZE) {
            memcpy(tx_buffer, prefix, prefix_len);
            memcpy(tx_buffer + prefix_len, rx_buffer, msg_len);
            tx_buffer[prefix_len + msg_len] = '\r';
            tx_buffer[prefix_len + msg_len + 1] = '\n';
            
            UART_SendBuffer(tx_buffer, total_len);
        }
        
        memset(rx_buffer, 0, RX_BUFFER_SIZE);
    }
}

// Простая задержка
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 10000; i++) {
        __NOP();
    }
}

int main(void) {
    UART_Init();
    
    // Приветствие
    UART_SendString("\r\n=== UART Echo Test ===\r\n");
    UART_SendString("Send any text and press Enter\r\n");
    UART_SendString("Response: Stm: [your text]\r\n\r\n");
    
    while (1) {
        ProcessEcho();
        delay_ms(10);
    }
}