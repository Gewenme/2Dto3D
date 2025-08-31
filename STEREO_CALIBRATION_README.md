# 双目标定程序 (Stereo Calibration Program)

类似MATLAB双目标定工具箱的完整标定流程程序

Similar to MATLAB Stereo Camera Calibrator Toolbox - Complete Calibration Workflow Program

## 功能特点 (Features)

- ✅ **输入文件地址导入** - 支持自定义左右图集路径
- ✅ **自动角点检测** - 检测棋盘格内角点并可视化
- ✅ **完整参数求取** - 单目标定 + 双目标定
- ✅ **图像矫正** - 基于标定参数自动矫正畸变
- ✅ **3D建模支持** - 支持立体匹配3D重建
- ✅ **类MATLAB界面** - 简洁的中英双语输出界面

## 使用方法 (Usage)

### 基本用法 (Basic Usage)

```bash
# 使用默认路径
./stereo_calibration_program

# 使用自定义路径
./stereo_calibration_program <左图像路径> <右图像路径> <输出路径>
```

### 示例 (Examples)

```bash
# 使用默认目录结构
./stereo_calibration_program

# 自定义输入和输出路径
./stereo_calibration_program ./images/left ./images/right ./results
```

## 输入要求 (Input Requirements)

### 目录结构 (Directory Structure)

```
输入目录/
├── 左图像目录/          # 左相机标定图像
│   ├── image1.jpg
│   ├── image2.jpg
│   └── ...
└── 右图像目录/          # 右相机标定图像
    ├── image1.jpg
    ├── image2.jpg
    └── ...
```

### 图像要求 (Image Requirements)

- **格式**: JPG, JPEG, PNG, BMP
- **数量**: 建议至少10张图像对
- **内容**: 包含9×6内角点棋盘格 (8.2mm方格)
- **质量**: 清晰无模糊，角点完整可见

## 输出结果 (Output Results)

### 目录结构 (Output Structure)

```
输出目录/
├── preprocessed/         # 预处理后的图像 (3264×2448)
│   ├── left/              # 左相机图像
│   └── right/             # 右相机图像
├── corners/              # 角点检测结果
│   ├── left/              # 左相机角点
│   │   ├── corner_data/   # 角点坐标数据
│   │   └── corner_images/ # 角点可视化图像
│   └── right/             # 右相机角点
├── calibration/          # 标定参数
│   ├── left/              # 左相机标定结果
│   │   ├── camera_calibration.yml  # 相机内参
│   │   ├── residual_images/        # 残差图像
│   │   └── undistorted/            # 畸变矫正图像
│   ├── right/             # 右相机标定结果
│   └── stereo/            # 双目标定结果
│       ├── stereo_calibration.yml  # 立体标定参数
│       └── stereo_rectify.yml      # 立体校正参数
├── rectified/            # 立体校正图像
│   ├── left/              # 左相机校正图像
│   └── right/             # 右相机校正图像
└── 3d_model/             # 3D重建结果 (如有场景图像)
```

### 输出文件说明 (Output Files)

1. **camera_calibration.yml** - 相机内参矩阵、畸变系数
2. **stereo_calibration.yml** - 立体标定参数 (R, T, E, F矩阵)
3. **stereo_rectify.yml** - 立体校正参数
4. **residual_images/** - 标定残差可视化图像
5. **undistorted/** - 畸变矫正后的图像
6. **rectified/** - 立体校正后的图像

## 标定流程 (Calibration Workflow)

### 第1步：图像预处理 (Image Preprocessing)
- 统一图像尺寸为3264×2448像素
- 保持纵横比，使用双线性插值

### 第2步：角点检测 (Corner Detection)
- 检测9×6内角点棋盘格
- 生成角点可视化图像
- 保存角点坐标数据

### 第3步：单目标定 (Monocular Calibration)
- 计算左右相机内参矩阵
- 估计畸变系数
- 生成残差图像和矫正图像

### 第4步：双目标定 (Stereo Calibration)  
- 计算相机间几何关系 (R, T)
- 计算本质矩阵 (E) 和基础矩阵 (F)
- 估计立体校正参数

### 第5步：立体校正 (Stereo Rectification)
- 基于双目标定参数校正图像
- 确保左右图像行对齐

### 第6步：3D建模 (3D Modeling) [可选]
- 如提供场景图像，进行立体匹配
- 生成3D点云模型

## 技术参数 (Technical Parameters)

- **棋盘格规格**: 9×6 内角点
- **方格尺寸**: 8.2mm
- **目标图像尺寸**: 3264×2448 像素
- **支持格式**: JPG, JPEG, PNG, BMP
- **最小图像数**: 建议 ≥ 10张图像对

## 与MATLAB对比 (Comparison with MATLAB)

| 功能 | MATLAB Stereo Calibrator | 本程序 |
|------|--------------------------|--------|
| 图像导入 | ✅ GUI界面 | ✅ 命令行路径 |
| 角点检测 | ✅ 自动+手动 | ✅ 自动检测 |
| 单目标定 | ✅ 内参+畸变 | ✅ 内参+畸变 |
| 双目标定 | ✅ 立体参数 | ✅ 立体参数 |
| 图像矫正 | ✅ 畸变+立体 | ✅ 畸变+立体 |
| 参数导出 | ✅ MAT格式 | ✅ YAML格式 |
| 3D重建 | ✅ 点云 | ✅ 点云 |
| 跨平台性 | ❌ Windows主要 | ✅ 跨平台 |

## 编译和安装 (Build and Installation)

### 依赖项 (Dependencies)
- OpenCV 4.x
- CMake 3.10+
- C++17 编译器

### 编译步骤 (Build Steps)
```bash
mkdir build
cd build
cmake ..
make stereo_calibration_program
```

### 运行 (Run)
```bash
./build/bin/stereo_calibration_program
```

## 故障排除 (Troubleshooting)

### 常见问题 (Common Issues)

1. **角点检测失败**
   - 检查图像质量和棋盘格清晰度
   - 确保9×6内角点规格正确

2. **标定精度不高**  
   - 增加标定图像数量 (≥15张)
   - 改善图像拍摄角度的多样性

3. **文件路径错误**
   - 检查输入路径是否存在
   - 确保有足够的磁盘空间

### 性能优化 (Performance)

- 使用SSD存储提高I/O性能
- 增加内存以处理大量图像
- 使用多线程并行处理 (未来版本)

## 许可证 (License)

本程序基于原项目许可证发布。

---

**开发信息 (Development Info)**
- 版本: v1.0
- 兼容: OpenCV 4.x
- 语言: C++17
- 平台: Linux, Windows, macOS