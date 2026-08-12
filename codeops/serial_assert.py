import sys
import os

def assert_logs(log_path):
    if not os.path.exists(log_path):
        print(f"[Error]: Log file {log_path} not found!")
        sys.exit(1)

    with open(log_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    print("--- Captured QEMU Output ---")
    print(content[:500]) # 打印前 500 字符预览
    print("----------------------------")

    # 核心断言检查：验证 Core 0 和 Core 1 是否都成功启动并输出了关键日志
    assertions = [
        "[Core 0 - RTOS]: Booted successfully",
        "[Core 1 - Vision]: Secondary core booted",
        "[Core 0 - Gimbal]: Adjusting PWM"
    ]

    for assertion in assertions:
        if assertion not in content:
            print(f"[Assertion Failed]: Missing expected log pattern -> '{assertion}'")
            sys.exit(1)
        else:
            print(f"[Assertion Passed]: Found -> '{assertion}'")

    print("[Success]: All hardware behavioral assertions passed!")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 serial_assert.py <log_file>")
        sys.exit(1)
    
    assert_logs(sys.argv[1])