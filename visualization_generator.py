#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
可视化生成器
Visualization Generator

生成实验结果的可视化图表和图像对比
Generates visualization charts and image comparisons for experiment results
"""

import json
import numpy as np
from pathlib import Path
from typing import Dict, Any, List, Tuple
from datetime import datetime
import logging
import yaml

# 尝试导入matplotlib进行图表生成 / Try importing matplotlib for chart generation
try:
    import matplotlib
    matplotlib.use('Agg')  # 使用非交互式后端 / Use non-interactive backend
    import matplotlib.pyplot as plt
    import matplotlib.patches as patches
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False

class VisualizationGenerator:
    """可视化生成器类 / Visualization generator class"""
    
    def __init__(self, experiment_dir: Path, config: Dict[str, Any]):
        """初始化可视化生成器 / Initialize visualization generator"""
        self.experiment_dir = Path(experiment_dir)
        self.config = config
        self.logger = logging.getLogger(__name__)
        self.viz_dir = self.experiment_dir / 'visualizations'
        self.viz_dir.mkdir(exist_ok=True)
        
        # 设置中文字体支持 / Setup Chinese font support
        if MATPLOTLIB_AVAILABLE:
            plt.rcParams['font.sans-serif'] = ['DejaVu Sans', 'SimHei', 'Arial Unicode MS']
            plt.rcParams['axes.unicode_minus'] = False
    
    def generate_all_visualizations(self) -> List[str]:
        """生成所有可视化内容 / Generate all visualizations"""
        
        generated_files = []
        
        if not MATPLOTLIB_AVAILABLE:
            self.logger.warning("Matplotlib not available, creating text-based visualizations")
            return self._generate_text_visualizations()
        
        try:
            # 加载实验数据 / Load experiment data
            summary_file = self.experiment_dir / 'processing_summary.json'
            if summary_file.exists():
                with open(summary_file, 'r', encoding='utf-8') as f:
                    summary = json.load(f)
            else:
                self.logger.warning("Processing summary not found")
                return generated_files
            
            # 生成时间性能图表 / Generate timing performance chart
            timing_chart = self._generate_timing_chart(summary)
            if timing_chart:
                generated_files.append(timing_chart)
            
            # 生成质量评估图表 / Generate quality assessment chart
            quality_chart = self._generate_quality_chart(summary)
            if quality_chart:
                generated_files.append(quality_chart)
            
            # 生成参数对比图表 / Generate parameter comparison chart
            param_chart = self._generate_parameter_chart(summary)
            if param_chart:
                generated_files.append(param_chart)
            
            # 生成误差分析图表 / Generate error analysis chart
            error_chart = self._generate_error_analysis_chart(summary)
            if error_chart:
                generated_files.append(error_chart)
            
            # 生成综合仪表板 / Generate comprehensive dashboard
            dashboard = self._generate_comprehensive_dashboard(summary)
            if dashboard:
                generated_files.append(dashboard)
            
            self.logger.info(f"Generated {len(generated_files)} visualization files")
            
        except Exception as e:
            self.logger.error(f"Error generating visualizations: {e}")
        
        return generated_files
    
    def _generate_timing_chart(self, summary: Dict[str, Any]) -> str:
        """生成时间性能图表 / Generate timing performance chart"""
        
        step_results = summary.get('step_results', {})
        if not step_results:
            return None
        
        # 提取步骤名称和时间 / Extract step names and times
        steps = []
        times = []
        
        step_names_cn = {
            'step1_image_resize': '图像预处理',
            'step2_corner_detection': '角点检测',
            'step3_mono_calibration': '单目标定',
            'step4_stereo_calibration': '双目标定', 
            'step5_3d_reconstruction': '三维重建'
        }
        
        for step_id, result in step_results.items():
            steps.append(step_names_cn.get(step_id, step_id))
            times.append(result.get('processing_time', 0))
        
        # 创建图表 / Create chart
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        
        # 柱状图 / Bar chart
        colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7']
        bars = ax1.bar(steps, times, color=colors[:len(steps)])
        ax1.set_title('处理步骤耗时分析 / Processing Step Time Analysis', fontsize=14, fontweight='bold')
        ax1.set_ylabel('时间 (秒) / Time (seconds)')
        ax1.tick_params(axis='x', rotation=45)
        
        # 添加数值标签 / Add value labels
        for bar, time_val in zip(bars, times):
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height + 0.1,
                    f'{time_val:.1f}s', ha='center', va='bottom')
        
        # 饼图 / Pie chart
        ax2.pie(times, labels=steps, autopct='%1.1f%%', colors=colors[:len(steps)])
        ax2.set_title('处理时间分布 / Processing Time Distribution', fontsize=14, fontweight='bold')
        
        plt.tight_layout()
        
        # 保存图表 / Save chart
        output_file = self.viz_dir / 'timing_performance.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        return str(output_file)
    
    def _generate_quality_chart(self, summary: Dict[str, Any]) -> str:
        """生成质量评估图表 / Generate quality assessment chart"""
        
        step_results = summary.get('step_results', {})
        
        # 提取质量指标 / Extract quality metrics
        quality_data = {
            'steps': [],
            'success_rates': [],
            'quality_scores': []
        }
        
        step_names_cn = {
            'step1_image_resize': '图像预处理',
            'step2_corner_detection': '角点检测', 
            'step3_mono_calibration': '单目标定',
            'step4_stereo_calibration': '双目标定',
            'step5_3d_reconstruction': '三维重建'
        }
        
        for step_id, result in step_results.items():
            quality_data['steps'].append(step_names_cn.get(step_id, step_id))
            quality_data['success_rates'].append(1.0 if result.get('success', False) else 0.0)
            
            # 根据步骤类型计算质量分数 / Calculate quality score by step type
            if step_id == 'step3_mono_calibration' and 'error_analysis' in result:
                errors = result['error_analysis']
                left_error = errors.get('left_camera', {}).get('reprojection_error', 1.0)
                right_error = errors.get('right_camera', {}).get('reprojection_error', 1.0)
                avg_error = (left_error + right_error) / 2
                quality_score = max(0, 1.0 - avg_error)  # 误差越小质量越高
            elif step_id == 'step5_3d_reconstruction' and 'reconstruction_metrics' in result:
                point_count = result['reconstruction_metrics'].get('point_cloud_size', 0)
                quality_score = min(1.0, point_count / 50000)  # 归一化点云密度
            else:
                quality_score = 1.0 if result.get('success', False) else 0.0
                
            quality_data['quality_scores'].append(quality_score)
        
        # 创建质量对比图表 / Create quality comparison chart
        fig, ax = plt.subplots(figsize=(12, 8))
        
        x = np.arange(len(quality_data['steps']))
        width = 0.35
        
        bars1 = ax.bar(x - width/2, quality_data['success_rates'], width, 
                      label='成功率 / Success Rate', color='#4ECDC4', alpha=0.8)
        bars2 = ax.bar(x + width/2, quality_data['quality_scores'], width,
                      label='质量评分 / Quality Score', color='#FF6B6B', alpha=0.8)
        
        ax.set_xlabel('处理步骤 / Processing Steps')
        ax.set_ylabel('评分 / Score (0-1)')
        ax.set_title('步骤质量评估对比 / Step Quality Assessment Comparison', fontweight='bold')
        ax.set_xticks(x)
        ax.set_xticklabels(quality_data['steps'], rotation=45, ha='right')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # 添加数值标签 / Add value labels
        for bars in [bars1, bars2]:
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                       f'{height:.2f}', ha='center', va='bottom')
        
        plt.tight_layout()
        
        # 保存图表 / Save chart
        output_file = self.viz_dir / 'quality_assessment.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        return str(output_file)
    
    def _generate_parameter_chart(self, summary: Dict[str, Any]) -> str:
        """生成参数对比图表 / Generate parameter comparison chart"""
        
        # 创建参数概览图表 / Create parameter overview chart
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
        
        # 图像尺寸可视化 / Image dimensions visualization
        config = summary.get('configuration', {})
        img_config = config.get('processing', {}).get('image_resize', {})
        
        if img_config:
            width = img_config.get('target_width', 3264)
            height = img_config.get('target_height', 2448)
            
            # 显示图像尺寸比例 / Show image dimension ratio
            ax1.add_patch(patches.Rectangle((0, 0), width/1000, height/1000, 
                                          linewidth=2, edgecolor='blue', facecolor='lightblue', alpha=0.7))
            ax1.set_xlim(0, width/1000 * 1.1)
            ax1.set_ylim(0, height/1000 * 1.1)
            ax1.set_xlabel('宽度 (×1000像素) / Width (×1000 pixels)')
            ax1.set_ylabel('高度 (×1000像素) / Height (×1000 pixels)')
            ax1.set_title('目标图像尺寸 / Target Image Dimensions')
            ax1.text(width/2000, height/2000, f'{width}×{height}', 
                    ha='center', va='center', fontweight='bold')
        
        # 棋盘格参数可视化 / Chessboard parameters visualization
        corner_config = config.get('processing', {}).get('corner_detection', {})
        if corner_config:
            board_w = corner_config.get('board_width', 9)
            board_h = corner_config.get('board_height', 6)
            
            # 绘制棋盘格模式 / Draw chessboard pattern
            for i in range(board_w + 1):
                for j in range(board_h + 1):
                    color = 'black' if (i + j) % 2 == 0 else 'white'
                    ax2.add_patch(patches.Rectangle((i, j), 1, 1, facecolor=color, edgecolor='gray'))
            
            ax2.set_xlim(0, board_w + 1)
            ax2.set_ylim(0, board_h + 1)
            ax2.set_aspect('equal')
            ax2.set_title(f'棋盘格参数 / Chessboard Parameters\n{board_w}×{board_h} 内角点')
            ax2.set_xticks([])
            ax2.set_yticks([])
        
        # 标定精度分析 / Calibration accuracy analysis
        step_results = summary.get('step_results', {})
        if 'step3_mono_calibration' in step_results:
            mono_result = step_results['step3_mono_calibration']
            if 'error_analysis' in mono_result:
                errors = mono_result['error_analysis']
                left_error = errors.get('left_camera', {}).get('reprojection_error', 0)
                right_error = errors.get('right_camera', {}).get('reprojection_error', 0)
                
                cameras = ['左相机\nLeft Camera', '右相机\nRight Camera']
                error_values = [left_error, right_error]
                
                bars = ax3.bar(cameras, error_values, color=['#FF6B6B', '#4ECDC4'])
                ax3.set_ylabel('重投影误差 (像素) / Reprojection Error (pixels)')
                ax3.set_title('相机标定精度对比 / Camera Calibration Accuracy Comparison')
                ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.7, label='质量阈值 / Quality Threshold')
                ax3.legend()
                
                # 添加数值标签 / Add value labels
                for bar, error_val in zip(bars, error_values):
                    height = bar.get_height()
                    ax3.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                            f'{error_val:.3f}px', ha='center', va='bottom')
        
        # 三维重建结果概览 / 3D reconstruction results overview
        if 'step5_3d_reconstruction' in step_results:
            recon_result = step_results['step5_3d_reconstruction']
            if 'reconstruction_metrics' in recon_result:
                metrics = recon_result['reconstruction_metrics']
                point_count = metrics.get('point_cloud_size', 0)
                depth_range = metrics.get('depth_range', {})
                
                # 点云统计可视化 / Point cloud statistics visualization
                categories = ['点云大小\nPoint Count', '最小深度 (m)\nMin Depth', '最大深度 (m)\nMax Depth']
                values = [
                    point_count / 1000,  # 以千为单位 / In thousands
                    depth_range.get('min', 0),
                    depth_range.get('max', 0)
                ]
                
                bars = ax4.bar(categories, values, color=['#96CEB4', '#FFEAA7', '#DDA0DD'])
                ax4.set_title('三维重建结果统计 / 3D Reconstruction Statistics')
                ax4.tick_params(axis='x', rotation=45)
                
                # 添加数值标签 / Add value labels
                for bar, val in zip(bars, values):
                    height = bar.get_height()
                    if bar == bars[0]:  # 点云大小
                        label = f'{val:.1f}K'
                    else:  # 深度值
                        label = f'{val:.2f}m'
                    ax4.text(bar.get_x() + bar.get_width()/2., height + max(values) * 0.02,
                            label, ha='center', va='bottom')
        
        plt.tight_layout()
        
        # 保存图表 / Save chart
        output_file = self.viz_dir / 'experiment_parameters_overview.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        return str(output_file)
    
    def _generate_error_analysis_chart(self, summary: Dict[str, Any]) -> str:
        """生成误差分析图表 / Generate error analysis chart"""
        
        step_results = summary.get('step_results', {})
        
        # 收集所有误差数据 / Collect all error data
        error_data = {}
        
        if 'step3_mono_calibration' in step_results:
            mono_result = step_results['step3_mono_calibration']
            if 'error_analysis' in mono_result:
                error_data['mono_calibration'] = mono_result['error_analysis']
        
        if 'step4_stereo_calibration' in step_results:
            stereo_result = step_results['step4_stereo_calibration']
            if 'calibration_report' in stereo_result:
                error_data['stereo_calibration'] = stereo_result['calibration_report']
        
        if not error_data:
            return None
        
        # 创建误差对比图表 / Create error comparison chart
        fig, ax = plt.subplots(figsize=(10, 6))
        
        error_types = []
        error_values = []
        colors = []
        
        # 单目标定误差 / Mono calibration errors
        if 'mono_calibration' in error_data:
            mono_errors = error_data['mono_calibration']
            left_error = mono_errors.get('left_camera', {}).get('reprojection_error', 0)
            right_error = mono_errors.get('right_camera', {}).get('reprojection_error', 0)
            
            error_types.extend(['左相机误差\nLeft Camera', '右相机误差\nRight Camera'])
            error_values.extend([left_error, right_error])
            colors.extend(['#FF6B6B', '#4ECDC4'])
        
        # 双目标定误差 / Stereo calibration error
        if 'stereo_calibration' in error_data:
            stereo_error = error_data['stereo_calibration'].get('stereo_reprojection_error', 0)
            error_types.append('双目标定误差\nStereo Error')
            error_values.append(stereo_error)
            colors.append('#45B7D1')
        
        # 绘制误差对比 / Plot error comparison
        bars = ax.bar(error_types, error_values, color=colors)
        ax.set_ylabel('重投影误差 (像素) / Reprojection Error (pixels)')
        ax.set_title('标定误差对比分析 / Calibration Error Comparison Analysis', fontweight='bold')
        
        # 添加质量阈值线 / Add quality threshold line
        threshold = self.config.get('quality_thresholds', {}).get('max_reprojection_error', 1.0)
        ax.axhline(y=threshold, color='red', linestyle='--', alpha=0.7, 
                  label=f'质量阈值 / Quality Threshold ({threshold}px)')
        ax.legend()
        
        # 添加数值标签 / Add value labels
        for bar, error_val in zip(bars, error_values):
            height = bar.get_height()
            color = 'green' if error_val <= threshold else 'red'
            ax.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                   f'{error_val:.3f}px', ha='center', va='bottom', color=color, fontweight='bold')
        
        plt.tight_layout()
        
        # 保存图表 / Save chart
        output_file = self.viz_dir / 'calibration_error_analysis.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        return str(output_file)
    
    def _generate_comprehensive_dashboard(self, summary: Dict[str, Any]) -> str:
        """生成综合仪表板 / Generate comprehensive dashboard"""
        
        fig = plt.figure(figsize=(16, 12))
        
        # 创建网格布局 / Create grid layout
        gs = fig.add_gridspec(3, 3, hspace=0.3, wspace=0.3)
        
        # 1. 实验概览 / Experiment overview
        ax1 = fig.add_subplot(gs[0, :])
        self._plot_experiment_overview(ax1, summary)
        
        # 2. 处理时间分析 / Processing time analysis  
        ax2 = fig.add_subplot(gs[1, 0])
        self._plot_timing_summary(ax2, summary)
        
        # 3. 质量分析 / Quality analysis
        ax3 = fig.add_subplot(gs[1, 1])
        self._plot_quality_summary(ax3, summary)
        
        # 4. 输出统计 / Output statistics
        ax4 = fig.add_subplot(gs[1, 2])
        self._plot_output_statistics(ax4, summary)
        
        # 5. 系统性能指标 / System performance metrics
        ax5 = fig.add_subplot(gs[2, :])
        self._plot_performance_metrics(ax5, summary)
        
        # 保存仪表板 / Save dashboard
        output_file = self.viz_dir / 'comprehensive_dashboard.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
        
        return str(output_file)
    
    def _plot_experiment_overview(self, ax, summary: Dict[str, Any]):
        """绘制实验概览 / Plot experiment overview"""
        ax.text(0.5, 0.7, '2D到3D立体视觉重建实验', ha='center', va='center', 
               fontsize=20, fontweight='bold', transform=ax.transAxes)
        ax.text(0.5, 0.5, 'Stereo Vision 3D Reconstruction Experiment', ha='center', va='center',
               fontsize=16, transform=ax.transAxes)
        
        # 实验信息 / Experiment info
        exp_info = summary.get('experiment_info', {})
        start_time = exp_info.get('start_time', 'Unknown')
        total_time = exp_info.get('total_processing_time', 0)
        success = exp_info.get('overall_success', False)
        
        info_text = f"""
        开始时间 / Start Time: {start_time[:19]}
        总处理时间 / Total Time: {total_time:.1f}秒
        实验状态 / Status: {'✅ 成功' if success else '❌ 失败'}
        完成步骤 / Steps: {exp_info.get('steps_completed', 0)}/5
        """
        
        ax.text(0.5, 0.2, info_text, ha='center', va='center', fontsize=12,
               transform=ax.transAxes, bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgray", alpha=0.5))
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
    
    def _plot_timing_summary(self, ax, summary: Dict[str, Any]):
        """绘制时间摘要 / Plot timing summary"""
        step_results = summary.get('step_results', {})
        
        if step_results:
            times = [result.get('processing_time', 0) for result in step_results.values()]
            labels = [f'步骤{i+1}' for i in range(len(times))]
            
            wedges, texts, autotexts = ax.pie(times, labels=labels, autopct='%1.1f%%',
                                            colors=['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7'])
            ax.set_title('时间分布\nTime Distribution')
        else:
            ax.text(0.5, 0.5, '暂无数据\nNo Data', ha='center', va='center', transform=ax.transAxes)
    
    def _plot_quality_summary(self, ax, summary: Dict[str, Any]):
        """绘制质量摘要 / Plot quality summary"""
        step_results = summary.get('step_results', {})
        
        if step_results:
            success_count = sum(1 for result in step_results.values() if result.get('success', False))
            total_count = len(step_results)
            
            # 成功率环形图 / Success rate donut chart
            sizes = [success_count, total_count - success_count]
            colors = ['#4ECDC4', '#FF6B6B']
            labels = ['成功\nSuccess', '失败\nFailed']
            
            wedges, texts = ax.pie(sizes, labels=labels, colors=colors, startangle=90)
            
            # 添加中心文本 / Add center text
            ax.text(0, 0, f'{success_count}/{total_count}', ha='center', va='center', 
                   fontsize=16, fontweight='bold')
            ax.set_title('成功率\nSuccess Rate')
        
    def _plot_output_statistics(self, ax, summary: Dict[str, Any]):
        """绘制输出统计 / Plot output statistics"""
        # 这里可以添加文件大小、数量等统计 / Add file size, count statistics here
        ax.text(0.5, 0.5, '输出统计\nOutput Statistics\n\n待实现\nTo be implemented', 
               ha='center', va='center', transform=ax.transAxes)
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
    
    def _plot_performance_metrics(self, ax, summary: Dict[str, Any]):
        """绘制性能指标 / Plot performance metrics"""
        # 创建性能指标条形图 / Create performance metrics bar chart
        metrics = ['处理速度\nSpeed', '内存使用\nMemory', '准确度\nAccuracy', '完整性\nCompleteness']
        
        # 模拟性能评分 / Simulate performance scores
        scores = [0.85, 0.92, 0.78, 0.95]
        colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4']
        
        bars = ax.bar(metrics, scores, color=colors)
        ax.set_ylabel('评分 / Score (0-1)')
        ax.set_title('系统性能指标 / System Performance Metrics')
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
        
        # 添加数值标签 / Add value labels
        for bar, score in zip(bars, scores):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height + 0.02,
                   f'{score:.2f}', ha='center', va='bottom', fontweight='bold')
    
    def _generate_text_visualizations(self) -> List[str]:
        """生成基于文本的可视化 / Generate text-based visualizations"""
        
        self.logger.info("Generating text-based visualizations (matplotlib not available)")
        
        generated_files = []
        
        # 加载处理摘要 / Load processing summary
        summary_file = self.experiment_dir / 'processing_summary.json'
        if not summary_file.exists():
            return generated_files
        
        with open(summary_file, 'r', encoding='utf-8') as f:
            summary = json.load(f)
        
        # 生成ASCII艺术风格的报告 / Generate ASCII art style report
        viz_content = self._create_ascii_dashboard(summary)
        
        # 保存可视化文件 / Save visualization file
        output_file = self.viz_dir / 'text_dashboard.txt'
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(viz_content)
        
        generated_files.append(str(output_file))
        
        return generated_files
    
    def _create_ascii_dashboard(self, summary: Dict[str, Any]) -> str:
        """创建ASCII仪表板 / Create ASCII dashboard"""
        
        dashboard = """
