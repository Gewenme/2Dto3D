#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
综合实验报告生成主程序
Comprehensive Experiment Report Generator Main Program

这是2D到3D转换流程自动化实验文档生成器的主入口点
This is the main entry point for the automated 2D to 3D conversion experiment documentation generator
"""

import os
import sys
import json
import yaml
import argparse
import subprocess
from datetime import datetime
from pathlib import Path
from typing import Dict, Any, Optional
import logging

# 导入自定义模块 / Import custom modules
from experiment_controller import ExperimentController
from report_generator import ReportGenerator
from result_analyzer import ResultAnalyzer, MemoryMonitor, FileAnalyzer, QualityAssessment
from visualization_generator import VisualizationGenerator

class ComprehensiveExperimentSystem:
    """综合实验系统 / Comprehensive experiment system"""
    
    def __init__(self, config_path: str = "config.yaml", output_dir: Optional[str] = None):
        """初始化综合实验系统 / Initialize comprehensive experiment system"""
        
        self.config_path = config_path
        self.config = self._load_config()
        
        # 设置输出目录 / Setup output directory
        if output_dir:
            self.base_output_dir = Path(output_dir)
        else:
            base_name = self.config['paths']['output']['base_dir']
            if self.config['paths']['output'].get('use_timestamp', True):
                timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
                self.base_output_dir = Path(f"{base_name}_{timestamp}")
            else:
                self.base_output_dir = Path(base_name)
        
        # 初始化监控器 / Initialize monitors
        self.memory_monitor = MemoryMonitor()
        self.start_time = datetime.now()
        
        # 设置日志 / Setup logging
        self._setup_logging()
        
    def _load_config(self) -> Dict[str, Any]:
        """加载配置文件 / Load configuration file"""
        try:
            with open(self.config_path, 'r', encoding='utf-8') as f:
                return yaml.safe_load(f)
        except Exception as e:
            print(f"Warning: Could not load config file {self.config_path}: {e}")
            return self._get_default_config()
    
    def _get_default_config(self) -> Dict[str, Any]:
        """获取默认配置 / Get default configuration"""
        return {
            'experiment': {
                'name': '2D到3D立体视觉重建自动化实验',
                'version': '1.0.0',
                'author': '自动化实验系统'
            },
            'paths': {
                'input': {
                    'left_images': 'input/left',
                    'right_images': 'input/right'
                },
                'output': {
                    'base_dir': 'experiment_results',
                    'use_timestamp': True
                }
            },
            'processing': {
                'image_resize': {
                    'target_width': 3264,
                    'target_height': 2448,
                    'interpolation': 'LINEAR'
                },
                'corner_detection': {
                    'board_width': 9,
                    'board_height': 6,
                    'scale_factor': 1.0
                },
                'calibration': {
                    'square_size': 0.0082,
                    'save_undistorted': True
                },
                'reconstruction': {
                    'output_format': 0,
                    'quality_level': 3
                }
            },
            'documentation': {
                'generate_markdown': True,
                'generate_pdf': True,
                'language': 'zh_CN'
            },
            'quality_thresholds': {
                'max_reprojection_error': 1.0,
                'min_point_cloud_size': 1000,
                'max_processing_time': 300
            }
        }
    
    def _setup_logging(self):
        """设置日志系统 / Setup logging system"""
        # 确保日志目录存在 / Ensure log directory exists
        log_dir = self.base_output_dir / 'logs'
        log_dir.mkdir(parents=True, exist_ok=True)
        
        log_file = log_dir / 'comprehensive_experiment.log'
        
        # 配置日志格式 / Configure logging format
        log_format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        
        logging.basicConfig(
            level=logging.INFO,
            format=log_format,
            handlers=[
                logging.FileHandler(log_file, encoding='utf-8'),
                logging.StreamHandler(sys.stdout)
            ]
        )
        
        self.logger = logging.getLogger(__name__)
        self.logger.info("Comprehensive Experiment System initialized")
        self.logger.info(f"Output directory: {self.base_output_dir}")
    
    def run_complete_experiment(self) -> bool:
        """运行完整实验流程 / Run complete experiment pipeline"""
        
        self.logger.info("="*60)
        self.logger.info("启动2D到3D转换流程自动化实验")
        self.logger.info("Starting Automated 2D to 3D Conversion Experiment")
        self.logger.info("="*60)
        
        try:
            # 记录初始内存状态 / Record initial memory state
            self.memory_monitor.take_snapshot("实验开始 / Experiment Start")
            
            # Phase 1: 运行处理流程 / Run processing pipeline
            self.logger.info("Phase 1: 执行处理流程 / Executing Processing Pipeline")
            controller = ExperimentController(self.config_path)
            
            # 确保使用相同的输出目录 / Ensure using same output directory
            controller.output_dir = self.base_output_dir
            controller._setup_logging()  # 重新设置日志以使用正确目录
            
            success = controller.run_experiment()
            
            if not success:
                self.logger.error("处理流程执行失败 / Processing pipeline execution failed")
                return False
            
            self.memory_monitor.take_snapshot("处理完成 / Processing Complete")
            
            # Phase 2: 结果分析 / Result analysis
            self.logger.info("Phase 2: 分析实验结果 / Analyzing Experiment Results")
            analyzer = ResultAnalyzer(self.base_output_dir, self.config)
            
            # 执行各类分析 / Perform various analyses
            self._perform_comprehensive_analysis(analyzer, controller.step_results)
            
            self.memory_monitor.take_snapshot("分析完成 / Analysis Complete")
            
            # Phase 3: 生成报告 / Generate reports
            self.logger.info("Phase 3: 生成综合报告 / Generating Comprehensive Reports")
            report_generator = ReportGenerator(self.base_output_dir, self.config)
            
            # 生成可视化内容 / Generate visualizations
            if self.config.get('visualization', {}).get('generate_comparison_images', True):
                self.logger.info("生成可视化内容 / Generating visualizations")
                viz_generator = VisualizationGenerator(self.base_output_dir, self.config)
                viz_files = viz_generator.generate_all_visualizations()
                self.logger.info(f"Generated {len(viz_files)} visualization files")
            
            # 生成Markdown报告 / Generate Markdown report
            markdown_report = report_generator.generate_comprehensive_report()
            self.logger.info(f"Markdown报告已生成 / Markdown report generated: {markdown_report}")
            
            # 尝试生成PDF报告 / Attempt to generate PDF report
            if self.config.get('documentation', {}).get('generate_pdf', False):
                pdf_report = self._generate_pdf_report(markdown_report)
                if pdf_report:
                    self.logger.info(f"PDF报告已生成 / PDF report generated: {pdf_report}")
            
            self.memory_monitor.take_snapshot("报告生成完成 / Report Generation Complete")
            
            # Phase 4: 生成最终摘要 / Generate final summary
            self._generate_final_summary()
            
            self.logger.info("="*60)
            self.logger.info("✅ 综合实验系统执行完成！")
            self.logger.info("✅ Comprehensive Experiment System Execution Complete!")
            self.logger.info(f"📁 结果目录 / Results Directory: {self.base_output_dir}")
            self.logger.info("="*60)
            
            return True
            
        except Exception as e:
            self.logger.error(f"实验系统执行失败 / Experiment system execution failed: {e}")
            return False
    
    def _perform_comprehensive_analysis(self, analyzer: ResultAnalyzer, step_results: Dict[str, Any]):
        """执行综合分析 / Perform comprehensive analysis"""
        
        # 标定精度分析 / Calibration accuracy analysis
        if 'step3_mono_calibration' in step_results:
            calibration_analysis = analyzer.analyze_calibration_accuracy(step_results['step3_mono_calibration'])
            self._save_analysis_result('calibration_accuracy', calibration_analysis)
        
        # 重建质量分析 / Reconstruction quality analysis
        if 'step5_3d_reconstruction' in step_results:
            reconstruction_analysis = analyzer.analyze_reconstruction_quality(step_results['step5_3d_reconstruction'])
            self._save_analysis_result('reconstruction_quality', reconstruction_analysis)
        
        # 性能分析 / Performance analysis
        performance_analysis = analyzer.analyze_processing_performance(step_results)
        self._save_analysis_result('processing_performance', performance_analysis)
        
        # 生成性能基准 / Generate performance benchmark
        benchmark = analyzer.generate_performance_benchmark()
        self._save_analysis_result('performance_benchmark', benchmark)
        
        # 文件分析 / File analysis
        file_analyzer = FileAnalyzer(self.base_output_dir)
        file_analysis = file_analyzer.analyze_output_files()
        self._save_analysis_result('file_analysis', file_analysis)
        
        # 质量评估 / Quality assessment
        quality_assessor = QualityAssessment(self.config)
        quality_assessment = quality_assessor.assess_overall_quality(step_results)
        self._save_analysis_result('quality_assessment', quality_assessment)
        
        self.logger.info("综合分析完成 / Comprehensive analysis completed")
    
    def _save_analysis_result(self, analysis_type: str, result: Dict[str, Any]):
        """保存分析结果 / Save analysis result"""
        analysis_dir = self.base_output_dir / 'analysis'
        analysis_dir.mkdir(exist_ok=True)
        
        result_file = analysis_dir / f'{analysis_type}.json'
        with open(result_file, 'w', encoding='utf-8') as f:
            json.dump(result, f, indent=2, ensure_ascii=False)
            
        self.logger.debug(f"Analysis result saved: {result_file}")
    
    def _generate_pdf_report(self, markdown_file: str) -> Optional[str]:
        """生成PDF报告 / Generate PDF report"""
        try:
            # 尝试使用pandoc转换为PDF / Try using pandoc to convert to PDF
            pdf_file = str(Path(markdown_file).with_suffix('.pdf'))
            
            # 检查pandoc是否可用 / Check if pandoc is available
            result = subprocess.run(['which', 'pandoc'], capture_output=True, text=True)
            if result.returncode != 0:
                self.logger.warning("pandoc not available, skipping PDF generation")
                return None
            
            # 转换为PDF / Convert to PDF
            cmd = [
                'pandoc',
                markdown_file,
                '-o', pdf_file,
                '--pdf-engine=xelatex',
                '-V', 'geometry:margin=1in',
                '-V', 'fontsize=12pt'
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
            
            if result.returncode == 0:
                return pdf_file
            else:
                self.logger.warning(f"PDF generation failed: {result.stderr}")
                return None
                
        except Exception as e:
            self.logger.warning(f"PDF generation error: {e}")
            return None
    
    def _generate_final_summary(self):
        """生成最终实验摘要 / Generate final experiment summary"""
        
        # 收集所有分析结果 / Collect all analysis results
        analysis_dir = self.base_output_dir / 'analysis'
        summary_data = {
            'experiment_info': {
                'start_time': self.start_time.isoformat(),
                'end_time': datetime.now().isoformat(),
                'duration': (datetime.now() - self.start_time).total_seconds(),
                'output_directory': str(self.base_output_dir),
                'config_file': self.config_path
            },
            'memory_usage': self.memory_monitor.get_memory_report(),
            'analysis_summary': {}
        }
        
        # 加载各分析结果 / Load analysis results
        if analysis_dir.exists():
            for analysis_file in analysis_dir.glob('*.json'):
                try:
                    with open(analysis_file, 'r', encoding='utf-8') as f:
                        analysis_data = json.load(f)
                        summary_data['analysis_summary'][analysis_file.stem] = analysis_data
                except Exception as e:
                    self.logger.warning(f"Could not load analysis file {analysis_file}: {e}")
        
        # 保存最终摘要 / Save final summary
        summary_file = self.base_output_dir / 'final_experiment_summary.json'
        with open(summary_file, 'w', encoding='utf-8') as f:
            json.dump(summary_data, f, indent=2, ensure_ascii=False)
        
        # 生成简化的状态报告 / Generate simplified status report
        self._generate_status_report(summary_data)
        
        self.logger.info(f"最终实验摘要已保存 / Final experiment summary saved: {summary_file}")
    
    def _generate_status_report(self, summary_data: Dict[str, Any]):
        """生成状态报告 / Generate status report"""
        
        status_file = self.base_output_dir / 'experiment_status.txt'
        
        duration = summary_data['experiment_info']['duration']
        memory_report = summary_data.get('memory_usage', {})
        
        status_content = f"""2D到3D转换流程自动化实验 - 状态报告
