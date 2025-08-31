# 相机标定和3D重建流程 (8.2mm棋盘格)
# Camera Calibration and 3D Reconstruction Pipeline (8.2mm Chessboard)

## 概述 (Overview)

这是一个使用8.2mm棋盘格边长的相机标定和3D重建完整流程实现。按照要求的5个步骤顺序执行：

This is a complete camera calibration and 3D reconstruction pipeline implementation using 8.2mm chessboard squares. Executes the required 5 steps in order:

## 流程步骤 (Pipeline Steps)

### 第1步：图像预处理 (Step 1: Image Preprocessing)
- **目标**: 将左右标定图组图片裁剪为3264×2448像素
- **功能**: 统一图像尺寸，为后续处理做准备
- **执行**: `./bin/step1_image_preprocessing` 或使用完整流程

### 第2步：角点检测 (Step 2: Corner Detection)  
- **目标**: 检测棋盘格角点并可视化
- **功能**: 检测9×6内角点，生成角点标记图像和坐标数据
- **执行**: `./bin/step2_corner_detection` 或使用完整流程

### 第3步：单目标定 (Step 3: Monocular Calibration)
- **目标**: 对左右相机分别进行单目标定  
- **功能**: 计算相机内参、畸变系数，生成矫正图和残差图
- **执行**: `./bin/step3_monocular_calibration` 或使用完整流程

### 第4步：立体校正 (Step 4: Stereo Rectification)
- **目标**: 双目标定和立体校正
- **功能**: 计算双目几何关系，生成立体校正参数
- **执行**: `./bin/step4_stereo_rectification` 或使用完整流程

### 第5步：三维重建 (Step 5: 3D Reconstruction)
- **目标**: 基于立体匹配的3D点云重建
- **功能**: 立体匹配，生成深度图和3D点云模型
- **执行**: `./bin/step5_3d_reconstruction` 或使用完整流程

## 关键配置 (Key Configuration)

| 参数 (Parameter) | 值 (Value) | 说明 (Description) |
|-----------------|------------|-------------------|
| 棋盘格边长 (Square Size) | **8.2mm (0.0082f)** | 关键要求参数 |
| 棋盘格尺寸 (Board Size) | 9×6 内角点 | 标准棋盘格 |
| 图像尺寸 (Image Size) | 3264×2448 像素 | 第1步要求的分辨率 |

## 使用方法 (Usage)

### 方法1：完整流程执行 (Method 1: Complete Pipeline Execution)
```bash
# 构建项目
mkdir build && cd build
cmake ..
make

# 准备输入数据
mkdir -p calibration_data/input/left
mkdir -p calibration_data/input/right
# 将棋盘格标定图像放入 left/ 和 right/ 文件夹
# 将场景图像命名为 scene_left.jpg 和 scene_right.jpg

# 运行完整流程
./bin/2Dto3D
# 或者运行演示版本
./bin/demo_8_2mm
```

### 方法2：分步执行 (Method 2: Step-by-Step Execution)
```bash
# 第1步：图像预处理
./bin/step1_image_preprocessing

# 第2步：角点检测
./bin/step2_corner_detection

# 第3步：单目标定
./bin/step3_monocular_calibration

# 第4步：双目标定和立体校正
./bin/step4_stereo_rectification

# 第5步：3D重建
./bin/step5_3d_reconstruction
```

## 输入数据结构 (Input Data Structure)

```
calibration_data/input/
├── left/           # 左相机棋盘格标定图像 (8.2mm方格)
├── right/          # 右相机棋盘格标定图像 (8.2mm方格)
├── scene_left.jpg  # 左场景图像 (用于3D重建)
└── scene_right.jpg # 右场景图像 (用于3D重建)
```

## 输出结果结构 (Output Results Structure)

```
calibration_data/output/
├── step1_left_resized/      # 第1步：左图像预处理结果 (3264×2448)
├── step1_right_resized/     # 第1步：右图像预处理结果 (3264×2448)
├── step2_left_corners/      # 第2步：左图像角点检测可视化
├── step2_right_corners/     # 第2步：右图像角点检测可视化
├── step3_left_calibration/  # 第3步：左相机单目标定参数和残差图
├── step3_right_calibration/ # 第3步：右相机单目标定参数和残差图
├── step3_left_corrected/    # 第3步：左相机矫正图
├── step3_right_corrected/   # 第3步：右相机矫正图
├── step4_stereo_calibration/# 第4步：双目标定和立体校正参数
└── step5_3d_reconstruction/ # 第5步：3D点云重建结果
```

## 要求特性 (Required Features)

✅ **简单直接的OpenCV代码实现** (Simple and direct OpenCV code implementation)
✅ **按流程顺序执行** (Execute in process order)  
✅ **棋盘格边长配置为8.2mm** (Chessboard edge length configured to 8.2mm)
✅ **每个步骤单独的函数或脚本** (Each step as separate function or script)
✅ **无复杂自动生成器** (No complex automatic generators)

## 输出文件类型 (Output File Types)

1. **矫正图 (Corrected Images)**: 去除镜头畸变的图像文件
2. **残差图 (Residual Images)**: 标定精度可视化图像
3. **3D模型 (3D Models)**: PLY格式点云文件，可用CloudCompare等工具查看

## 依赖要求 (Dependencies)

- OpenCV 4.0+
- CMake 3.10+
- C++17 编译器
- 8.2mm方格的棋盘格标定图像
- 立体图像对（用于3D重建）