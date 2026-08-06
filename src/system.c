#include "stm32h723xx.h"
#include <sys/stat.h>
#include <errno.h>

void SystemInit(void) {
    // Включить FPU
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
}


void _init(void) {
}

int _write(int file, char *ptr, int len) {
    (void)file;  // Подавляем предупреждения
    (void)ptr;
    return len;
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