Automated 2D to 3D Conversion Experiment - Status Report

实验开始时间 / Start Time: {summary_data['experiment_info']['start_time']}
实验结束时间 / End Time: {summary_data['experiment_info']['end_time']}
总耗时 / Total Duration: {duration:.2f} 秒

输出目录 / Output Directory: {summary_data['experiment_info']['output_directory']}

内存使用 / Memory Usage:
- 峰值使用 / Peak Usage: {memory_report.get('peak_usage', {}).get('used_gb', 'N/A')} GB
- 平均使用 / Average Usage: {memory_report.get('average_usage', 'N/A')} GB

生成的文件 / Generated Files:
- 综合实验报告 / Comprehensive Report: comprehensive_experiment_report.md
- 处理摘要 / Processing Summary: processing_summary.json
- 最终摘要 / Final Summary: final_experiment_summary.json
- 详细日志 / Detailed Logs: logs/comprehensive_experiment.log

实验状态 / Experiment Status: 已完成 / COMPLETED

查看综合报告以获取详细的实验结果和分析。
View the comprehensive report for detailed experiment results and analysis.
"""
        
        with open(status_file, 'w', encoding='utf-8') as f:
            f.write(status_content)
        
        # 在控制台显示状态 / Display status in console
        print("\n" + "="*60)
        print("实验完成状态 / Experiment Completion Status")
        print("="*60)
        print(f"📁 输出目录 / Output Directory: {self.base_output_dir}")
        print(f"⏱️  总耗时 / Total Duration: {duration:.2f} 秒")
        print(f"📊 状态报告 / Status Report: {status_file}")
        print(f"📋 综合报告 / Comprehensive Report: {self.base_output_dir}/comprehensive_experiment_report.md")
        print("="*60)

def create_sample_input_structure():
    """创建示例输入结构 / Create sample input structure"""
    
    print("创建示例输入目录结构 / Creating sample input directory structure...")
    
    # 创建输入目录 / Create input directories
    input_dirs = [
        'input/left',
        'input/right',
        'input/scene/left', 
        'input/scene/right'
    ]
    
    for dir_path in input_dirs:
        Path(dir_path).mkdir(parents=True, exist_ok=True)
        
        # 创建README文件说明如何使用 / Create README file explaining usage
        readme_content = f"""# {dir_path} 目录说明

