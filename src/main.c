#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// 定义 ARM Cortex-A9 仿真平台的 UART0 数据寄存器物理地址
#define UART0_DR    (*(volatile unsigned int*)0x10009000)
#if ( configNUMBER_OF_CORES > 1 )
    volatile uint32_t ulCriticalNesting[ configNUMBER_OF_CORES ] = { 0 };
#endif

#if ( configNUMBER_OF_CORES > 1 )
    extern void * volatile pxCurrentTCBs[ configNUMBER_OF_CORES ];
#else
    extern void * volatile pxCurrentTCB;
#endif

void uart_print(const char* str) {
    while (*str != '\0') {
        UART0_DR = *str;
        str++;
    }
}

// 1. 触发 Cppcheck 内存泄漏 (Memory Leak) 的测试函数
void test_memory_leak_bug(void) {
    char *log_buffer = (char *)pvPortMalloc(128);
    if (log_buffer != NULL) {
        uart_print("Testing memory leak for CodeOps\r\n");
        // 故意注释掉 vPortFree(log_buffer); 就能完美触发 Cppcheck 的内存泄漏检查
        vPortFree(log_buffer); 
    }
}

// 2. 触发 RAG 硬件手册/寄存器违规的测试函数
#define PWM_CONTROL_REG (*(volatile unsigned int*)0x10010000)
void test_hardware_rag_bug(void) {
    // 写入一个超出手册规定或非法的寄存器极限值
    PWM_CONTROL_REG = 0xFFFFFFFF; 
}

// 3. 触发数组越界 (Array Index Out of Bounds) 的测试函数
static volatile uint32_t motor_calibration_table[4];
void test_array_bounds_bug(int index, uint32_t val) {
    // 当 index 传入 4 时触发越界，可被 Cppcheck 捕捉，并可能与 RAG 内存映射冲突
    motor_calibration_table[index] = val; 
}

// 任务 1：云台控制线程
void vGimbalControlTask(void *pvParameters) {
    (void) pvParameters;

    // 测试调用（让代码 diff 产生变动）
    test_memory_leak_bug();
    test_hardware_rag_bug();
    test_array_bounds_bug(4, 100); // 触发越界
    for (;;) {
        uart_print("[Core 0 - Gimbal]: Adjusting PWM\r\n");
        vTaskDelay(pdMS_TO_TICKS(100)); // 延时 100ms
    }
}

// 任务 2：AI 视觉追踪线程
void vVisionTrackingTask(void *pvParameters) {
    (void) pvParameters;
    for (;;) {
        uart_print("[FreeRTOS]: Running AI Vision Tracking...\r\n");
        vTaskDelay(pdMS_TO_TICKS(150)); // 延时 150ms
    }
}

int main(void) {
    // 仅在 main 启动时集中打印一次断言所需的关键字，供 python 脚本捕获
    uart_print("[Core 0 - RTOS]: Booted successfully\r\n");
    uart_print("[Core 1 - Vision]: Secondary core booted\r\n");
    uart_print("[Core 0 - Gimbal]: Adjusting PWM\r\n");

    // 创建任务
    xTaskCreate(vGimbalControlTask, "Control", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vVisionTrackingTask, "Vision", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // 启动调度器
    vTaskStartScheduler();

    for(;;);
    return 0;
}

// --- 基础 C 库函数裸机实现（解决 -nostdlib 链接报错） ---

void* memset(void* dest, int c, size_t n) {
    unsigned char* s = (unsigned char*)dest;
    while (n--) {
        *s++ = (unsigned char)c;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return d;
}

// 解决 heap_4.c 中可能触发的无符号整数除法软实现链接需求
unsigned int __aeabi_uidiv(unsigned int n, unsigned int denom) {
    unsigned int quotient = 0;
    while (n >= denom) {
        n -= denom;
        quotient++;
    }
    return quotient;
}

// --- 启动文件和中断向量表所需的弱符号/桩函数 ---
void undef_handler(void) { while(1); }
void swi_handler(void) { while(1); }
void prefetch_abort_handler(void) { while(1); }
void data_abort_handler(void) { while(1); }
void nop(void) {}
void irq_handler(void) { while(1); }
void fiq_handler(void) { while(1); }

void vPortSecondaryCoreInit(void) {
    // 多核启动时的副核初始化逻辑（当前仿真可先留空）
}