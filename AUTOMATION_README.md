# 2D到3D转换流程自动化实验文档生成器
# Automated 2D to 3D Conversion Experiment Documentation Generator

## 概述 / Overview

这是一个智能化的自动化实验系统，能够完整执行2D到3D立体视觉重建流程，并生成详细的实验文档。系统通过Python脚本协调现有的C++处理模块，自动化执行从图像预处理到三维重建的完整流程，同时实时监控处理进度、记录参数配置、分析结果质量，并生成专业的实验报告。

This is an intelligent automated experiment system that can completely execute the 2D to 3D stereo vision reconstruction pipeline and generate detailed experimental documentation. The system coordinates existing C++ processing modules through Python scripts, automatically executing the complete process from image preprocessing to 3D reconstruction, while real-time monitoring processing progress, recording parameter configurations, analyzing result quality, and generating professional experiment reports.

## 核心功能 / Core Features

### 🚀 自动化处理流程 / Automated Processing Pipeline
- **Step 1**: 图像预处理 - 将图像调整为3264×2448像素标准尺寸
- **Step 2**: 角点检测 - 自动检测9×6棋盘格角点并生成可视化结果
- **Step 3**: 单目标定 - 分别标定左右相机的内参和畸变参数
- **Step 4**: 双目标定 - 计算立体相机系统的相对位置关系
- **Step 5**: 三维重建 - 基于立体匹配生成高质量三维点云

### 📊 智能分析与监控 / Intelligent Analysis & Monitoring
- **实时进度监控** - 监控每个步骤的执行状态和处理时间
- **质量评估分析** - 自动评估标定精度和重建质量
- **性能基准测试** - 记录系统性能指标和资源使用情况
- **错误诊断** - 提供详细的错误信息和解决建议

### 📋 综合文档生成 / Comprehensive Documentation Generation
- **多语言报告** - 支持中英文双语实验报告
- **结构化输出** - 按步骤组织的完整目录结构
- **可视化图表** - 自动生成性能图表和质量分析图
- **可追溯记录** - 完整记录所有参数、中间结果和最终输出

## 安装和使用 / Installation and Usage

### 系统要求 / System Requirements

```bash
# Python依赖 / Python Dependencies
pip3 install pyyaml psutil numpy

# 可选：图表生成 / Optional: Chart Generation
pip3 install matplotlib

# 可选：PDF生成 / Optional: PDF Generation  
apt install pandoc texlive-xetex
```

### 快速开始 / Quick Start

1. **创建输入目录结构 / Create Input Directory Structure**
```bash
python3 comprehensive_experiment_report.py --setup-only
```

2. **准备输入图像 / Prepare Input Images**
```
input/
├── left/          # 左相机标定图像 / Left camera calibration images
├── right/         # 右相机标定图像 / Right camera calibration images
└── scene/         # 场景图像 / Scene images
    ├── left/      # 左相机场景图像 / Left camera scene images
    └── right/     # 右相机场景图像 / Right camera scene images
```

3. **运行完整实验 / Run Complete Experiment**
```bash
python3 comprehensive_experiment_report.py
```

4. **查看结果 / View Results**
```
experiment_results_YYYY-MM-DD_HH-MM-SS/
├── comprehensive_experiment_report.md    # 综合实验报告
├── processing_summary.json              # 处理摘要
├── experiment_status.txt                # 状态报告
└── ...                                  # 详细结果文件
```

### 高级用法 / Advanced Usage

#### 自定义配置 / Custom Configuration
```bash
# 使用自定义配置文件 / Use custom configuration file
python3 comprehensive_experiment_report.py --config my_config.yaml

# 指定输出目录 / Specify output directory
python3 comprehensive_experiment_report.py --output my_results

# 详细输出模式 / Verbose output mode
python3 comprehensive_experiment_report.py --verbose
```

#### 批处理模式 / Batch Processing Mode
```python
# 批量处理多组图像 / Batch process multiple image sets
from experiment_controller import ExperimentController

for dataset in datasets:
    controller = ExperimentController(f"configs/{dataset}.yaml")
    controller.run_experiment()
```

## 配置文件说明 / Configuration File Description

### config.yaml 主要配置项 / Main Configuration Items

