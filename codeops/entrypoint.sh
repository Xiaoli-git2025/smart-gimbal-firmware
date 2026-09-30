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

# 3. 调用 serial_assert.py 进行自动化行为断言
if python3 codeops/serial_assert.py qemu_output.log; then
    echo "=== [CodeOps]: HIL Assertion Passed! ==="
    TEST_STATUS="success"
    EXIT_CODE=0
else
    echo "=== [CodeOps]: HIL Assertion Failed! ==="
    TEST_STATUS="failure"
    EXIT_CODE=1
fi

# 4. 结果回传实现闭环通知 (Result Callback)
# 树莓派 Agent 可以通过捕获容器执行的退出码或日志来感知结果。
echo "=== [CodeOps]: Pipeline finished with status: $TEST_STATUS ==="

exit $EXIT_CODE