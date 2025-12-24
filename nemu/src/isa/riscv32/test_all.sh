#!/bin/bash
cd ../../../../am-kernels/tests/cpu-tests/

# 自动从 tests 目录获取所有测试用例
tests=$(ls tests/*.c | xargs -n1 basename | sed 's/\.c$//' | sort)

for test in $tests; do
    echo "Testing $test..."
    result=$(printf "c\nq\n" | timeout 15 make ARCH=riscv32-nemu ALL=$test run 2>&1)
    
    if echo "$result" | grep -q "HIT GOOD TRAP"; then
        echo "  ✓ PASS"
    elif echo "$result" | grep -q "invalid opcode"; then
        echo "  ✗ FAIL - Missing instruction"
        pc=$(echo "$result" | grep "invalid opcode(PC" | sed 's/.*PC = \(0x[0-9a-f]*\).*/\1/')
        inst=$(echo "$result" | grep -A2 "invalid opcode" | tail -1 | awk '{print $1}')
        echo "    PC: $pc, Instruction: $inst"
    else
        echo "  ? UNKNOWN"
    fi
done
