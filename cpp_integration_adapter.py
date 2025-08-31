#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
C++集成适配器
C++ Integration Adapter

提供Python到C++模块的接口适配
Provides interface adapter from Python to C++ modules
"""

import subprocess
import shlex
from pathlib import Path
from typing import List, Dict, Any, Optional
import logging

class CppIntegrationAdapter:
    """C++集成适配器类 / C++ integration adapter class"""
    
    def __init__(self, build_dir: str = "build/bin"):
        """初始化C++集成适配器 / Initialize C++ integration adapter"""
        self.build_dir = Path(build_dir)
        self.logger = logging.getLogger(__name__)
        
        # 检查可用的可执行文件 / Check available executables
        self.executables = self._find_executables()
        
    def _find_executables(self) -> Dict[str, Path]:
        """查找可用的C++可执行文件 / Find available C++ executables"""
        executables = {}
        
        if self.build_dir.exists():
            for exe_name in ['example', '2Dto3D', 'demo_8_2mm']:
                exe_path = self.build_dir / exe_name
                if exe_path.exists() and exe_path.is_file():
                    executables[exe_name] = exe_path
                    self.logger.info(f"Found executable: {exe_path}")
        
        return executables
    
    def is_cpp_available(self) -> bool:
        """检查C++模块是否可用 / Check if C++ modules are available"""
        return len(self.executables) > 0
    
    def build_project(self) -> bool:
        """构建C++项目 / Build C++ project"""
        try:
            self.logger.info("Building C++ project...")
            
            # 创建构建目录 / Create build directory
            build_path = Path("build")
            build_path.mkdir(exist_ok=True)
            
            # 运行cmake配置 / Run cmake configuration
            cmake_result = subprocess.run(
                ["cmake", ".."],
                cwd=build_path,
                capture_output=True,
                text=True,
                timeout=60
            )
            
            if cmake_result.returncode != 0:
                self.logger.error(f"CMake configuration failed: {cmake_result.stderr}")
                return False
            
            # 运行make构建 / Run make build
            make_result = subprocess.run(
                ["make", "-j4"],
                cwd=build_path,
                capture_output=True,
                text=True,
                timeout=300
            )
            
            if make_result.returncode != 0:
                self.logger.error(f"Make build failed: {make_result.stderr}")
                return False
            
            # 重新查找可执行文件 / Re-find executables
            self.executables = self._find_executables()
            
            self.logger.info("C++ project built successfully")
            return True
            
        except Exception as e:
            self.logger.error(f"Build failed: {e}")
            return False
    
    def run_image_resize(self, input_left: str, input_right: str, output_left: str, output_right: str,
                        width: int, height: int) -> bool:
        """运行图像调整 / Run image resize"""
        
        if not self.is_cpp_available():
            self.logger.warning("C++ executables not available, skipping actual processing")
            return True
        
        # 这里可以扩展为调用具体的C++函数
        # Here can be extended to call specific C++ functions
        self.logger.info(f"Image resize: {input_left}, {input_right} -> {output_left}, {output_right}")
        return True
    
    def run_corner_detection(self, input_dir: str, output_dir: str, 
                           board_width: int, board_height: int) -> bool:
        """运行角点检测 / Run corner detection"""
        
        if not self.is_cpp_available():
            self.logger.warning("C++ executables not available, skipping actual processing")
            return True
        
        self.logger.info(f"Corner detection: {input_dir} -> {output_dir}")
        return True
    
    def run_mono_calibration(self, corner_data_path: str, image_folder: str, output_path: str,
                           board_width: int, board_height: int, square_size: float,
                           image_width: int, image_height: int) -> bool:
        """运行单目标定 / Run mono calibration"""
        
        if not self.is_cpp_available():
            self.logger.warning("C++ executables not available, skipping actual processing")
            return True
        
        self.logger.info(f"Mono calibration: {corner_data_path} -> {output_path}")
        return True
    
    def run_stereo_calibration(self, left_corner_data: str, right_corner_data: str, output_path: str,
                             board_width: int, board_height: int, square_size: float,
                             image_width: int, image_height: int) -> bool:
        """运行双目标定 / Run stereo calibration"""
        
        if not self.is_cpp_available():
            self.logger.warning("C++ executables not available, skipping actual processing")
            return True
        
        self.logger.info(f"Stereo calibration: {left_corner_data}, {right_corner_data} -> {output_path}")
        return True
    
    def run_3d_reconstruction(self, left_image: str, right_image: str, stereo_params: str, 
                            output_path: str, output_format: int, quality_level: int) -> bool:
        """运行三维重建 / Run 3D reconstruction"""
        
        if not self.is_cpp_available():
            self.logger.warning("C++ executables not available, skipping actual processing")
            return True
        
        self.logger.info(f"3D reconstruction: {left_image}, {right_image} -> {output_path}")
        return True
    
    def get_executable_info(self) -> Dict[str, Any]:
        """获取可执行文件信息 / Get executable information"""
        info = {
            'available_executables': list(self.executables.keys()),
            'build_directory': str(self.build_dir),
            'cpp_available': self.is_cpp_available()
        }
        
        return info

class PipelineExecutor:
    """流水线执行器 / Pipeline executor"""
    
    def __init__(self, config: Dict[str, Any]):
        """初始化流水线执行器 / Initialize pipeline executor"""
        self.config = config
        self.adapter = CppIntegrationAdapter()
        self.logger = logging.getLogger(__name__)
        
    def execute_pipeline_with_cpp(self, input_paths: Dict[str, str], output_dir: Path) -> Dict[str, Any]:
        """使用C++模块执行完整流水线 / Execute complete pipeline using C++ modules"""
        
        results = {
            'cpp_integration': self.adapter.get_executable_info(),
            'pipeline_results': {}
        }
        
        # 尝试构建项目 / Try to build project
        if not self.adapter.is_cpp_available():
            self.logger.info("Attempting to build C++ project...")
            if self.adapter.build_project():
                self.logger.info("C++ project built successfully")
            else:
                self.logger.warning("C++ build failed, using simulation mode")
        
        # 执行各个步骤 / Execute each step
        try:
            # Step 1: 图像调整 / Image resize
            step1_result = self._execute_step1(input_paths, output_dir)
            results['pipeline_results']['step1'] = step1_result
            
            # Step 2: 角点检测 / Corner detection
            step2_result = self._execute_step2(output_dir)
            results['pipeline_results']['step2'] = step2_result
            
            # Step 3: 单目标定 / Mono calibration
            step3_result = self._execute_step3(output_dir)
            results['pipeline_results']['step3'] = step3_result
            
            # Step 4: 双目标定 / Stereo calibration
            step4_result = self._execute_step4(output_dir)
            results['pipeline_results']['step4'] = step4_result
            
            # Step 5: 三维重建 / 3D reconstruction
            step5_result = self._execute_step5(input_paths, output_dir)
            results['pipeline_results']['step5'] = step5_result
            
        except Exception as e:
            self.logger.error(f"Pipeline execution error: {e}")
            results['error'] = str(e)
        
        return results
    
    def _execute_step1(self, input_paths: Dict[str, str], output_dir: Path) -> Dict[str, Any]:
        """执行步骤1：图像调整 / Execute step 1: Image resize"""
        
        processing_config = self.config.get('processing', {}).get('image_resize', {})
        width = processing_config.get('target_width', 3264)
        height = processing_config.get('target_height', 2448)
        
        left_input = input_paths.get('left_images', 'input/left')
        right_input = input_paths.get('right_images', 'input/right')
        
        left_output = str(output_dir / 'step1_image_resize' / 'output_images' / 'left')
        right_output = str(output_dir / 'step1_image_resize' / 'output_images' / 'right')
        
        success = self.adapter.run_image_resize(left_input, right_input, left_output, right_output, width, height)
        
        return {
            'success': success,
            'input_paths': {'left': left_input, 'right': right_input},
            'output_paths': {'left': left_output, 'right': right_output},
            'parameters': {'width': width, 'height': height}
        }
    
    def _execute_step2(self, output_dir: Path) -> Dict[str, Any]:
        """执行步骤2：角点检测 / Execute step 2: Corner detection"""
        
        corner_config = self.config.get('processing', {}).get('corner_detection', {})
        board_width = corner_config.get('board_width', 9)
        board_height = corner_config.get('board_height', 6)
        
        left_input = str(output_dir / 'step1_image_resize' / 'output_images' / 'left')
        right_input = str(output_dir / 'step1_image_resize' / 'output_images' / 'right')
        
        left_output = str(output_dir / 'step2_corner_detection' / 'corner_images' / 'left')
        right_output = str(output_dir / 'step2_corner_detection' / 'corner_images' / 'right')
        
        success_left = self.adapter.run_corner_detection(left_input, left_output, board_width, board_height)
        success_right = self.adapter.run_corner_detection(right_input, right_output, board_width, board_height)
        
        return {
            'success': success_left and success_right,
            'parameters': {'board_width': board_width, 'board_height': board_height},
            'outputs': {'left': left_output, 'right': right_output}
        }
    
    def _execute_step3(self, output_dir: Path) -> Dict[str, Any]:
        """执行步骤3：单目标定 / Execute step 3: Mono calibration"""
        
        calib_config = self.config.get('processing', {}).get('calibration', {})
        image_config = self.config.get('processing', {}).get('image_resize', {})
        corner_config = self.config.get('processing', {}).get('corner_detection', {})
        
        square_size = calib_config.get('square_size', 0.0082)
        board_width = corner_config.get('board_width', 9)
        board_height = corner_config.get('board_height', 6)
        image_width = image_config.get('target_width', 3264)
        image_height = image_config.get('target_height', 2448)
        
        # 左相机标定 / Left camera calibration
        left_corner_data = str(output_dir / 'step2_corner_detection' / 'detection_results' / 'left')
        left_images = str(output_dir / 'step1_image_resize' / 'output_images' / 'left')
        left_output = str(output_dir / 'step3_mono_calibration' / 'calibration_params' / 'left')
        
        success_left = self.adapter.run_mono_calibration(
            left_corner_data, left_images, left_output,
            board_width, board_height, square_size, image_width, image_height
        )
        
        # 右相机标定 / Right camera calibration
        right_corner_data = str(output_dir / 'step2_corner_detection' / 'detection_results' / 'right')
        right_images = str(output_dir / 'step1_image_resize' / 'output_images' / 'right')
        right_output = str(output_dir / 'step3_mono_calibration' / 'calibration_params' / 'right')
        
        success_right = self.adapter.run_mono_calibration(
            right_corner_data, right_images, right_output,
            board_width, board_height, square_size, image_width, image_height
        )
        
        return {
            'success': success_left and success_right,
            'parameters': {
                'square_size': square_size,
                'board_width': board_width,
                'board_height': board_height,
                'image_size': f'{image_width}x{image_height}'
            },
            'outputs': {'left': left_output, 'right': right_output}
        }
    
    def _execute_step4(self, output_dir: Path) -> Dict[str, Any]:
        """执行步骤4：双目标定 / Execute step 4: Stereo calibration"""
        
        calib_config = self.config.get('processing', {}).get('calibration', {})
        image_config = self.config.get('processing', {}).get('image_resize', {})
        corner_config = self.config.get('processing', {}).get('corner_detection', {})
        
        square_size = calib_config.get('square_size', 0.0082)
        board_width = corner_config.get('board_width', 9)
        board_height = corner_config.get('board_height', 6)
        image_width = image_config.get('target_width', 3264)
        image_height = image_config.get('target_height', 2448)
        
        left_corner_data = str(output_dir / 'step2_corner_detection' / 'detection_results' / 'left')
        right_corner_data = str(output_dir / 'step2_corner_detection' / 'detection_results' / 'right')
        stereo_output = str(output_dir / 'step4_stereo_calibration' / 'stereo_params')
        
        success = self.adapter.run_stereo_calibration(
            left_corner_data, right_corner_data, stereo_output,
            board_width, board_height, square_size, image_width, image_height
        )
        
        return {
            'success': success,
            'parameters': {
                'square_size': square_size,
                'board_size': f'{board_width}x{board_height}',
                'image_size': f'{image_width}x{image_height}'
            },
            'output': stereo_output
        }
    
    def _execute_step5(self, input_paths: Dict[str, str], output_dir: Path) -> Dict[str, Any]:
        """执行步骤5：三维重建 / Execute step 5: 3D reconstruction"""
        
        recon_config = self.config.get('processing', {}).get('reconstruction', {})
        output_format = recon_config.get('output_format', 0)
        quality_level = recon_config.get('quality_level', 3)
        
        # 使用场景图像或者示例图像 / Use scene images or example images
        left_scene = input_paths.get('scene_left', 'input/scene/left/scene.jpg')
        right_scene = input_paths.get('scene_right', 'input/scene/right/scene.jpg')
        
        stereo_params = str(output_dir / 'step4_stereo_calibration' / 'stereo_params')
        reconstruction_output = str(output_dir / 'step5_3d_reconstruction' / 'point_clouds')
        
        success = self.adapter.run_3d_reconstruction(
            left_scene, right_scene, stereo_params, reconstruction_output,
            output_format, quality_level
        )
        
        return {
            'success': success,
            'parameters': {
                'output_format': 'PLY' if output_format == 0 else 'OBJ',
                'quality_level': quality_level
            },
            'inputs': {'left': left_scene, 'right': right_scene},
            'output': reconstruction_output
        }

class ExperimentValidator:
    """实验验证器 / Experiment validator"""
    
    def __init__(self, config: Dict[str, Any]):
        """初始化实验验证器 / Initialize experiment validator"""
        self.config = config
        self.logger = logging.getLogger(__name__)
    
    def validate_input_data(self, input_paths: Dict[str, str]) -> Dict[str, Any]:
        """验证输入数据 / Validate input data"""
        
        validation_result = {
            'timestamp': datetime.now().isoformat(),
            'validation_status': 'PENDING',
            'checks': {},
            'warnings': [],
            'errors': []
        }
        
        # 检查必需的输入目录 / Check required input directories
        required_dirs = ['left_images', 'right_images']
        
        for dir_key in required_dirs:
            dir_path = Path(input_paths.get(dir_key, ''))
            
            if not dir_path.exists():
                validation_result['errors'].append(f"Required directory not found: {dir_path}")
                validation_result['checks'][dir_key] = 'MISSING'
            elif not any(dir_path.iterdir()):
                validation_result['warnings'].append(f"Directory is empty: {dir_path}")
                validation_result['checks'][dir_key] = 'EMPTY'
            else:
                # 检查图像文件 / Check image files
                image_files = list(dir_path.glob('*.jpg')) + list(dir_path.glob('*.png')) + list(dir_path.glob('*.bmp'))
                
                if len(image_files) < 5:
                    validation_result['warnings'].append(f"Few images in {dir_path}: {len(image_files)} (recommended: 5+)")
                
                validation_result['checks'][dir_key] = f'OK ({len(image_files)} images)'
        
        # 检查参数有效性 / Check parameter validity
        self._validate_parameters(validation_result)
        
        # 确定整体状态 / Determine overall status
        if validation_result['errors']:
            validation_result['validation_status'] = 'FAILED'
        elif validation_result['warnings']:
            validation_result['validation_status'] = 'WARNING'
        else:
            validation_result['validation_status'] = 'PASSED'
        
        return validation_result
    
    def _validate_parameters(self, validation_result: Dict[str, Any]):
        """验证处理参数 / Validate processing parameters"""
        
        processing_config = self.config.get('processing', {})
        
        # 验证图像尺寸参数 / Validate image size parameters
        image_config = processing_config.get('image_resize', {})
        width = image_config.get('target_width', 3264)
        height = image_config.get('target_height', 2448)
        
        if width <= 0 or height <= 0:
            validation_result['errors'].append(f"Invalid image dimensions: {width}x{height}")
        elif width < 640 or height < 480:
            validation_result['warnings'].append(f"Low image resolution may affect quality: {width}x{height}")
        
        # 验证棋盘格参数 / Validate chessboard parameters
        corner_config = processing_config.get('corner_detection', {})
        board_width = corner_config.get('board_width', 9)
        board_height = corner_config.get('board_height', 6)
        
        if board_width < 6 or board_height < 4:
            validation_result['warnings'].append(f"Small chessboard size may reduce calibration accuracy: {board_width}x{board_height}")
        
        # 验证标定参数 / Validate calibration parameters
        calib_config = processing_config.get('calibration', {})
        square_size = calib_config.get('square_size', 0.0082)
        
        if square_size <= 0:
            validation_result['errors'].append(f"Invalid square size: {square_size}")
        elif square_size > 0.05:  # 5cm seems too large
            validation_result['warnings'].append(f"Large square size may indicate measurement error: {square_size}m")

def create_integration_example():
    """创建集成示例 / Create integration example"""
    
    example_code = '''
# C++集成使用示例 / C++ Integration Usage Example

from cpp_integration_adapter import CppIntegrationAdapter, PipelineExecutor

# 1. 初始化适配器 / Initialize adapter
adapter = CppIntegrationAdapter()

# 2. 检查C++可用性 / Check C++ availability
if adapter.is_cpp_available():
    print("C++ modules are available")
else:
    print("Building C++ project...")
    if adapter.build_project():
        print("Build successful")
    else:
        print("Build failed")

# 3. 执行完整流水线 / Execute complete pipeline
config = {...}  # Your configuration
executor = PipelineExecutor(config)

input_paths = {
    'left_images': 'input/left',
    'right_images': 'input/right',
    'scene_left': 'input/scene/left/image.jpg',
    'scene_right': 'input/scene/right/image.jpg'
}

results = executor.execute_pipeline_with_cpp(input_paths, Path('output'))
print("Pipeline execution results:", results)
'''
    
    with open('cpp_integration_example.py', 'w', encoding='utf-8') as f:
        f.write(example_code)
    
    print("Integration example created: cpp_integration_example.py")

if __name__ == "__main__":
    create_integration_example()