```yaml
# 实验基本设置 / Basic experiment settings
experiment:
  name: "Your Experiment Name"
  version: "1.0.0"
  author: "Your Name"

# 路径配置 / Path configuration
paths:
  input:
    left_images: "input/left"
    right_images: "input/right"
  output:
    base_dir: "experiment_results"
    use_timestamp: true

# 处理参数 / Processing parameters
processing:
  image_resize:
    target_width: 3264      # 目标图像宽度
    target_height: 2448     # 目标图像高度
    interpolation: "LINEAR" # 插值方法
    
  corner_detection:
    board_width: 9          # 棋盘格内角点宽度
    board_height: 6         # 棋盘格内角点高度
    scale_factor: 1.0       # 缩放因子
    
  calibration:
    square_size: 0.0082     # 棋盘格实际尺寸(米)
    save_undistorted: true  # 保存校正后图像
    
  reconstruction:
    output_format: 0        # 0=PLY, 1=OBJ
    quality_level: 3        # 1=低, 2=中, 3=高

# 质量阈值 / Quality thresholds
quality_thresholds:
  max_reprojection_error: 1.0    # 最大重投影误差(像素)
  min_point_cloud_size: 1000     # 最小点云大小
  max_processing_time: 300       # 最大处理时间(秒)
```

## 输出文件结构 / Output File Structure

```
experiment_results_YYYY-MM-DD_HH-MM-SS/
├── config/
│   └── experiment_config.yaml           # 实验配置备份
├── step1_image_resize/
│   ├── input_images/                    # 原始输入图像
│   ├── output_images/                   # 调整后图像
│   └── processing_log.txt               # 处理日志
├── step2_corner_detection/
│   ├── corner_images/                   # 角点检测结果图像
│   ├── detection_results/               # 角点数据文件
│   └── statistics.json                  # 检测统计
├── step3_mono_calibration/
│   ├── calibration_params/              # 标定参数文件
│   ├── corrected_images/                # 校正后图像
│   └── error_analysis.json              # 误差分析
├── step4_stereo_calibration/
│   ├── stereo_params/                   # 双目标定参数
│   ├── rectified_images/                # 立体校正图像
│   └── calibration_report.json         # 标定报告
├── step5_3d_reconstruction/
│   ├── point_clouds/                    # 三维点云文件
│   ├── depth_maps/                      # 深度图
│   └── reconstruction_metrics.json     # 重建指标
├── analysis/
│   ├── calibration_accuracy.json       # 标定精度分析
│   ├── reconstruction_quality.json     # 重建质量分析
│   ├── processing_performance.json     # 处理性能分析
│   └── quality_assessment.json         # 整体质量评估
├── visualizations/
│   ├── text_dashboard.txt              # 文本仪表板
│   ├── timing_performance.png          # 时间性能图表*
│   ├── quality_assessment.png          # 质量评估图表*
│   └── comprehensive_dashboard.png     # 综合仪表板*
├── logs/
│   ├── experiment.log                  # 实验日志
│   └── comprehensive_experiment.log    # 综合实验日志
├── comprehensive_experiment_report.md  # 📋 综合实验报告
├── experiment_report.pdf               # 📄 PDF报告*
├── processing_summary.json             # 处理摘要
├── final_experiment_summary.json       # 最终摘要
└── experiment_status.txt               # 状态报告

* 需要相应的依赖包 / Requires corresponding dependencies
```

## 核心脚本说明 / Core Scripts Description

### 1. comprehensive_experiment_report.py
**主入口脚本 / Main entry script**
- 协调整个实验流程的执行
- 集成所有功能模块
- 提供命令行接口

### 2. experiment_controller.py  
**实验控制器 / Experiment controller**
- 管理5个处理步骤的执行顺序
- 监控处理进度和状态
- 记录处理时间和结果

### 3. report_generator.py
**报告生成器 / Report generator**
- 生成详细的Markdown格式报告
- 支持中英文双语输出
- 包含完整的分析和建议

### 4. result_analyzer.py
**结果分析器 / Result analyzer**
- 分析标定精度和重建质量
- 生成性能基准报告
- 提供质量评估和优化建议

### 5. visualization_generator.py
**可视化生成器 / Visualization generator**
- 生成性能和质量分析图表
- 创建文本版仪表板
- 支持多种可视化格式

