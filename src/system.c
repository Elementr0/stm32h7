#include "stm32h723xx.h"
#include <sys/stat.h>
#include <errno.h>

#define CPU_Freq 64000000U

#define Tick_Freq_1kHz 1000u
#define Tick_Freq_Default Tick_Freq_1kHz
#define Tickrate CPU_Freq / Tick_Freq_Default

volatile uint32_t tick = 0;
void SysTick_Handler(void)
{
    tick++;
}

void delay_ms(uint32_t ms)
{
    uint32_t target = tick + ms;
    while (tick < target) {
        // Ожидание
    }
}

void SystemInit(void) {
    // Включить FPU
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
    SysTick_Config(Tickrate); // 1мс
}

#define SWO_TIMEOUT_CYCLES 16000U

static void swo_putc(char c)
{
    if (((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0UL) ||
        ((ITM->TER & 1UL) == 0UL)) {
        return;
    }
    uint32_t start = DWT->CYCCNT;
    while (ITM->PORT[0U].u32 == 0UL) {
        if ((DWT->CYCCNT - start) > SWO_TIMEOUT_CYCLES) {
            return;
        }
    }
    ITM->PORT[0U].u8 = (uint8_t)c;
}


void _init(void) {
}

int __io_putchar(int ch) {
    swo_putc((char)ch);
    return ch;
}

int _write(int file, char *ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        swo_putc(ptr[i]);
    }
    return len;
}

void swo_printf(const char *str) {
    while (*str) {
        __io_putchar(*str++);
    }
}

int _read(int file, char *ptr, int len) {
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _close(int file) {
    (void)file;
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

void _exit(int status) {
    (void)status;
    while(1);
}

void *_sbrk(int incr) {
    extern char _ebss;
    static char *heap_end;
    char *prev_heap_end;
    
    if (heap_end == 0) {
        heap_end = &_ebss;
    }
    
    prev_heap_end = heap_end;
    heap_end += incr;
    
    return (void*)prev_heap_end;
}
