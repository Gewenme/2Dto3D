#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
实验报告生成器
Experiment Report Generator

生成详细的Markdown格式实验报告
Generates detailed Markdown format experiment reports
"""

import json
import yaml
from datetime import datetime
from pathlib import Path
from typing import Dict, Any, List
import logging

class ReportGenerator:
    """实验报告生成器类 / Experiment report generator class"""
    
    def __init__(self, experiment_dir: Path, config: Dict[str, Any]):
        """初始化报告生成器 / Initialize report generator"""
        self.experiment_dir = Path(experiment_dir)
        self.config = config
        self.logger = logging.getLogger(__name__)
        
    def generate_comprehensive_report(self) -> str:
        """生成综合实验报告 / Generate comprehensive experiment report"""
        
        # 加载处理摘要 / Load processing summary
        summary_file = self.experiment_dir / 'processing_summary.json'
        if summary_file.exists():
            with open(summary_file, 'r', encoding='utf-8') as f:
                summary = json.load(f)
        else:
            summary = {'step_results': {}, 'experiment_info': {}}
        
        # 生成报告内容 / Generate report content
        report_content = self._generate_report_header(summary)
        report_content += self._generate_experiment_overview(summary)
        report_content += self._generate_processing_steps(summary)
        report_content += self._generate_quality_analysis(summary)
        report_content += self._generate_performance_metrics(summary)
        report_content += self._generate_visualizations()
        report_content += self._generate_conclusions(summary)
        
        # 保存报告 / Save report
        report_file = self.experiment_dir / 'comprehensive_experiment_report.md'
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write(report_content)
        
        self.logger.info(f"Comprehensive report generated: {report_file}")
        return str(report_file)
    
    def _generate_report_header(self, summary: Dict[str, Any]) -> str:
        """生成报告头部 / Generate report header"""
        start_time = summary.get('experiment_info', {}).get('start_time', 'Unknown')
        experiment_name = self.config.get('experiment', {}).get('name', '2D到3D立体视觉重建实验')
        
        return f"""# {experiment_name}
## Stereo Vision 3D Reconstruction Experiment Report

**实验日期 / Experiment Date:** {start_time}  
**报告生成时间 / Report Generated:** {datetime.now().isoformat()}  
**实验版本 / Version:** {self.config.get('experiment', {}).get('version', '1.0.0')}  
**作者 / Author:** {self.config.get('experiment', {}).get('author', 'Automation System')}

---

"""
    
    def _generate_experiment_overview(self, summary: Dict[str, Any]) -> str:
        """生成实验概述 / Generate experiment overview"""
        
        overview = """## 实验概述 / Experiment Overview

### 实验目标 / Experiment Objectives
本实验旨在通过自动化流程，完成从双目立体图像到三维点云的完整重建过程，并生成详细的实验文档记录每个处理步骤的参数、结果和质量评估。

This experiment aims to complete the full reconstruction process from binocular stereo images to 3D point clouds through an automated pipeline, generating detailed experimental documentation that records parameters, results, and quality assessments for each processing step.

### 实验背景 / Background
立体视觉是计算机视觉领域的重要技术，通过模拟人眼的双目视觉原理，利用两个不同视角的图像计算深度信息，实现三维场景重建。

Stereo vision is an important technology in computer vision that simulates the principle of human binocular vision, using images from two different viewpoints to calculate depth information and achieve 3D scene reconstruction.

### 技术路线 / Technical Approach
1. **图像预处理** - 标准化图像尺寸至3264×2448像素
2. **角点检测** - 检测标定图像中的棋盘格角点
3. **单目标定** - 分别标定左右相机的内参和畸变参数
4. **双目标定** - 计算两相机间的相对位置关系
5. **三维重建** - 基于立体匹配算法生成三维点云

---

"""
        return overview
    
    def _generate_processing_steps(self, summary: Dict[str, Any]) -> str:
        """生成处理步骤详情 / Generate processing steps details"""
        
        steps_content = """## 详细处理步骤 / Detailed Processing Steps

