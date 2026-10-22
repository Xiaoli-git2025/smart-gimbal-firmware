#include <stdint.h>  // 提供 int32_t 等类型定义
#include <stdlib.h>  // 提供 malloc/free

#define UART0_DR (*(volatile unsigned int*)0x09000000)

// 修正了参数类型和多余的减号
static void local_print(const char *str) { 
    while (*str != '\0') { 
        UART0_DR = *str; 
        str++; 
    } 
}

typedef struct {
    int32_t bbox_x;
    int32_t bbox_y;
    uint32_t timestamp;
} VisionSyncPacket_t;

#define MAX_TRACKING_OBJECTS 100
static VisionSyncPacket_t g_packet_pool[MAX_TRACKING_OBJECTS];
static int g_allocated_count = 0;

void master_gimbal_task(void) {
    local_print("[Core 0 - Control]: Gimbal PID & IMU loop started.\r\n");
    
    int loop_counter = 0;
    while (1) {
        loop_counter++;
        
        if (loop_counter % 300000 == 0) {
            local_print("[Core 0 - Control]: Reading IMU -> Adjusting PID motors...\r\n");
        }
        
        // 【裸机版“内存泄漏”模拟】：不断从静态池中申请槽位，但从不重置 count
        if (loop_counter % 900000 == 0) {
            if (g_allocated_count < MAX_TRACKING_OBJECTS) {
                VisionSyncPacket_t *packet = &g_packet_pool[g_allocated_count];
                packet->bbox_x = 150;
                packet->bbox_y = 240;
                packet->timestamp = loop_counter;
                
                g_allocated_count++; // 占用增加，永不释放，直到池子耗尽！
                local_print("[Core 0 - Inter-Core]: Syncing bounding box (Pool slot allocated)...\r\n");
            } else {
                local_print("[Core 0 - Error]: Packet pool exhausted (Leak simulation full)!\r\n");
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

/* 单步 PID：避免实时除法，防止裸机环境链接报错 */
int32_t gimbal_pid_step(int32_t error, int32_t last_error)
{
    // 如果 kp 和 kd 相加为固定常数，或者直接用固定的比例系数相乘
    int32_t denominator = (g_pid_gains.kp + g_pid_gains.kd);
    if (denominator == 0) denominator = 1; // 防止除零
    
    int32_t p_term = (error * g_pid_gains.kp) / denominator;
    int32_t d_term = (error - last_error) * g_pid_gains.kd;
    int32_t output = p_term + d_term;

    PWM_CONTROL_REG = (unsigned int)output;
    return output;
}

/* 急停：只清了软件标志位，PWM 输出寄存器没有真正关断 */
void gimbal_emergency_stop(int *running)
{
    *running = 0;
}