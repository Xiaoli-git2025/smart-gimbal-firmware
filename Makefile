# ==========================================
# 工具链与编译选项
# ==========================================
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy

# CPU_FLAGS = -mcpu=cortex-a9 -mfpu=vfpv3-d16 -mfloat-abi=hard -marm
# 将 vfpv3-d16 改为 vfpv3，允许使用全部 32 个双精度寄存器 (D0-D31)
CPU_FLAGS = -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -marm

CFLAGS = $(CPU_FLAGS) -O0 -g3 -Wall -Wextra \
          -I./include \
          -I./src/freertos/include \
          -I./src/freertos/portable/GCC/ARM_CA9

# ==========================================
# 显式列出所有源文件路径（已更正 heap_4.c 路径）
# ==========================================
C_SOURCES = \
    src/main.c \
    src/gimbal_core.c \
    src/vision_core.c \
    src/rtos_tasks.c \
    src/freertos/tasks.c \
    src/freertos/list.c \
    src/freertos/queue.c \
    src/freertos/timers.c \
    src/freertos/portable/GCC/ARM_CA9/port.c \
    src/freertos/portable/MemMang/heap_4.c

ASM_SOURCES = \
    startup.s \
    src/freertos/portable/GCC/ARM_CA9/portASM.S

# 将所有源文件转换为对应的 .o 路径
OBJECTS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.S=.o)

# ==========================================
# 构建规则
# ==========================================
all: firmware.elf firmware.bin

firmware.elf: $(OBJECTS)
	$(CC) $(CPU_FLAGS) -Tlink.ld $(OBJECTS) -o $@ -nostdlib

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

# 显式为正确的 MemMang 路径提供编译规则
src/freertos/portable/MemMang/heap_4.o: src/freertos/portable/MemMang/heap_4.c
	$(CC) $(CFLAGS) -c $< -o $@

# 通用编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find . -name "*.o") firmware.elf firmware.bin