"""
        
        step_names = {
            'step1_image_resize': '图像预处理 / Image Preprocessing',
            'step2_corner_detection': '角点检测 / Corner Detection', 
            'step3_mono_calibration': '单目标定 / Mono Calibration',
            'step4_stereo_calibration': '双目标定 / Stereo Calibration',
            'step5_3d_reconstruction': '三维重建 / 3D Reconstruction'
        }
        
        step_results = summary.get('step_results', {})
        
        for step_id, step_title in step_names.items():
            if step_id in step_results:
                result = step_results[step_id]
                
                steps_content += f"""### {step_title}

**状态 / Status:** {'✅ 成功 / SUCCESS' if result.get('success', False) else '❌ 失败 / FAILED'}  
**处理时间 / Processing Time:** {result.get('processing_time', 0):.2f}秒 / seconds

"""
                
                # 添加输入参数 / Add input parameters
                if 'input_params' in result:
                    steps_content += "**输入参数 / Input Parameters:**\n"
                    for param, value in result['input_params'].items():
                        steps_content += f"- {param}: `{value}`\n"
                    steps_content += "\n"
                
                # 添加特定步骤的详细信息 / Add step-specific details
                if step_id == 'step2_corner_detection' and 'statistics' in result:
                    stats = result['statistics']
                    steps_content += "**检测统计 / Detection Statistics:**\n"
                    steps_content += f"- 左相机角点检测: {stats.get('left_camera', {}).get('corners_detected', 0)} / {stats.get('left_camera', {}).get('images_processed', 0)} (检测率: {stats.get('left_camera', {}).get('detection_rate', 0):.1%})\n"
                    steps_content += f"- 右相机角点检测: {stats.get('right_camera', {}).get('corners_detected', 0)} / {stats.get('right_camera', {}).get('images_processed', 0)} (检测率: {stats.get('right_camera', {}).get('detection_rate', 0):.1%})\n\n"
                
                if step_id == 'step3_mono_calibration' and 'error_analysis' in result:
                    error = result['error_analysis']
                    steps_content += "**标定误差分析 / Calibration Error Analysis:**\n"
                    steps_content += f"- 左相机重投影误差: {error.get('left_camera', {}).get('reprojection_error', 0):.3f} 像素\n"
                    steps_content += f"- 右相机重投影误差: {error.get('right_camera', {}).get('reprojection_error', 0):.3f} 像素\n"
                    steps_content += f"- 标定质量评级: {error.get('left_camera', {}).get('calibration_quality', 'Unknown')} / {error.get('right_camera', {}).get('calibration_quality', 'Unknown')}\n\n"
                
                if step_id == 'step4_stereo_calibration' and 'calibration_report' in result:
                    report = result['calibration_report']
                    steps_content += "**双目标定结果 / Stereo Calibration Results:**\n"
                    steps_content += f"- 立体重投影误差: {report.get('stereo_reprojection_error', 0):.3f} 像素\n"
                    steps_content += f"- 基线距离: {report.get('baseline_distance', 0):.3f} 米\n"
                    steps_content += f"- 会聚角度: {report.get('convergence_angle', 0):.1f} 度\n"
                    steps_content += f"- 校正质量: {report.get('rectification_quality', 'Unknown')}\n\n"
                
                if step_id == 'step5_3d_reconstruction' and 'reconstruction_metrics' in result:
                    metrics = result['reconstruction_metrics']
                    steps_content += "**三维重建结果 / 3D Reconstruction Results:**\n"
                    steps_content += f"- 点云大小: {metrics.get('point_cloud_size', 0):,} 个点\n"
                    steps_content += f"- 重建质量: {metrics.get('reconstruction_quality', 'Unknown')}\n"
                    steps_content += f"- 输出格式: {metrics.get('output_format', 'Unknown')}\n"
                    steps_content += f"- 质量等级: {metrics.get('quality_level', 0)}\n"
                    depth_range = metrics.get('depth_range', {})
                    steps_content += f"- 深度范围: {depth_range.get('min', 0):.1f} - {depth_range.get('max', 0):.1f} 米\n\n"
        
        steps_content += "---\n\n"
        return steps_content
    
    def _generate_quality_analysis(self, summary: Dict[str, Any]) -> str:
        """生成质量分析 / Generate quality analysis"""
        
        quality_content = """## 质量分析 / Quality Analysis

