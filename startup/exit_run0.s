    .syntax unified
    .cpu cortex-m7
    .thumb

/* Configure the power supply for maximum performance (Voltage Scale 0).
   Called by the startup code before .data/.bss are initialized,
   therefore implemented in pure assembly (no global data access). */

    .section .text.ExitRun0Mode,"ax",%progbits
    .global ExitRun0Mode
    .type   ExitRun0Mode, %function
ExitRun0Mode:
    ldr   r0, =0x58024800        /* PWR base */
    ldr   r1, [r0, #0x0C]        /* CR3: enable LDO */
    orr   r1, r1, #0x00000002
    str   r1, [r0, #0x0C]

    ldr   r1, [r0, #0x18]        /* D3CR: VOS = Scale 0 (0b00) */
    bic   r1, r1, #0x0000C000
    str   r1, [r0, #0x18]

1:  ldr   r1, [r0, #0x18]        /* wait VOSRDY (bit 13) */
    tst   r1, #0x00002000
    beq   1b

    bx    lr
    .size ExitRun0Mode, .-ExitRun0Mode