请将相应的图像文件放入此目录：

## 如果这是标定图像目录 (left/right):
- 放入包含9×6棋盘格的标定图像
- 图像应该包含清晰的棋盘格角点
- 建议使用多个角度和位置的图像
- 支持的格式：JPG, PNG, BMP, TIFF

## 如果这是场景图像目录 (scene/left, scene/right):
- 放入需要进行3D重建的立体图像对
- 确保左右图像是同步拍摄的
- 图像应该有重叠区域用于立体匹配

## 图像要求:
- 建议分辨率：至少640×480，最佳3264×2448
- 图像质量：清晰，无运动模糊
- 光照条件：均匀，避免强烈阴影

放入图像后，运行实验脚本开始自动化处理。
"""
        
        readme_file = Path(dir_path) / 'README.md'
        with open(readme_file, 'w', encoding='utf-8') as f:
            f.write(readme_content)
    
    print("✅ 示例目录结构创建完成 / Sample directory structure created")
    print("请将标定和场景图像放入相应目录后运行实验")
    print("Please place calibration and scene images in respective directories before running experiment")

def main():
    """主函数 / Main function"""
    
    parser = argparse.ArgumentParser(
        description='2D到3D转换流程自动化实验文档生成器 / Automated 2D to 3D Conversion Experiment Documentation Generator',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例用法 / Example Usage:

  # 运行完整实验 / Run complete experiment
  python3 comprehensive_experiment_report.py

  # 使用自定义配置 / Use custom configuration
  python3 comprehensive_experiment_report.py --config my_config.yaml

  # 指定输出目录 / Specify output directory
  python3 comprehensive_experiment_report.py --output my_experiment_results

  # 仅创建输入目录结构 / Only create input directory structure
  python3 comprehensive_experiment_report.py --setup-only

详细文档请参考生成的实验报告。
For detailed documentation, refer to the generated experiment report.
        """
    )
    
    parser.add_argument(
        '--config', 
        default='config.yaml',
        help='配置文件路径 / Configuration file path (default: config.yaml)'
    )
    
    parser.add_argument(
        '--output',
        help='输出目录路径 / Output directory path (default: from config with timestamp)'
    )
    
    parser.add_argument(
        '--setup-only',
        action='store_true',
        help='仅创建输入目录结构，不运行实验 / Only create input directory structure, do not run experiment'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='显示详细输出 / Show verbose output'
    )
    
    args = parser.parse_args()
    
    # 显示系统信息 / Display system information
    print("2D到3D转换流程自动化实验文档生成器")
    print("Automated 2D to 3D Conversion Experiment Documentation Generator")
    print("="*60)
    print(f"Python版本 / Python Version: {sys.version}")
    print(f"当前目录 / Current Directory: {Path.cwd()}")
    print(f"配置文件 / Config File: {args.config}")
    print("="*60)
    
    try:
        # 仅设置模式 / Setup-only mode
        if args.setup_only:
            create_sample_input_structure()
            return
        
        # 检查配置文件是否存在 / Check if config file exists
        if not Path(args.config).exists():
            print(f"⚠️  配置文件不存在，使用默认配置 / Config file not found, using default configuration")
        
        # 创建并运行综合实验系统 / Create and run comprehensive experiment system
        experiment_system = ComprehensiveExperimentSystem(
            config_path=args.config,
            output_dir=args.output
        )
        
        success = experiment_system.run_complete_experiment()
        
        if success:
            print("\n🎉 实验成功完成！")
            print("🎉 Experiment completed successfully!")
            print(f"\n📖 查看详细报告 / View detailed report:")
            print(f"   {experiment_system.base_output_dir}/comprehensive_experiment_report.md")
        else:
            print("\n❌ 实验执行失败")
            print("❌ Experiment execution failed")
            sys.exit(1)
            
    except KeyboardInterrupt:
        print("\n\n⏹️  实验被用户中断 / Experiment interrupted by user")
        sys.exit(130)
    except Exception as e:
        print(f"\n💥 意外错误 / Unexpected error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()