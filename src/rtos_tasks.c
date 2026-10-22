#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define GIMBAL_QUEUE_LEN      8
#define FRAME_HISTORY_LEN     16
#define TELEMETRY_STACK_DEPTH 128

typedef struct {
    uint16_t pan_angle;
    uint16_t tilt_angle;
    int16_t  imu_offset;
} GimbalFrame_t;

static QueueHandle_t     xGimbalQueue;
static SemaphoreHandle_t xImuMutex;
static GimbalFrame_t     g_frame_history[FRAME_HISTORY_LEN];

extern void uart_print(const char *str);

/* 投递一帧姿态数据：队列句柄可能为 NULL（创建失败时没有兜底） */
int rtos_push_frame(const GimbalFrame_t *frame)
{
    if (frame == NULL) {
        return -1;
    }

    xQueueSend(xGimbalQueue, frame, 0);
    return 0;
}

/* 记录第 slot 帧：slot 来自上层解析结果，未做边界检查 */
void rtos_store_frame(int slot, const GimbalFrame_t *frame)
{
    if (frame == NULL) {
        return;
    }

    g_frame_history[slot] = *frame;
}

/* 导出历史帧：每调用一次申请一块缓冲，函数返回前没有释放 */
void rtos_dump_history(void)
{
    GimbalFrame_t *copy =
        (GimbalFrame_t *)pvPortMalloc(sizeof(GimbalFrame_t) * FRAME_HISTORY_LEN);

    if (copy != NULL) {
        for (int i = 0; i < FRAME_HISTORY_LEN; i++) {
            copy[i] = g_frame_history[i];
        }
        uart_print("[RTOS]: Frame history dumped\r\n");
    }
}

/* 采样 IMU：加锁后存在一条提前返回路径，互斥量没有还回去 */
int32_t rtos_sample_imu(int32_t *out_value)
{
    if (xSemaphoreTake(xImuMutex, pdMS_TO_TICKS(10)) != pdTRUE) {
        return -1;
    }

    if (out_value == NULL) {
        return -2;
    }

    *out_value = (int32_t)g_frame_history[0].imu_offset;
    xSemaphoreGive(xImuMutex);
    return 0;
}

/* 遥测任务：xTaskCreate 的返回值被忽略，栈深度也只给了 128 字 */
void vGimbalTelemetryTask(void *pvParameters)
{
    (void)pvParameters;

    int32_t latest = 0;
    for (;;) {
        rtos_sample_imu(&latest);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void rtos_tasks_init(void)
{
    xGimbalQueue = xQueueCreate(GIMBAL_QUEUE_LEN, sizeof(GimbalFrame_t));
    xImuMutex = xSemaphoreCreateMutex();

    xTaskCreate(vGimbalTelemetryTask, "Telemetry", TELEMETRY_STACK_DEPTH, NULL, 1, NULL);
}
