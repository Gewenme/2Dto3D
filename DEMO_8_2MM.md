# 8.2mm Camera Calibration Demo / 8.2毫米相机标定演示

## Overview / 概述

This demo implements camera calibration and 3D reconstruction using **8.2mm chessboard squares** as specified in the requirements. It generates all three required outputs:

本演示使用**8.2毫米棋盘格方块**实现相机标定和3D重建，生成所有三种要求的输出：

- **矫正图 (Corrected Images)**: Undistorted images with lens distortion removed
- **残差图 (Residual Images)**: Visualization of calibration accuracy and reprojection errors  
- **模型 (3D Models)**: Point cloud reconstruction from stereo image pairs

## Quick Start / 快速开始

### 1. Build the Demo / 构建演示
```bash
mkdir build && cd build
cmake ..
make
```

### 2. Run the Demo / 运行演示
```bash
# Run from source directory for correct paths
cd /path/to/2Dto3D
./build/bin/demo_8_2mm
```

### 3. Input Structure / 输入结构
```
demo_8_2mm/
├── input/
│   ├── left/           # Left camera chessboard images (8.2mm squares)
│   ├── right/          # Right camera chessboard images (8.2mm squares)  
│   ├── scene_left.jpg  # Left scene image for 3D reconstruction
│   └── scene_right.jpg # Right scene image for 3D reconstruction
```

## Configuration / 配置参数

| Parameter / 参数 | Value / 值 | Description / 描述 |
|------------------|------------|-------------------|
| Square Size / 方格边长 | **8.2mm (0.0082f)** | Key requirement / 关键参数 |
| Board Size / 棋盘尺寸 | 9x6 inner corners / 内角点 | Standard chessboard / 标准棋盘格 |
| Image Size / 图像尺寸 | 640x480 pixels / 像素 | Processing resolution / 处理分辨率 |

## Generated Outputs / 生成的输出

### 1. Corrected Images / 矫正图
- **Location / 位置**: `demo_8_2mm/output/*_corrected/`
- **Purpose / 用途**: Images with lens distortion removed using calibration parameters
- **Format / 格式**: Same as input (JPG, PNG, etc.)

### 2. Residual Images / 残差图  
- **Location / 位置**: `demo_8_2mm/output/*_calibration/residual_images/`
- **Purpose / 用途**: Visualization of calibration accuracy and reprojection errors
- **Features / 特征**:
  - Green circles: Detected chessboard corners / 检测到的棋盘格角点
  - Red circles: Projected corners using calibration / 标定参数投影的角点
  - Magenta lines: Error vectors / 误差向量
  - Color-coded error magnitude / 颜色编码的误差大小
  - Statistics overlay / 统计信息叠加

### 3. 3D Models / 3D模型
- **Location / 位置**: `demo_8_2mm/output/3d_model/`
- **Purpose / 用途**: Point cloud reconstruction from stereo image pairs
- **Formats / 格式**:
  - `reconstruction.ply`: Main point cloud file
  - `model_statistics.txt`: Model analysis and statistics
  - `model_views/`: Projection views from different angles

## Pipeline Stages / 流程阶段

### 1. Image Resizing / 图像调整
```cpp
ImageUtils::resizeImage(inputFolder, outputFolder, 640, 480, ImageUtils::LINEAR);
```

### 2. Corner Detection / 角点检测
```cpp  
detectAndDrawCorners(imageFolder, outputFolder, 9, 6, 1.0f);
```

### 3. Camera Calibration / 相机标定
```cpp
MonoCalibration::calibrateCamera(
    cornerDataPath, imageFolderPath, outputPath,
    9, 6, 0.0082f,  // 8.2mm squares
    640, 480,
    true,  // Generate corrected images
    undistortedPath
);
```

### 4. Stereo Calibration / 双目标定
```cpp
StereoCalibration::calibrateStereoCamera(
    leftCornerData, rightCornerData, outputPath,
    9, 6, 0.0082f,  // 8.2mm squares  
    640, 480
);
```

### 5. 3D Reconstruction / 3D重建
```cpp
StereoReconstruction::reconstruct3D(
    leftSceneImage, rightSceneImage,
    calibrationPath, outputPath,
    PLY_FORMAT, MEDIUM_QUALITY,
    0.0082f,  // 8.2mm squares
    640, 480
);
```

## Key Features / 主要特征

### Automatic Residual Image Generation / 自动残差图生成
- Automatically called during `calibrateCamera()`
- Shows reprojection accuracy visually  
- Color-codes error magnitudes
- Displays per-image statistics

### Bilingual Interface / 双语界面
- Chinese and English output messages
- Clear progress indicators (✓ ✗ ⚠ ℹ)
- Comprehensive error handling

### Comprehensive Output / 全面输出
- All three required output types generated automatically
- Proper directory structure creation
- Statistics and analysis included

## Technical Details / 技术细节

### Residual Image Algorithm / 残差图算法
1. Load calibration results (camera matrix, distortion coefficients, rvecs, tvecs)
2. Project 3D object points to 2D using calibration parameters
3. Calculate reprojection errors between detected and projected points  
4. Visualize errors with color coding and vector overlays
5. Generate statistics (average error, max error, per-image metrics)

### Color Coding / 颜色编码
- **Green**: Detected chessboard corners / 检测到的角点
- **Red**: Projected corners from calibration / 标定投影角点  
- **Magenta**: Error vectors / 误差向量
- **Heat map**: Error magnitude (blue=low, red=high) / 误差大小热图

## Example Usage / 使用示例

```bash
# 1. Add chessboard calibration images (8.2mm squares)
cp your_chessboard_images/* demo_8_2mm/input/left/
cp your_chessboard_images/* demo_8_2mm/input/right/

# 2. Add scene images for 3D reconstruction  
cp scene_left.jpg demo_8_2mm/input/
cp scene_right.jpg demo_8_2mm/input/

# 3. Run the complete pipeline
./build/bin/demo_8_2mm

# 4. Check outputs
ls demo_8_2mm/output/left_corrected/        # Corrected images
ls demo_8_2mm/output/left_calibration/residual_images/  # Residual images  
ls demo_8_2mm/output/3d_model/              # 3D models
```

## Requirements / 要求

- OpenCV 4.0+ 
- CMake 3.10+
- C++17 compiler
- Chessboard calibration images with 8.2mm squares
- Stereo image pairs for 3D reconstruction

## Troubleshooting / 故障排除

### No corners found / 未找到角点
- Ensure chessboard patterns are clearly visible
- Check image quality and lighting
- Verify board dimensions (9x6 inner corners)
- Confirm square size is actually 8.2mm

### Calibration fails / 标定失败  
- Need at least 10+ good chessboard images
- Images should cover different positions and angles
- Avoid blurry or poorly lit images

### 3D reconstruction fails / 3D重建失败
- Verify stereo calibration completed successfully
- Check that scene images are stereo pairs
- Ensure scene images have sufficient texture/features

## Files Modified / 修改的文件

- `mono_calibration.h/cpp`: Added residual image generation
- `example.cpp`: Updated to use 8.2mm parameter  
- `demo_8_2mm.cpp`: New comprehensive demo
- `CMakeLists.txt`: Added new demo target
- `.gitignore`: Excluded output directories