### 整体质量评估 / Overall Quality Assessment

"""
        
        step_results = summary.get('step_results', {})
        
        # 分析各步骤质量 / Analyze step quality
        quality_scores = []
        
        for step_id, result in step_results.items():
            if result.get('success', False):
                quality_scores.append(1.0)
                
                # 根据具体指标评估质量 / Evaluate quality based on specific metrics
                if step_id == 'step3_mono_calibration' and 'error_analysis' in result:
                    left_error = result['error_analysis'].get('left_camera', {}).get('reprojection_error', 999)
                    right_error = result['error_analysis'].get('right_camera', {}).get('reprojection_error', 999)
                    avg_error = (left_error + right_error) / 2
                    
                    if avg_error < 0.5:
                        quality_scores.append(1.0)
                    elif avg_error < 1.0:
                        quality_scores.append(0.8)
                    else:
                        quality_scores.append(0.6)
                        
                if step_id == 'step5_3d_reconstruction' and 'reconstruction_metrics' in result:
                    point_count = result['reconstruction_metrics'].get('point_cloud_size', 0)
                    if point_count > 10000:
                        quality_scores.append(1.0)
                    elif point_count > 1000:
                        quality_scores.append(0.7)
                    else:
                        quality_scores.append(0.4)
            else:
                quality_scores.append(0.0)
        
        overall_quality = sum(quality_scores) / len(quality_scores) if quality_scores else 0
        
        quality_content += f"**整体质量评分 / Overall Quality Score:** {overall_quality:.1%}\n\n"
        
        if overall_quality >= 0.9:
            quality_content += "🟢 **优秀 / Excellent** - 所有步骤都达到了高质量标准\n\n"
        elif overall_quality >= 0.7:
            quality_content += "🟡 **良好 / Good** - 大部分步骤质量良好，有少量改进空间\n\n"
        else:
            quality_content += "🔴 **需要改进 / Needs Improvement** - 多个步骤需要参数调优\n\n"
        
        # 详细质量指标 / Detailed quality metrics
        quality_content += """### 详细质量指标 / Detailed Quality Metrics

| 处理步骤 / Processing Step | 状态 / Status | 质量评级 / Quality Rating | 关键指标 / Key Metrics |
|---------------------------|---------------|-------------------------|----------------------|
"""
        
        for step_id, result in step_results.items():
            step_name = step_id.replace('_', ' ').title()
            status = "✅ 成功" if result.get('success', False) else "❌ 失败"
            
            # 根据步骤类型提取关键指标 / Extract key metrics by step type
            key_metrics = "N/A"
            quality_rating = "未评估"
            
            if step_id == 'step3_mono_calibration' and 'error_analysis' in result:
                left_error = result['error_analysis'].get('left_camera', {}).get('reprojection_error', 999)
                right_error = result['error_analysis'].get('right_camera', {}).get('reprojection_error', 999)
                key_metrics = f"误差: {left_error:.3f}, {right_error:.3f}px"
                quality_rating = result['error_analysis'].get('left_camera', {}).get('calibration_quality', 'Unknown')
                
            elif step_id == 'step5_3d_reconstruction' and 'reconstruction_metrics' in result:
                point_count = result['reconstruction_metrics'].get('point_cloud_size', 0)
                key_metrics = f"点云: {point_count:,} 个点"
                quality_rating = result['reconstruction_metrics'].get('reconstruction_quality', 'Unknown')
                
            quality_content += f"| {step_name} | {status} | {quality_rating} | {key_metrics} |\n"
        
        quality_content += "\n---\n\n"
        return quality_content
    
    def _generate_performance_metrics(self, summary: Dict[str, Any]) -> str:
        """生成性能指标 / Generate performance metrics"""
        
        perf_content = """## 性能评估 / Performance Assessment

