/* STM32H723VGT6 (WeAct board) linker script */
ENTRY(Reset_Handler)

_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);   /* stack top: end of AXI SRAM (RAM_D1) */

MEMORY
{
  ITCMRAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64K
  DTCMRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1  (rwx) : ORIGIN = 0x24000000, LENGTH = 320K
  RAM_D2  (rwx) : ORIGIN = 0x30000000, LENGTH = 32K
  RAM_D3  (rwx) : ORIGIN = 0x38000000, LENGTH = 16K
  FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
}

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector))
    . = ALIGN(8);
  } >FLASH

  .text :
  {
    . = ALIGN(8);
    *(.text)
    *(.text*)
    . = ALIGN(8);
  } >FLASH

  .init :
  {
    . = ALIGN(4);
    KEEP(*(.init))
    . = ALIGN(4);
  } >FLASH

  .fini :
  {
    . = ALIGN(4);
    KEEP(*(.fini))
    . = ALIGN(4);
  } >FLASH

  .preinit_array :
  {
    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;
  } >FLASH

  .init_array :
  {
    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    __init_array_end = .;
  } >FLASH

  .fini_array :
  {
    . = ALIGN(4);
    __fini_array_start = .;
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    __fini_array_end = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(8);
    _etext = .;
  } >FLASH

  .data :
  {
    . = ALIGN(8);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(8);
    _edata = .;
  } >RAM_D1 AT> FLASH
  _sidata = LOADADDR(.data);

  .bss (NOLOAD) :
  {
    . = ALIGN(8);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    _ebss = .;
  } >RAM_D1

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