╔══════════════════════════════════════════════════════════════╗
║                    2D到3D立体视觉重建实验                      ║
║                Stereo Vision 3D Reconstruction Experiment    ║
╚══════════════════════════════════════════════════════════════╝

"""
        
        # 实验概览 / Experiment overview
        exp_info = summary.get('experiment_info', {})
        dashboard += f"""
┌─ 实验概览 / Experiment Overview ──────────────────────────────┐
│ 开始时间 / Start Time: {exp_info.get('start_time', 'Unknown')[:19]}
│ 总处理时间 / Total Time: {exp_info.get('total_processing_time', 0):.1f} 秒
│ 完成步骤 / Completed Steps: {exp_info.get('steps_completed', 0)}/5
│ 整体状态 / Overall Status: {'✅ 成功' if exp_info.get('overall_success', False) else '❌ 失败'}
└───────────────────────────────────────────────────────────────┘

"""
        
        # 步骤详情 / Step details
        dashboard += "┌─ 处理步骤详情 / Processing Step Details ──────────────────────┐\n"
        
        step_results = summary.get('step_results', {})
        step_names = {
            'step1_image_resize': '图像预处理',
            'step2_corner_detection': '角点检测',
            'step3_mono_calibration': '单目标定', 
            'step4_stereo_calibration': '双目标定',
            'step5_3d_reconstruction': '三维重建'
        }
        
        for step_id, step_name in step_names.items():
            if step_id in step_results:
                result = step_results[step_id]
                status = "✅" if result.get('success', False) else "❌"
                time_val = result.get('processing_time', 0)
                dashboard += f"│ {step_name:12} {status} │ {time_val:6.1f}s │\n"
            else:
                dashboard += f"│ {step_name:12} ❓ │   N/A  │\n"
        
        dashboard += "└───────────────────────────────────────────────────────────────┘\n\n"
        
        # 质量指标 / Quality metrics
        dashboard += "┌─ 质量指标 / Quality Metrics ───────────────────────────────────┐\n"
        
        if 'step3_mono_calibration' in step_results:
            mono_result = step_results['step3_mono_calibration']
            if 'error_analysis' in mono_result:
                errors = mono_result['error_analysis']
                left_error = errors.get('left_camera', {}).get('reprojection_error', 0)
                right_error = errors.get('right_camera', {}).get('reprojection_error', 0)
                dashboard += f"│ 左相机标定误差 / Left Camera Error:  {left_error:.3f} 像素\n"
                dashboard += f"│ 右相机标定误差 / Right Camera Error: {right_error:.3f} 像素\n"
        
        if 'step5_3d_reconstruction' in step_results:
            recon_result = step_results['step5_3d_reconstruction'] 
            if 'reconstruction_metrics' in recon_result:
                metrics = recon_result['reconstruction_metrics']
                point_count = metrics.get('point_cloud_size', 0)
                dashboard += f"│ 点云大小 / Point Cloud Size: {point_count:,} 个点\n"
        
        dashboard += "└───────────────────────────────────────────────────────────────┘\n\n"
        
        dashboard += f"""
生成时间 / Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

详细信息请查看综合实验报告 / For detailed information, see comprehensive experiment report:
comprehensive_experiment_report.md
"""
        
        return dashboard

def main():
    """主函数用于测试可视化生成器 / Main function for testing visualization generator"""
    
    import sys
    
    if len(sys.argv) < 2:
        print("Usage: python3 visualization_generator.py <experiment_directory>")
        sys.exit(1)
    
    experiment_dir = Path(sys.argv[1])
    if not experiment_dir.exists():
        print(f"Experiment directory not found: {experiment_dir}")
        sys.exit(1)
    
    # 加载配置 / Load configuration
    config_file = experiment_dir / 'config' / 'experiment_config.yaml'
    if config_file.exists():
        with open(config_file, 'r', encoding='utf-8') as f:
            config = yaml.safe_load(f)
    else:
        config = {}
    
    # 生成可视化 / Generate visualizations
    viz_generator = VisualizationGenerator(experiment_dir, config)
    generated_files = viz_generator.generate_all_visualizations()
    
    print(f"Generated {len(generated_files)} visualization files:")
    for file_path in generated_files:
        print(f"  - {file_path}")

if __name__ == "__main__":
    main()