#include "stm32h723xx.h"
#include "main.h"



int main(void){
	RCC->AHB4ENR = RCC_AHB4ENR_GPIOEEN;
	GPIOE->MODER &= ~(0x3 << (3*2));
	GPIOE->MODER |=(0x1 <<(3*2));
	// Настраиваем скорость: высокая (можно также 0x1, 0x2 или 0x3)
	GPIOE->OSPEEDR &= ~(0x3 << (3 * 2));
	GPIOE->OSPEEDR |= (0x2 << (3 * 2));  // 0x2 = высокая скорость
	// Отключаем подтяжку (можно также включить, если нужно)
	GPIOE->PUPDR &= ~(0x3 << (3 * 2));  // 0 = без подтяжки
	GPIOE->BSRR = (0x1 << 3); 


	while(1) {
	}
}
