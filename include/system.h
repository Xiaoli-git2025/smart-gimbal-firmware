#ifndef SYSTEM_H
#define SYSTEM_H

// 在现代编译器中，即使是 freestanding 环境，通常也支持包含基础类型定义
#include <stdint.h> 
#include <stddef.h>

// 后续可以在这里统一声明硬件基地址、全局结构体或寄存器映射
#define PERIPHERAL_BASE 0x3F000000

#endif