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
        
        // ========================================================================
        // 【BUG 引入点 - Memory Leak】
        // 模拟兼顾调度从核的视觉数据：动态分配内存用于存放接收到的同步数据包，
        // 但在处理完同步逻辑后，忘记调用 free(packet) 释放堆内存！
        // 每当 loop_counter 达到 900000 的倍数时，就会泄漏一块内存，直至 Heap 耗尽崩溃。
        // ========================================================================
        if (loop_counter % 900000 == 0) {
            VisionSyncPacket_t *packet = (VisionSyncPacket_t *)malloc(sizeof(VisionSyncPacket_t));
            if (packet != NULL) {
                packet->bbox_x = 120;
                packet->bbox_y = 240;
                packet->timestamp = loop_counter;
                
                local_print("[Core 0 - Inter-Core]: Syncing bounding box with Vision Core...\r\n");
                
                // 处理完 packet 后漏掉了 free(packet); !!!
            }
        }
    }
}

// ==========================================
// 姿态闭环 PID 与急停实现
// ==========================================
#define PWM_CONTROL_REG (*(volatile unsigned int*)0x10010000)
#define PWM_MAX_DUTY    1000U

typedef struct {
    int32_t kp;
    int32_t ki;
    int32_t kd;
} PidGains_t;

static PidGains_t g_pid_gains = { 1, 0, 0 };

/* 单步 PID：比例项的分母由增益相加得到，增益配成 0 时直接除零 */
int32_t gimbal_pid_step(int32_t error, int32_t last_error)
{
    int32_t p_term = (error * g_pid_gains.kp) / (g_pid_gains.kp + g_pid_gains.kd);
    int32_t d_term = (error - last_error) * g_pid_gains.kd;
    int32_t output = p_term + d_term;

    /* 直接写 PWM 寄存器，没有做 0..PWM_MAX_DUTY 的饱和限幅 */
    PWM_CONTROL_REG = (unsigned int)output;
    return output;
}

/* 急停：只清了软件标志位，PWM 输出寄存器没有真正关断 */
void gimbal_emergency_stop(int *running)
{
    *running = 0;
}