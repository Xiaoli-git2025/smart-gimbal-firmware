#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configNUMBER_OF_CORES    2   // 明确告诉 RTOS，我们现在由 2 个核心共管
#define configUSE_PREEMPTION                    1
#define configUSE_TICKLESS_IDLE                 0
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configUSE_PASSIVE_IDLE_HOOK 0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUNIQUE_INTERRUPT_PRIORITIES 32  // Cortex-A9 常见的中断优先级数，或者根据你的硬件平台设置
#define configCPU_CLOCK_HZ                      ( 100000000UL )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 ) // 1ms 系统心跳
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 16 * 1024 ) ) // 16KB 堆空间
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1

/* 包含常用的 API 函数 */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelay                      1

/* --- Cortex-A9 GIC 中断控制器配置 --- */
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS         0x1F000100  // 根据常见的 Cortex-A9 平台设置（如 RealView PBX A9 的 GIC 内存基址，或根据你的平台调整）
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET 0x0100      // CPU 接口端偏移地址

/* --- 中断优先级与安全掩码配置 --- */
#define configUNIQUE_INTERRUPT_PRIORITIES               32
#define configMAX_API_CALL_INTERRUPT_PRIORITY           18  // 必须大于 (configUNIQUE_INTERRUPT_PRIORITIES / 2)，且不能为 0

/* --- 系统 Tick 中断设置 --- */
// 如果你暂时没有实现自定义的 Tick 初始化函数，可以先配为一个空宏或指向你的定时器初始化函数
#define configSETUP_TICK_INTERRUPT()                    /* 可以在这里填入你的定时器初始化代码，或暂时留空若后续处理 */

#endif /* FREERTOS_CONFIG_H */