#define UART0_DR (*(volatile unsigned int*)0x09000000)
static void local_print(const char* str) { while (*str != '\0') { UART0_DR = *str; str++; } }

void master_gimbal_task(void) {
    local_print("[Core 0 - Control]: Gimbal PID & IMU loop started.\r\n");
    
    int loop_counter = 0;
    while (1) {
        loop_counter++;
        // 模拟高频实时控制循环 (例如 1kHz 姿态调整)
        if (loop_counter % 300000 == 0) {
            local_print("[Core 0 - Control]: Reading IMU -> Adjusting PID motors...\r\n");
        }
        
        // 模拟兼顾调度从核的视觉数据
        if (loop_counter == 900000) {
            local_print("[Core 0 - Inter-Core]: Syncing bounding box with Vision Core...\r\n");
        }
    }
}