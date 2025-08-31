#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
结果分析器
Result Analyzer

分析实验结果，生成质量评估和性能基准报告
Analyzes experiment results and generates quality assessment and performance benchmark reports
"""

import json
import time
import psutil
import numpy as np
from pathlib import Path
from typing import Dict, Any, List, Tuple
from datetime import datetime
import logging

class ResultAnalyzer:
    """实验结果分析器类 / Experiment result analyzer class"""
    
    def __init__(self, experiment_dir: Path, config: Dict[str, Any]):
        """初始化结果分析器 / Initialize result analyzer"""
        self.experiment_dir = Path(experiment_dir)
        self.config = config
        self.logger = logging.getLogger(__name__)
        self.analysis_results = {}
        
    def analyze_calibration_accuracy(self, calibration_data: Dict[str, Any]) -> Dict[str, Any]:
        """分析标定精度 / Analyze calibration accuracy"""
        
        analysis = {
            'timestamp': datetime.now().isoformat(),
            'calibration_type': 'mono_stereo',
            'quality_assessment': {}
        }
        
        # 分析单目标定精度 / Analyze mono calibration accuracy
        if 'error_analysis' in calibration_data:
            error_data = calibration_data['error_analysis']
            
            left_error = error_data.get('left_camera', {}).get('reprojection_error', 999)
            right_error = error_data.get('right_camera', {}).get('reprojection_error', 999)
            
            analysis['mono_calibration'] = {
                'left_camera_error': left_error,
                'right_camera_error': right_error,
                'average_error': (left_error + right_error) / 2,
                'error_difference': abs(left_error - right_error),
                'quality_grade': self._assess_calibration_quality(left_error, right_error)
            }
            
            # 评估标定一致性 / Assess calibration consistency
            error_threshold = self.config.get('quality_thresholds', {}).get('max_reprojection_error', 1.0)
            analysis['consistency_check'] = {
                'within_threshold': left_error < error_threshold and right_error < error_threshold,
                'error_balance': abs(left_error - right_error) < 0.3,
                'overall_consistency': 'GOOD' if abs(left_error - right_error) < 0.2 else 'MODERATE'
            }
        
        return analysis
    
    def analyze_reconstruction_quality(self, reconstruction_data: Dict[str, Any]) -> Dict[str, Any]:
        """分析三维重建质量 / Analyze 3D reconstruction quality"""
        
        analysis = {
            'timestamp': datetime.now().isoformat(),
            'reconstruction_assessment': {}
        }
        
        if 'reconstruction_metrics' in reconstruction_data:
            metrics = reconstruction_data['reconstruction_metrics']
            point_count = metrics.get('point_cloud_size', 0)
            
            analysis['point_cloud_analysis'] = {
                'total_points': point_count,
                'density_level': self._assess_point_density(point_count),
                'quality_rating': metrics.get('reconstruction_quality', 'Unknown'),
                'completeness_score': min(point_count / 50000, 1.0)  # 基于50k点的完整性评分
            }
            
            # 深度范围分析 / Depth range analysis
            depth_range = metrics.get('depth_range', {})
            if depth_range:
                analysis['depth_analysis'] = {
                    'min_depth': depth_range.get('min', 0),
                    'max_depth': depth_range.get('max', 0),
                    'depth_span': depth_range.get('max', 0) - depth_range.get('min', 0),
                    'working_volume': self._calculate_working_volume(depth_range)
                }
        
        return analysis
    
    def analyze_processing_performance(self, step_results: Dict[str, Any]) -> Dict[str, Any]:
        """分析处理性能 / Analyze processing performance"""
        
        performance_analysis = {
            'timestamp': datetime.now().isoformat(),
            'system_info': self._get_system_info(),
            'performance_metrics': {}
        }
        
        # 时间性能分析 / Time performance analysis
        processing_times = {}
        total_time = 0
        
        for step_name, result in step_results.items():
            processing_time = result.get('processing_time', 0)
            processing_times[step_name] = processing_time
            total_time += processing_time
        
        performance_analysis['timing_analysis'] = {
            'individual_steps': processing_times,
            'total_time': total_time,
            'average_step_time': total_time / len(processing_times) if processing_times else 0,
            'bottleneck_step': max(processing_times.items(), key=lambda x: x[1]) if processing_times else None
        }
        
        # 效率评估 / Efficiency assessment
        performance_analysis['efficiency_metrics'] = {
            'steps_per_minute': len(processing_times) / (total_time / 60) if total_time > 0 else 0,
            'processing_rate': 'FAST' if total_time < 30 else 'NORMAL' if total_time < 120 else 'SLOW',
            'optimization_potential': self._assess_optimization_potential(processing_times)
        }
        
        return performance_analysis
    
    def generate_performance_benchmark(self) -> Dict[str, Any]:
        """生成性能基准报告 / Generate performance benchmark report"""
        
        benchmark = {
            'benchmark_info': {
                'generated_at': datetime.now().isoformat(),
                'experiment_dir': str(self.experiment_dir),
                'system_specs': self._get_system_info()
            },
            'baseline_metrics': {
                'target_processing_time': 60,  # seconds
                'target_accuracy': 0.8,        # pixel error
                'target_point_density': 10000   # points
            }
        }
        
        # 加载实验结果 / Load experiment results
        summary_file = self.experiment_dir / 'processing_summary.json'
        if summary_file.exists():
            with open(summary_file, 'r', encoding='utf-8') as f:
                summary = json.load(f)
                
            benchmark['actual_metrics'] = self._extract_actual_metrics(summary)
            benchmark['performance_comparison'] = self._compare_with_baseline(benchmark)
        
        return benchmark
    
    def _assess_calibration_quality(self, left_error: float, right_error: float) -> str:
        """评估标定质量等级 / Assess calibration quality grade"""
        avg_error = (left_error + right_error) / 2
        
        if avg_error < 0.3:
            return "EXCELLENT"
        elif avg_error < 0.5:
            return "VERY_GOOD"
        elif avg_error < 0.8:
            return "GOOD"
        elif avg_error < 1.0:
            return "ACCEPTABLE"
        else:
            return "POOR"
    
    def _assess_point_density(self, point_count: int) -> str:
        """评估点云密度等级 / Assess point cloud density level"""
        if point_count > 100000:
            return "VERY_HIGH"
        elif point_count > 50000:
            return "HIGH" 
        elif point_count > 10000:
            return "MEDIUM"
        elif point_count > 1000:
            return "LOW"
        else:
            return "VERY_LOW"
    
    def _calculate_working_volume(self, depth_range: Dict[str, float]) -> float:
        """计算工作体积 / Calculate working volume"""
        # 简化的工作体积计算 / Simplified working volume calculation
        min_depth = depth_range.get('min', 0)
        max_depth = depth_range.get('max', 0)
        
        if min_depth > 0 and max_depth > min_depth:
            # 假设标准FOV计算体积 / Assume standard FOV for volume calculation
            volume = (max_depth - min_depth) * 1.0 * 0.75  # 深度 × 宽度 × 高度 (米³)
            return round(volume, 3)
        
        return 0.0
    
    def _get_system_info(self) -> Dict[str, Any]:
        """获取系统信息 / Get system information"""
        try:
            return {
                'cpu_count': psutil.cpu_count(),
                'memory_total_gb': round(psutil.virtual_memory().total / (1024**3), 2),
                'memory_available_gb': round(psutil.virtual_memory().available / (1024**3), 2),
                'disk_usage_gb': round(psutil.disk_usage('.').free / (1024**3), 2)
            }
        except:
            return {
                'cpu_count': 'Unknown',
                'memory_total_gb': 'Unknown',
                'memory_available_gb': 'Unknown',
                'disk_usage_gb': 'Unknown'
            }
    
    def _assess_optimization_potential(self, processing_times: Dict[str, float]) -> List[str]:
        """评估优化潜力 / Assess optimization potential"""
        suggestions = []
        
        # 找出最耗时的步骤 / Find most time-consuming steps
        if processing_times:
            max_time_step = max(processing_times.items(), key=lambda x: x[1])
            total_time = sum(processing_times.values())
            
            if max_time_step[1] > total_time * 0.5:
                suggestions.append(f"优化 {max_time_step[0]} - 占用了总时间的 {max_time_step[1]/total_time:.1%}")
                
            if total_time > 120:
                suggestions.append("考虑降低图像分辨率以提高处理速度")
                
            if len(processing_times) > 3:
                suggestions.append("实现步骤并行化以减少总处理时间")
        
        return suggestions
    
    def _extract_actual_metrics(self, summary: Dict[str, Any]) -> Dict[str, Any]:
        """提取实际指标 / Extract actual metrics"""
        step_results = summary.get('step_results', {})
        
        # 提取关键性能指标 / Extract key performance metrics
        actual = {
            'total_processing_time': summary.get('experiment_info', {}).get('total_processing_time', 0),
            'calibration_accuracy': 0,
            'point_cloud_size': 0,
            'success_rate': 0
        }
        
        # 计算标定精度 / Calculate calibration accuracy
        if 'step3_mono_calibration' in step_results:
            mono_result = step_results['step3_mono_calibration']
            if 'error_analysis' in mono_result:
                errors = mono_result['error_analysis']
                left_error = errors.get('left_camera', {}).get('reprojection_error', 999)
                right_error = errors.get('right_camera', {}).get('reprojection_error', 999)
                actual['calibration_accuracy'] = (left_error + right_error) / 2
        
        # 提取点云大小 / Extract point cloud size
        if 'step5_3d_reconstruction' in step_results:
            recon_result = step_results['step5_3d_reconstruction']
            if 'reconstruction_metrics' in recon_result:
                actual['point_cloud_size'] = recon_result['reconstruction_metrics'].get('point_cloud_size', 0)
        
        # 计算成功率 / Calculate success rate
        successful_steps = sum(1 for result in step_results.values() if result.get('success', False))
        actual['success_rate'] = successful_steps / len(step_results) if step_results else 0
        
        return actual
    
    def _compare_with_baseline(self, benchmark: Dict[str, Any]) -> Dict[str, Any]:
        """与基准进行比较 / Compare with baseline"""
        baseline = benchmark['baseline_metrics']
        actual = benchmark['actual_metrics']
        
        comparison = {
            'time_performance': {
                'baseline': baseline['target_processing_time'],
                'actual': actual['total_processing_time'],
                'ratio': actual['total_processing_time'] / baseline['target_processing_time'],
                'assessment': 'BETTER' if actual['total_processing_time'] < baseline['target_processing_time'] else 'SLOWER'
            },
            'accuracy_performance': {
                'baseline': baseline['target_accuracy'],
                'actual': actual['calibration_accuracy'],
                'assessment': 'BETTER' if actual['calibration_accuracy'] < baseline['target_accuracy'] else 'WORSE'
            },
            'density_performance': {
                'baseline': baseline['target_point_density'],
                'actual': actual['point_cloud_size'],
                'ratio': actual['point_cloud_size'] / baseline['target_point_density'],
                'assessment': 'BETTER' if actual['point_cloud_size'] > baseline['target_point_density'] else 'LOWER'
            }
        }
        
        return comparison
    
    def save_analysis_results(self) -> str:
        """保存分析结果 / Save analysis results"""
        analysis_file = self.experiment_dir / 'analysis_results.json'
        
        with open(analysis_file, 'w', encoding='utf-8') as f:
            json.dump(self.analysis_results, f, indent=2, ensure_ascii=False)
            
        self.logger.info(f"Analysis results saved to: {analysis_file}")
        return str(analysis_file)

class MemoryMonitor:
    """内存监控器 / Memory monitor"""
    
    def __init__(self):
        self.memory_snapshots = []
        self.start_memory = psutil.virtual_memory().used
        
    def take_snapshot(self, label: str):
        """记录内存快照 / Take memory snapshot"""
        try:
            current_memory = psutil.virtual_memory()
            self.memory_snapshots.append({
                'timestamp': datetime.now().isoformat(),
                'label': label,
                'used_gb': round(current_memory.used / (1024**3), 3),
                'available_gb': round(current_memory.available / (1024**3), 3),
                'percent_used': current_memory.percent
            })
        except:
            pass  # 忽略监控错误 / Ignore monitoring errors
    
    def get_memory_report(self) -> Dict[str, Any]:
        """获取内存使用报告 / Get memory usage report"""
        if not self.memory_snapshots:
            return {'error': 'No memory snapshots recorded'}
        
        max_memory = max(self.memory_snapshots, key=lambda x: x['used_gb'])
        min_memory = min(self.memory_snapshots, key=lambda x: x['used_gb'])
        
        return {
            'snapshots': self.memory_snapshots,
            'peak_usage': max_memory,
            'minimum_usage': min_memory,
            'memory_delta': max_memory['used_gb'] - min_memory['used_gb'],
            'average_usage': sum(s['used_gb'] for s in self.memory_snapshots) / len(self.memory_snapshots)
        }

class FileAnalyzer:
    """文件分析器 / File analyzer"""
    
    def __init__(self, experiment_dir: Path):
        self.experiment_dir = Path(experiment_dir)
        
    def analyze_output_files(self) -> Dict[str, Any]:
        """分析输出文件 / Analyze output files"""
        
        file_analysis = {
            'timestamp': datetime.now().isoformat(),
            'directory_structure': {},
            'file_statistics': {},
            'total_size_mb': 0
        }
        
        # 分析目录结构 / Analyze directory structure
        for root in self.experiment_dir.rglob('*'):
            if root.is_dir():
                relative_path = root.relative_to(self.experiment_dir)
                file_count = len([f for f in root.iterdir() if f.is_file()])
                
                file_analysis['directory_structure'][str(relative_path)] = {
                    'file_count': file_count,
                    'subdirectories': len([d for d in root.iterdir() if d.is_dir()])
                }
        
        # 分析文件统计 / Analyze file statistics
        all_files = list(self.experiment_dir.rglob('*'))
        total_size = 0
        
        file_types = {}
        for file_path in all_files:
            if file_path.is_file():
                file_size = file_path.stat().st_size
                total_size += file_size
                
                suffix = file_path.suffix.lower()
                if suffix in file_types:
                    file_types[suffix]['count'] += 1
                    file_types[suffix]['total_size'] += file_size
                else:
                    file_types[suffix] = {'count': 1, 'total_size': file_size}
        
        file_analysis['file_statistics'] = {
            'total_files': len([f for f in all_files if f.is_file()]),
            'total_size_mb': round(total_size / (1024**2), 2),
            'file_types': {k: {'count': v['count'], 'size_mb': round(v['total_size'] / (1024**2), 2)} 
                          for k, v in file_types.items()}
        }
        
        return file_analysis
    
    def validate_output_completeness(self, expected_outputs: List[str]) -> Dict[str, Any]:
        """验证输出完整性 / Validate output completeness"""
        
        validation = {
            'timestamp': datetime.now().isoformat(),
            'expected_files': expected_outputs,
            'validation_results': {}
        }
        
        missing_files = []
        existing_files = []
        
        for expected_file in expected_outputs:
            file_path = self.experiment_dir / expected_file
            if file_path.exists():
                existing_files.append(expected_file)
            else:
                missing_files.append(expected_file)
        
        validation['validation_results'] = {
            'completeness_rate': len(existing_files) / len(expected_outputs) if expected_outputs else 1.0,
            'missing_files': missing_files,
            'existing_files': existing_files,
            'overall_status': 'COMPLETE' if not missing_files else 'INCOMPLETE'
        }
        
        return validation

class QualityAssessment:
    """质量评估器 / Quality assessor"""
    
    def __init__(self, config: Dict[str, Any]):
        self.thresholds = config.get('quality_thresholds', {})
        
    def assess_overall_quality(self, step_results: Dict[str, Any]) -> Dict[str, Any]:
        """评估整体质量 / Assess overall quality"""
        
        assessment = {
            'timestamp': datetime.now().isoformat(),
            'quality_scores': {},
            'overall_grade': '',
            'recommendations': []
        }
        
        scores = []
        
        # 评估各步骤质量 / Assess quality of each step
        for step_name, result in step_results.items():
            step_score = self._assess_step_quality(step_name, result)
            assessment['quality_scores'][step_name] = step_score
            scores.append(step_score)
        
        # 计算总体评分 / Calculate overall score
        overall_score = sum(scores) / len(scores) if scores else 0
        assessment['overall_score'] = overall_score
        
        # 确定等级 / Determine grade
        if overall_score >= 0.9:
            assessment['overall_grade'] = 'A - 优秀 / Excellent'
        elif overall_score >= 0.8:
            assessment['overall_grade'] = 'B - 良好 / Good'
        elif overall_score >= 0.7:
            assessment['overall_grade'] = 'C - 合格 / Satisfactory'
        else:
            assessment['overall_grade'] = 'D - 需要改进 / Needs Improvement'
        
        # 生成建议 / Generate recommendations
        assessment['recommendations'] = self._generate_quality_recommendations(step_results)
        
        return assessment
    
    def _assess_step_quality(self, step_name: str, result: Dict[str, Any]) -> float:
        """评估单个步骤质量 / Assess individual step quality"""
        
        if not result.get('success', False):
            return 0.0
        
        # 基于步骤特定指标评估 / Assess based on step-specific metrics
        if step_name == 'step3_mono_calibration' and 'error_analysis' in result:
            errors = result['error_analysis']
            left_error = errors.get('left_camera', {}).get('reprojection_error', 999)
            right_error = errors.get('right_camera', {}).get('reprojection_error', 999)
            avg_error = (left_error + right_error) / 2
            
            max_error = self.thresholds.get('max_reprojection_error', 1.0)
            return max(0, 1.0 - (avg_error / max_error))
        
        elif step_name == 'step5_3d_reconstruction' and 'reconstruction_metrics' in result:
            metrics = result['reconstruction_metrics']
            point_count = metrics.get('point_cloud_size', 0)
            min_points = self.thresholds.get('min_point_cloud_size', 1000)
            
            return min(1.0, point_count / (min_points * 10))  # 10倍最小值为满分
        
        # 默认基于成功状态评分 / Default scoring based on success status
        return 1.0
    
    def _generate_quality_recommendations(self, step_results: Dict[str, Any]) -> List[str]:
        """生成质量改进建议 / Generate quality improvement recommendations"""
        
        recommendations = []
        
        for step_name, result in step_results.items():
            if not result.get('success', False):
                recommendations.append(f"修复 {step_name} 的执行失败问题")
            
            elif step_name == 'step3_mono_calibration' and 'error_analysis' in result:
                errors = result['error_analysis']
                left_error = errors.get('left_camera', {}).get('reprojection_error', 0)
                right_error = errors.get('right_camera', {}).get('reprojection_error', 0)
                
                if max(left_error, right_error) > 1.0:
                    recommendations.append("单目标定误差较大，建议增加标定图像数量或改善图像质量")
                
                if abs(left_error - right_error) > 0.3:
                    recommendations.append("左右相机标定误差差异较大，检查相机设置和图像质量")
        
        if not recommendations:
            recommendations.append("所有步骤质量良好，无需特别优化")
        
        return recommendations