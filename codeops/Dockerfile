FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    qemu-system-arm \
    python3 \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# 复制 entrypoint 脚本到容器内
COPY codeops/entrypoint.sh /codeops/entrypoint.sh
RUN chmod +x /codeops/entrypoint.sh

# 设置容器默认入口
ENTRYPOINT ["/codeops/entrypoint.sh"]