### 处理时间分析 / Processing Time Analysis

"""
        
        step_results = summary.get('step_results', {})
        total_time = sum(result.get('processing_time', 0) for result in step_results.values())
        
        perf_content += f"**总处理时间 / Total Processing Time:** {total_time:.2f} 秒\n\n"
        
        perf_content += "| 处理步骤 / Step | 耗时 / Time (s) | 占比 / Percentage |\n"
        perf_content += "|-----------------|------------------|-------------------|\n"
        
        for step_id, result in step_results.items():
            step_name = step_id.replace('_', ' ').title()
            time_taken = result.get('processing_time', 0)
            percentage = (time_taken / total_time * 100) if total_time > 0 else 0
            perf_content += f"| {step_name} | {time_taken:.2f} | {percentage:.1f}% |\n"
        
        perf_content += "\n"
        
        # 性能建议 / Performance recommendations
        if total_time > 60:
            perf_content += "**性能建议 / Performance Recommendations:**\n"
            perf_content += "- 考虑使用更小的图像分辨率以提高处理速度\n"
            perf_content += "- 优化算法参数以减少计算复杂度\n\n"
        
        perf_content += "---\n\n"
        return perf_content
    
    def _generate_visualizations(self) -> str:
        """生成可视化部分 / Generate visualizations section"""
        
        viz_content = """## 可视化结果 / Visualization Results

### 处理流程图像对比 / Processing Pipeline Image Comparison

本节展示了每个处理步骤的输入和输出图像对比，帮助理解算法效果。

This section shows input and output image comparisons for each processing step to help understand algorithm effectiveness.

"""
        
        # 检查可视化文件是否存在 / Check if visualization files exist
        viz_dir = self.experiment_dir / 'visualizations'
        
        if viz_dir.exists():
            viz_files = list(viz_dir.glob('*.png')) + list(viz_dir.glob('*.jpg'))
            
            if viz_files:
                viz_content += "**生成的可视化文件 / Generated Visualization Files:**\n"
                for viz_file in sorted(viz_files):
                    viz_content += f"- ![{viz_file.stem}]({viz_file.relative_to(self.experiment_dir)})\n"
                viz_content += "\n"
            else:
                viz_content += "*注意：由于环境限制，可视化图像将在完整环境中生成*\n"
                viz_content += "*Note: Visualization images will be generated in full environment*\n\n"
        
        viz_content += "### 三维模型预览 / 3D Model Preview\n\n"
        
        # 检查3D模型文件 / Check 3D model files
        model_dir = self.experiment_dir / 'step5_3d_reconstruction' / 'point_clouds'
        if model_dir.exists():
            model_files = list(model_dir.glob('*.ply')) + list(model_dir.glob('*.obj'))
            if model_files:
                viz_content += "**生成的3D模型文件 / Generated 3D Model Files:**\n"
                for model_file in model_files:
                    viz_content += f"- [{model_file.name}]({model_file.relative_to(self.experiment_dir)}) - 三维点云模型\n"
                viz_content += "\n"
        
        viz_content += "---\n\n"
        return viz_content
    
    def _generate_conclusions(self, summary: Dict[str, Any]) -> str:
        """生成结论和建议 / Generate conclusions and recommendations"""
        
        conclusions_content = """## 结论和建议 / Conclusions and Recommendations

### 实验结果总结 / Experiment Results Summary

"""
        
        step_results = summary.get('step_results', {})
        successful_steps = sum(1 for result in step_results.values() if result.get('success', False))
        total_steps = len(step_results)
        success_rate = (successful_steps / total_steps * 100) if total_steps > 0 else 0
        
        conclusions_content += f"**成功率 / Success Rate:** {successful_steps}/{total_steps} ({success_rate:.1f}%)\n\n"
        
        if success_rate == 100:
            conclusions_content += """**实验评估 / Experiment Assessment:**
