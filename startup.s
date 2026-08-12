.global _start
.section .vectors, "ax"

_start:
    b   reset_handler        @ 0x00: Reset
    b   undef_handler        @ 0x04: Undefined Instruction
    b   swi_handler          @ 0x08: Software Interrupt
    b   prefetch_abort_handler @ 0x0C: Prefetch Abort
    b   data_abort_handler   @ 0x10: Data Abort
    b   nop                  @ 0x14: Reserved
    b   irq_handler          @ 0x18: IRQ (中断处理)
    b   fiq_handler          @ 0x1C: FIQ

reset_handler:
    @ ==========================================
    @ 第一步：读取 CP15 的 MPIDR 寄存器，获取当前 CPU ID
    @ ==========================================
    mrc p15, 0, r0, c0, c0, 5    @ 读取 MPIDR 寄存器到 r0
    ands r0, r0, #0x03           @ 取出最低两位，判断是 Core 0 还是 Core 1

    @ ==========================================
    @ 第二步：根据 CPU ID 进行分流处理
    @ ==========================================
    cmp r0, #0
    beq core0_init               @ 如果 ID == 0，跳去 Core 0 的专属初始化
    bne core1_init               @ 如果 ID == 1，跳去 Core 1 的专属初始化

@ ==========================================
@ 核心 0 的初始化流程 (负责繁重的系统准备工作)
@ ==========================================
core0_init:
    @ 1. 设置 Core 0 的专属栈指针 (从 link.ld 里拿符号)
    ldr sp, =_stack_core0_top

    @ 2. 清空 BSS 段 (把全局未初始化变量清零，这活只能让一个核干一次)
    ldr r1, =__bss_start
    ldr r2, =__bss_end
    mov r3, #0
bss_loop_core0:
    cmp r1, r2
    strlt r3, [r1], #4
    blt bss_loop_core0

    @ 3. 清理完毕，Core 0 正式跳入 C 语言的 main 函数！
    bl main
    b  hang

@ ==========================================
@ 核心 1 的初始化流程 (副核，轻量级唤醒)
@ ==========================================
core1_init:
    @ 1. 设置 Core 1 的专属栈指针 (使用我们在 link.ld 里刚加的栈顶)
    ldr sp, =_stack_core1_top

    @ 2. 副核不需要清 BSS，也不需要重复初始化硬件
    @ 它只需要进入 FreeRTOS 的副核启动挂起函数或等待调度
    bl vPortSecondaryCoreInit    @ 调用 FreeRTOS 多核副核引导函数

core1_wait:
    wfe                          @ 睡眠等待事件，直到 Core 0 通过核间中断唤醒它
    b  core1_wait

hang:
    b  hang
    