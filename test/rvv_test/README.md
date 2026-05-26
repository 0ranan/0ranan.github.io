# RVV Intrinsic 编程测试题

本目录包含 **5 道 RISC‑V 向量扩展（RVV）Intrinsic 编程测试题**，供学生练习 RVV intrinsic 编程。每个测试文件中留有一个空函数，学生需要在标记区域内填入 RVV intrinsic 代码，通过编译并在 QEMU 中运行来验证结果。

## 测试题概述

| 题目 | 文件 | 待实现函数 | 操作描述 |
|------|------|------------|----------|
| test1 | `test1.c` | `rvv_sub_int32` | 整数数组减法 `c = a - b`（int32_t） |
| test2 | `test2.c` | `rvv_mul_int16_to_int32` | 16 位整数乘法，结果扩展为 32 位 `c = a * b`（int16_t → int32_t） |
| test3 | `test3.c` | `rvv_add_float32` | 单精度浮点数组加法 `c = a + b`（float） |
| test4 | `test4.c` | `rvv_mul_scalar_uint64` | 数组每个元素乘以标量 `c = a * scalar`（uint64_t） |
| test5 | `test5.c` | `rvv_sum_uint64` | 数组归约求和 `sum(a[0..n-1])`（uint64_t） |

每个文件同时提供了标量版本实现，用于自动校验 RVV 实现结果的正确性。

## 环境要求

- **交叉编译器**: `riscv64-unknown-linux-gnu-gcc`（需支持 RVV intrinsic，即 `riscv_vector.h`）
- **模拟器**: `qemu-riscv64`（需支持 V 扩展，即 `-cpu rv64,v=true`）
- **make**

### 推荐路径

| 工具 | 路径 |
|------|------|
| 编译器 | `/opt/riscv64/bin/riscv64-unknown-linux-gnu-gcc` |
| 系统根 | `/opt/riscv64/sysroot` |
| QEMU | `qemu-riscv64` |

如果工具安装在不同路径，可通过环境变量或命令行覆盖（见下文）。

## 使用 Makefile 构建与测试

### 构建并运行全部测试

```bash
make          # 等同于 make all
```

### 仅编译（不运行）

```bash
make build
```

### 运行全部测试

```bash
make test
```

### 运行单个测试

```bash
make test1    # 构建并运行 test1
make test2    # 构建并运行 test2
# ...
```

### 清理

```bash
make clean
```

### 自定义工具路径

```bash
# 覆盖编译器
make CC=/path/to/riscv64-unknown-linux-gnu-gcc

# 覆盖所有变量
make CC=/path/to/gcc QEMU=qemu-riscv64 SYSROOT=/path/to/sysroot

# 也可通过环境变量设置
export CC=/path/to/riscv64-unknown-linux-gnu-gcc
export QEMU=qemu-riscv64
export SYSROOT=/path/to/sysroot
make
```

### 编译选项

```bash
# 自定义 CFLAGS 或 QEMU 参数
make CFLAGS="-O0 -g -march=rv64gcv" QEMU_FLAGS="-cpu rv64,v=true"
```

生成的可执行文件位于 `build/` 目录下。

## 手工编译与运行

### 编译单个源文件

```bash
# 交叉编译（需要 --sysroot 或在 CFLAGS 中指定）
riscv64-unknown-linux-gnu-gcc -Wall -Wextra -O2 -march=rv64gcv \
    test1.c -o build/test1
```

**参数说明**:
- `-march=rv64gcv`: 必须启用 `v` 扩展以使用 RVV intrinsic
- `-O2`: 推荐启用优化（部分 RVV intrinsic 依赖优化生成高效代码）
- 如提示找不到 `riscv_vector.h`，请确认编译器版本支持 RVV（建议 GCC 13+ / Clang 17+）

### 用 QEMU 运行

```bash
qemu-riscv64 -cpu rv64,v=true -L /opt/riscv64/sysroot build/test1
```

**参数说明**:
- `-cpu rv64,v=true`: 启用 RISC‑V 64 位 + V（向量）扩展
- `-L /opt/riscv64/sysroot`: 指定 sysroot，QEMU 从该路径加载动态链接器和共享库

### QEMU 多核测试

单测程序默认使用单核即可。如需使用多线程 / 多核心运行测试，可在 QEMU 启动时指定核数：

```bash
# 使用 4 个 vCPU 启动 QEMU 用户模式（仅 Linux 用户模式支持 -smp）
qemu-riscv64 -cpu rv64,v=true \
    -L /opt/riscv64/sysroot \
    -smp 4 \
    build/test5

# 也可用环境变量设置
export QEMU_RISCV64_SMP=4
qemu-riscv64 -cpu rv64,v=true -L /opt/riscv64/sysroot build/test5
```

> **注意**: `-smp` 参数在 `qemu-riscv64`（linux-user 模式）下可用，用于模拟多核调度。如果测试程序本身是单线程的，多余的核不会带来加速，但可作为验证目的使用。

### 纯静态编译（无需 sysroot）

如希望运行时不依赖 sysroot 中的动态库，可静态编译：

```bash
riscv64-unknown-linux-gnu-gcc -static -O2 -march=rv64gcv test1.c -o build/test1
qemu-riscv64 -cpu rv64,v=true build/test1
```

此时不需要 `-L` 参数。

## 解题提示

- 每个测试的"学生编写代码区域"中通常留有**提示注释**，告知推荐使用的 intrinsic 函数
- RVV 使用 **CSR `vl`/`vtype`** 控制向量长度和元素类型，通过 `vsetvl_eXXmX` 类 intrinsic 配置
- 剩余元素使用 `n = n - vl` 循环处理
- 可参考 [RISC‑V Vector Extension Intrinsic Specification](https://github.com/riscv-non-isa/rvv-intrinsic-doc)

## 文件结构

```
rvv_test/
├── README.md     # 本文件
├── makefile      # 构建与测试脚本
├── test1.c       # 整数减法
├── test2.c       # 16位乘法（结果32位）
├── test3.c       # 浮点加法
├── test4.c       # 向量×标量乘法
└── test5.c       # 归约求和
```
