#include <stdint.h>

#define UART0_DR (*(volatile unsigned int*)0x09000000)

static void vision_uart_print(const char* str) {
    while (*str != '\0') {
        UART0_DR = *str;
        str++;
    }
}

void secondary_vision_task(void) {
    vision_uart_print("[Core 1 - Vision]: AI accelerator & Camera pipeline active.\r\n");
    
    int frame_id = 0;
    while (1) {
        frame_id++;
        if (frame_id % 400000 == 0) {
            vision_uart_print("[Core 1 - Vision]: Processing frame [Target Locked] -> Sending offset.\r\n");
        }
    }
}

// ==========================================
// 帧缓冲与曝光增益表
// ==========================================
#define FRAME_BUFFER_SIZE 320

static uint8_t g_frame_buffer[FRAME_BUFFER_SIZE];

/* 拷贝一帧灰度像素：length 由相机驱动上报，未与缓冲区容量比较 */
void vision_copy_frame(const uint8_t *src, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++) {
        g_frame_buffer[i] = src[i];
    }
}

/* 计算一帧 RGB 字节数：乘法在 int 上溢出后再被截断成 uint16 */
uint16_t vision_frame_bytes(uint16_t width, uint16_t height)
{
    return (uint16_t)(width * height * 3);
}

/* 查表取曝光增益：索引来自前级检测结果，未做上界检查 */
static const int16_t k_exposure_gain[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

int16_t vision_lookup_gain(uint8_t index)
{
    return k_exposure_gain[index];
}