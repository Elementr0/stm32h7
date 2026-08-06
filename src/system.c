#include "stm32h723xx.h"
#include <sys/stat.h>
#include <errno.h>

/* Ядро работает на HSI 64 МГц (нет настройки PLL) */
uint32_t SystemCoreClock = 64000000;

/* Аппаратные адреса SWO Trace Funnel (нет в stm32h723xx.h) */
#define SWO_LAR        (0x5C003FB0UL)   /* SWO Lock Access Register        */
#define SWO_CODR       (0x5C003010UL)   /* SWO Current Output Divider      */
#define SWO_SPPR       (0x5C0030F0UL)   /* SWO Selected Pin Protocol       */
#define SWTF_LAR       (0x5C004FB0UL)   /* SWO Trace Funnel Lock           */
#define SWTF_CTRL      (0x5C004000UL)   /* SWO Trace Funnel Control        */
#define SWO_HZ         (2000000UL)      /* скорость SWO */

void SystemInit(void) {
    // Включить FPU
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

    // Такт ядра трассы (D1/D3) — TRACECKEN | CKD1EN | CKD3EN
    DBGMCU->CR |= DBGMCU_CR_DBG_TRACECKEN | DBGMCU_CR_DBG_CKD1EN | DBGMCU_CR_DBG_CKD3EN;

    // Разблокировать SWO и SWO Trace Funnel
    *(volatile uint32_t*)SWO_LAR  = 0xC5ACCE55UL;
    *(volatile uint32_t*)SWTF_LAR = 0xC5ACCE55UL;

    // Делитель SWO: на H72x/H73x такт SWO = яд/2
    *(volatile uint32_t*)SWO_CODR = (SystemCoreClock / 2UL / SWO_HZ) - 1UL;
    // Протокол: NRZ-выход
    *(volatile uint32_t*)SWO_SPPR = 0x00000002UL;
    // Включить SWO Trace Funnel
    *(volatile uint32_t*)SWTF_CTRL |= 0x00000001UL;

    // Пин PB3 = SWO (AF0, high speed)
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    GPIOB->MODER   &= ~(0x3UL << 6);
    GPIOB->MODER   |=  (0x2UL << 6);          /* AF   */
    GPIOB->PUPDR   &= ~(0x3UL << 6);          /* no pull-up/pull-down */
    GPIOB->OSPEEDR |=  (0x3UL << 6);          /* very high speed */
    GPIOB->AFR[0]  &= ~(0xFUL << 12);         /* AF0 */

    // ITM: порт 0, SWO + шину
    ITM->LAR = 0xC5ACCE55UL;   /* unlock ITM control writes */
    ITM->TCR = ITM_TCR_ITMENA_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_DWTENA_Msk |
               (1UL << ITM_TCR_TraceBusID_Pos);
    ITM->LAR = 0x0;              /* re-lock */
    ITM->TER = 0x1;

    // Включить счётчик циклов для таймаута SWO
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
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
