# YEMU - Yet another EMUlator

一个简单的教学用模拟器，用于演示CPU执行过程。

## 项目结构

```
yemu/
├── include/          # 头文件
│   ├── common.h      # 公共定义
│   ├── cpu.h         # CPU接口
│   ├── inst.h        # 指令格式定义
│   └── memory.h      # 内存接口
├── src/              # 源文件
│   ├── main.c        # 主程序
│   ├── cpu.c         # CPU实现
│   └── memory.c      # 内存实现
├── build/            # 构建输出目录
├── Makefile          # 构建脚本
└── README.md         # 本文件
```

## 构建和运行

### 编译项目
```bash
make
```

### 运行程序
```bash
make run
```

### 清理构建
```bash
make clean
```

### 查看项目信息
```bash
make info
```

## 指令集

该模拟器实现了一个简单的4位指令集：

| 指令 | 格式 | 功能 | 编码 |
|------|------|------|------|
| mov rt,rs | R-type | R[rt] <- R[rs] | 0000 |
| add rt,rs | R-type | R[rt] <- R[rs] + R[rt] | 0001 |
| load addr | M-type | R[0] <- M[addr] | 1110 |
| store addr | M-type | M[addr] <- R[0] | 1111 |

## 示例程序

内存中预加载了一个计算 z = x + y 的程序：
- x = 16 (存储在地址5)
- y = 33 (存储在地址6)
- z = 0 (存储在地址7)

程序执行后，地址7将包含结果49。

## 设计特点

- **模块化设计**: CPU、内存和指令解码分离
- **函数化**: 使用函数替代宏定义，提高代码可维护性
- **类型安全**: 使用结构体进行指令解码，避免直接位操作
- **增量编译**: Makefile支持依赖追踪和增量编译
