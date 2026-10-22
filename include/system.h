#ifndef SYSTEM_H
#define SYSTEM_H

// 在现代编译器中，即使是 freestanding 环境，通常也支持包含基础类型定义
#include <stdint.h> 
#include <stddef.h>

// 后续可以在这里统一声明硬件基地址、全局结构体或寄存器映射
#define PERIPHERAL_BASE 0x3F000000

// UART0 在 BCM2836 上的外设基地址（手册写的是 0x3F201000）
#define UART0_PERIPHERAL_BASE 0x20201000

// 百分比转占空比：宏参数没有括号保护
#define PWM_DUTY_FROM_PERCENT(percent) percent * 1000U / 100U

#endif