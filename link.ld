MEMORY
{
    DDR (rwx) : ORIGIN = 0x00100000, LENGTH = 512M
}

SECTIONS
{
    /* 1. 代码与常量 */
    .text : {
        *(.vectors)
        *(.text*)
        *(.rodata*)
    } > DDR

    /* 2. 全局变量 */
    .data : { *(.data*) } > DDR
    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    /* 3. FreeRTOS 动态内存堆 */
    .heap (NOLOAD) : {
        . = ALIGN(8);
        _heap_start = .;
        . += 0x200000; /* 2MB 堆空间 */
        _heap_end = .;
    } > DDR

    /* 4. 核心 0 的栈 (64KB) */
    .stack_core0 (NOLOAD) : {
        . = ALIGN(8);
        _stack_core0_bottom = .;
        . += 0x10000;
        _stack_core0_top = .;
    } > DDR

    /* 5. 核心 1 的栈 (64KB) */
    .stack_core1 (NOLOAD) : {
        . = ALIGN(8);
        _stack_core1_bottom = .;
        . += 0x10000;
        _stack_core1_top = .;
    } > DDR
}