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