### 6. cpp_integration_adapter.py
**C++集成适配器 / C++ integration adapter**
- 提供Python到C++的接口
- 自动构建C++项目
- 处理参数转换和错误处理

## 使用场景 / Usage Scenarios

### 🔬 研究实验 / Research Experiments
- 对比不同参数配置的效果
- 评估算法性能和精度
- 生成实验报告用于论文发表

### 🏭 工业应用 / Industrial Applications  
- 质量控制和批量处理
- 自动化的3D检测流程
- 标准化的处理文档记录

### 🎓 教学演示 / Educational Demonstrations
- 展示立体视觉原理和流程
- 生成详细的教学材料
- 提供参数调试的指导

## 扩展功能 / Extension Features

### 📊 高级分析 / Advanced Analysis
- 多实验结果对比分析
- 参数优化建议
- 趋势分析和预测

### 🔧 自定义处理 / Custom Processing
- 可插拔的处理模块
- 自定义质量评估指标
- 灵活的输出格式支持

### 🌐 Web界面 / Web Interface
- 浏览器中查看实验结果
- 交互式参数调整
- 实时进度监控

## 故障排除 / Troubleshooting

### 常见问题 / Common Issues

1. **C++构建失败 / C++ Build Failure**
   ```bash
   # 安装OpenCV开发包 / Install OpenCV development packages
   sudo apt install libopencv-dev
   
   # 重新构建项目 / Rebuild project
   rm -rf build && mkdir build && cd build
   cmake .. && make
   ```

2. **Python依赖缺失 / Missing Python Dependencies**
   ```bash
   pip3 install pyyaml psutil numpy matplotlib
   ```

3. **输入图像不足 / Insufficient Input Images**
   - 确保每个目录至少有5张标定图像
   - 检查图像格式是否支持(JPG, PNG, BMP)
   - 验证图像包含清晰的棋盘格图案

4. **标定精度不佳 / Poor Calibration Accuracy**
   - 增加标定图像的数量和角度多样性
   - 检查棋盘格尺寸参数是否正确
   - 确保图像质量清晰无模糊

### 日志文件 / Log Files
检查以下日志文件获取详细错误信息：
- `logs/comprehensive_experiment.log` - 主实验日志
- `logs/experiment.log` - 处理步骤日志
- `step*/processing_log.txt` - 各步骤详细日志

## 技术架构 / Technical Architecture

```
┌─────────────────────┐    ┌─────────────────────┐    ┌─────────────────────┐
│ Python控制层         │    │ C++处理层           │    │ 输出文档层           │
│ Python Control Layer│    │ C++ Processing Layer│    │ Output Doc Layer    │
├─────────────────────┤    ├─────────────────────┤    ├─────────────────────┤
│ • 实验控制器         │────│ • 图像处理模块       │────│ • Markdown报告      │
│ • 配置管理          │    │ • 标定算法模块       │    │ • JSON数据文件      │
│ • 进度监控          │    │ • 三维重建模块       │    │ • 可视化图表        │
│ • 结果分析          │    │ • 模型查看模块       │    │ • PDF文档*          │
└─────────────────────┘    └─────────────────────┘    └─────────────────────┘
```

## 开发和贡献 / Development and Contribution

### 代码结构 / Code Structure
- 模块化设计，每个功能独立封装
- 面向对象的架构，便于扩展和维护
- 完整的错误处理和日志记录
- 支持多种配置和参数调整

### 扩展指南 / Extension Guide
1. **添加新的处理步骤**：在`experiment_controller.py`中添加新的`_stepX_*`方法
2. **自定义分析指标**：在`result_analyzer.py`中扩展分析功能
3. **增强可视化**：在`visualization_generator.py`中添加新的图表类型
4. **集成新的C++模块**：在`cpp_integration_adapter.py`中添加接口适配

## 许可证 / License

本项目遵循与原始2Dto3D项目相同的许可证条款。

This project follows the same license terms as the original 2Dto3D project.

## 联系和支持 / Contact and Support

如有问题或建议，请通过GitHub Issues提交。

For questions or suggestions, please submit through GitHub Issues.

---

**最后更新 / Last Updated:** {datetime.now().strftime('%Y-%m-%d')}  
**版本 / Version:** 1.0.0