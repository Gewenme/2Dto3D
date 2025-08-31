#!/bin/bash

# 双目标定程序测试脚本
# Stereo Calibration Program Test Script

echo "=== 双目标定程序测试 (Stereo Calibration Program Test) ==="
echo

# 检查程序是否存在
PROGRAM="./build/bin/stereo_calibration_program"
if [ ! -f "$PROGRAM" ]; then
    echo "错误：程序未找到，请先编译 (Error: Program not found, please build first)"
    echo "运行：cd build && make stereo_calibration_program"
    exit 1
fi

# 测试1：使用默认路径
echo "测试1：使用默认路径 (Test 1: Using default paths)"
echo "----------------------------------------"
$PROGRAM

echo
echo "测试1完成 (Test 1 completed)"
echo

# 测试2：使用自定义路径
echo "测试2：使用自定义路径 (Test 2: Using custom paths)"
echo "----------------------------------------"
CUSTOM_OUTPUT="custom_test_output_$(date +%s)"
$PROGRAM calibration_data/input/left calibration_data/input/right $CUSTOM_OUTPUT

echo
echo "测试2完成，输出目录：$CUSTOM_OUTPUT (Test 2 completed, output directory: $CUSTOM_OUTPUT)"
echo

# 显示输出结果
echo "=== 结果对比 (Results Comparison) ==="
echo
echo "默认输出目录结构 (Default output structure):"
if [ -d "calibration_data/output" ]; then
    tree calibration_data/output -L 2
else
    echo "未找到默认输出目录 (Default output directory not found)"
fi

echo
echo "自定义输出目录结构 (Custom output structure):"
if [ -d "$CUSTOM_OUTPUT" ]; then
    tree $CUSTOM_OUTPUT -L 2
else
    echo "未找到自定义输出目录 (Custom output directory not found)"
fi

echo
echo "=== 测试完成 (Testing Complete) ==="
echo "检查输出目录中的标定结果 (Check calibration results in output directories)"