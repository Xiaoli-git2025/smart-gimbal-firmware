#!/bin/bash
set -e

echo "=== [CodeOps]: Starting SMP build & simulation pipeline ==="

# 1. 直接调用 Makefile 清理并编译生成 firmware.elf 和 firmware.bin
make clean
make

echo "=== [CodeOps]: Firmware compiled successfully. Launching SMP QEMU simulation ==="

# 2. 禁声卡并使用完美匹配 UART 地址 (0x10009000) 的 realview-pb-a9 双核仿真平台
QEMU_AUDIO_DRV=none timeout 3 qemu-system-arm -M realview-pbx-a9 -m 256M -smp 2 -nographic -kernel firmware.elf > qemu_output.log 2>&1 || true

echo "=== [CodeOps]: Simulation finished. Running Python assertions ==="

# 3. 调用你写好的 serial_assert.py 进行自动化行为断言
python3 codeops/serial_assert.py qemu_output.log