✅ 所有处理步骤均成功完成
✅ 立体视觉流程运行正常
✅ 三维重建质量达到预期标准

All processing steps completed successfully. The stereo vision pipeline operated normally and 3D reconstruction quality met expected standards.

"""
        elif success_rate >= 80:
            conclusions_content += """**实验评估 / Experiment Assessment:**
🟡 大部分步骤成功完成，有个别步骤需要调优
🟡 整体流程基本正常，建议检查失败步骤的参数设置

Most steps completed successfully with a few requiring parameter adjustment. Overall pipeline is functioning normally.

"""
        else:
            conclusions_content += """**实验评估 / Experiment Assessment:**
🔴 多个步骤执行失败，需要全面检查参数配置和输入数据
🔴 建议详细分析失败原因并调整实验设置

Multiple steps failed execution. Comprehensive review of parameter configuration and input data is recommended.

"""
        
        conclusions_content += """### 改进建议 / Improvement Recommendations

#### 参数优化 / Parameter Optimization
- 根据标定误差调整棋盘格尺寸参数
- 优化图像分辨率以平衡质量和处理速度
- 调整立体匹配算法参数以提高重建精度

#### 数据质量 / Data Quality  
- 确保标定图像包含清晰的棋盘格图案
- 增加标定图像的数量和角度多样性
- 检查左右图像的同步性和对应关系

#### 系统优化 / System Optimization
- 考虑使用GPU加速计算密集型步骤
- 优化内存使用以处理更大分辨率图像
- 实现并行处理以提高整体效率

### 技术路线图 / Technical Roadmap

#### 短期改进 / Short-term Improvements
- [ ] 添加更多质量评估指标
- [ ] 实现自动参数调优功能
- [ ] 增强错误诊断和恢复机制

#### 长期规划 / Long-term Planning
- [ ] 集成深度学习立体匹配算法
- [ ] 添加实时处理能力
- [ ] 开发Web界面用于交互式操作

---

## 实验数据存档 / Experiment Data Archive

本次实验的所有数据、参数、结果和分析报告已保存在以下目录结构中，确保实验的可重现性和可追溯性。

All experiment data, parameters, results, and analysis reports have been saved in the following directory structure to ensure experiment reproducibility and traceability.

**实验文件夹 / Experiment Folder:** `{experiment_dir}`

**报告生成时间 / Report Generated:** {report_time}

---
*本报告由2D到3D转换流程自动化实验系统生成*  
*This report was generated by the Automated 2D to 3D Conversion Experiment System*
""".format(
            experiment_dir=self.experiment_dir.name,
            report_time=datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        )
        
        return conclusions_content
    
    def generate_step_by_step_report(self, step_name: str, step_data: Dict[str, Any]) -> str:
        """为单个步骤生成详细报告 / Generate detailed report for individual step"""
        
        report = f"""# {step_name} 详细报告 / Detailed Report

**处理时间 / Processing Time:** {step_data.get('processing_time', 0):.2f}s  
**状态 / Status:** {'成功 / SUCCESS' if step_data.get('success', False) else '失败 / FAILED'}

## 输入参数 / Input Parameters

"""
        
        if 'input_params' in step_data:
            for param, value in step_data['input_params'].items():
                report += f"- **{param}:** `{value}`\n"
        
        report += "\n## 处理结果 / Processing Results\n\n"
        
        # 添加步骤特定的结果 / Add step-specific results
        if 'statistics' in step_data:
            report += "### 统计信息 / Statistics\n\n"
            stats = step_data['statistics']
            report += f"```json\n{json.dumps(stats, indent=2, ensure_ascii=False)}\n```\n\n"
        
        if 'error_analysis' in step_data:
            report += "### 误差分析 / Error Analysis\n\n"
            analysis = step_data['error_analysis']
            report += f"```json\n{json.dumps(analysis, indent=2, ensure_ascii=False)}\n```\n\n"
        
        return report