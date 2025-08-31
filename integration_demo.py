#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
集成演示脚本
Integration Demo Script

演示如何与构建好的C++可执行文件集成
Demonstrates integration with built C++ executables
"""

import subprocess
import sys
from pathlib import Path
import yaml

def check_build_status():
    """检查构建状态 / Check build status"""
    
    print("检查C++项目构建状态 / Checking C++ project build status")
    print("="*50)
    
    # 检查构建目录 / Check build directory
    build_dir = Path("build")
    bin_dir = build_dir / "bin"
    
    if not build_dir.exists():
        print("❌ 构建目录不存在 / Build directory does not exist")
        return False
    
    if not bin_dir.exists():
        print("❌ 可执行文件目录不存在 / Executable directory does not exist")
        return False
    
    # 检查可执行文件 / Check executables
    executables = ['example', '2Dto3D', 'demo_8_2mm']
    found_executables = []
    
    for exe in executables:
        exe_path = bin_dir / exe
        if exe_path.exists():
            print(f"✅ 找到可执行文件 / Found executable: {exe}")
            found_executables.append(exe)
        else:
            print(f"❌ 缺少可执行文件 / Missing executable: {exe}")
    
    return len(found_executables) > 0

def try_build_project():
    """尝试构建项目 / Try to build project"""
    
    print("\n尝试构建C++项目 / Attempting to build C++ project")
    print("="*50)
    
    try:
        # 创建构建目录 / Create build directory
        build_dir = Path("build")
        build_dir.mkdir(exist_ok=True)
        
        print("1. 配置项目 / Configuring project...")
        cmake_result = subprocess.run(
            ["cmake", ".."],
            cwd=build_dir,
            capture_output=True,
            text=True,
            timeout=60
        )
        
        if cmake_result.returncode != 0:
            print(f"❌ CMake配置失败 / CMake configuration failed")
            print(f"错误信息 / Error: {cmake_result.stderr}")
            return False
        
        print("✅ CMake配置成功 / CMake configuration successful")
        
        print("2. 编译项目 / Building project...")
        make_result = subprocess.run(
            ["make", "-j4"],
            cwd=build_dir,
            capture_output=True,
            text=True,
            timeout=300
        )
        
        if make_result.returncode != 0:
            print(f"❌ 编译失败 / Build failed")
            print(f"错误信息 / Error: {make_result.stderr}")
            return False
        
        print("✅ 编译成功 / Build successful")
        
        # 检查生成的可执行文件 / Check generated executables
        return check_build_status()
        
    except subprocess.TimeoutExpired:
        print("❌ 构建超时 / Build timeout")
        return False
    except Exception as e:
        print(f"❌ 构建错误 / Build error: {e}")
        return False

def run_integration_demo():
    """运行集成演示 / Run integration demo"""
    
    print("\n运行集成演示 / Running integration demo")
    print("="*50)
    
    # 首先检查构建状态 / First check build status
    if not check_build_status():
        print("\n尝试构建项目 / Trying to build project...")
        if not try_build_project():
            print("\n⚠️  C++项目构建失败，将使用模拟模式运行自动化系统")
            print("⚠️  C++ project build failed, will run automation system in simulation mode")
        else:
            print("\n✅ C++项目构建成功！/ C++ project built successfully!")
    
    print("\n启动自动化实验系统 / Starting automated experiment system...")
    
    # 运行自动化实验系统 / Run automated experiment system
    try:
        result = subprocess.run(
            [sys.executable, "comprehensive_experiment_report.py"],
            capture_output=True,
            text=True,
            timeout=60
        )
        
        if result.returncode == 0:
            print("✅ 自动化实验系统运行成功 / Automated experiment system ran successfully")
            print("\n输出摘要 / Output Summary:")
            print(result.stdout.split("============================================================")[-2])
        else:
            print("❌ 自动化实验系统运行失败 / Automated experiment system failed")
            print(f"错误信息 / Error: {result.stderr}")
    
    except subprocess.TimeoutExpired:
        print("⏱️  实验系统运行超时 / Experiment system timeout")
    except Exception as e:
        print(f"❌ 运行错误 / Execution error: {e}")

def show_usage_examples():
    """显示使用示例 / Show usage examples"""
    
    print("\n使用示例 / Usage Examples")
    print("="*50)
    
    examples = [
        ("基本使用 / Basic Usage", 
         "python3 comprehensive_experiment_report.py"),
        
        ("仅创建输入结构 / Setup only", 
         "python3 comprehensive_experiment_report.py --setup-only"),
        
        ("使用自定义配置 / Custom config", 
         "python3 comprehensive_experiment_report.py --config my_config.yaml"),
        
        ("指定输出目录 / Specify output", 
         "python3 comprehensive_experiment_report.py --output my_results"),
        
        ("详细输出模式 / Verbose mode", 
         "python3 comprehensive_experiment_report.py --verbose"),
        
        ("生成可视化 / Generate visualizations", 
         "python3 visualization_generator.py experiment_results_2025-08-31_01-24-14"),
        
        ("C++集成适配 / C++ integration", 
         "python3 cpp_integration_adapter.py")
    ]
    
    for description, command in examples:
        print(f"\n{description}:")
        print(f"  {command}")
    
    print(f"\n详细文档 / Detailed Documentation:")
    print(f"  📖 查看 AUTOMATION_README.md")
    print(f"  📖 View AUTOMATION_README.md")

def main():
    """主函数 / Main function"""
    
    print("2D到3D转换流程自动化实验 - 集成演示")
    print("Automated 2D to 3D Conversion Experiment - Integration Demo")
    print("="*60)
    
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == "build":
            try_build_project()
        elif command == "check":
            check_build_status()
        elif command == "demo":
            run_integration_demo()
        elif command == "examples":
            show_usage_examples()
        else:
            print(f"未知命令 / Unknown command: {command}")
            print("可用命令 / Available commands: build, check, demo, examples")
    else:
        # 默认运行完整演示 / Default run complete demo
        run_integration_demo()
        show_usage_examples()

if __name__ == "__main__":
    main()