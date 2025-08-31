#include "image_resize.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "corner_detection.h"
#include "model_viewer.h"

#include <iostream>
#include <fstream>
#include <filesystem>

/**
 * 相机标定和3D重建流程 - 使用8.2mm棋盘格边长
 * Camera Calibration and 3D Reconstruction Pipeline - Using 8.2mm chessboard squares
 * 
 * 按照要求的5个步骤顺序执行：
 * Execute following 5 steps in order as required:
 * 1. 图像预处理 - 裁剪为3264×2448像素 (Image preprocessing - crop to 3264×2448 pixels)
 * 2. 角点检测并可视化 (Corner detection and visualization)
 * 3. 左右相机单目标定 (Monocular calibration for left/right cameras)
 * 4. 双目标定和立体校正 (Stereo calibration and rectification)
 * 5. 基于立体匹配的3D点云重建 (3D point cloud reconstruction based on stereo matching)
 */

int main() {
    std::cout << "=== 相机标定与3D重建流程 (8.2mm棋盘格) ===" << std::endl;
    std::cout << "=== Camera Calibration & 3D Reconstruction Pipeline (8.2mm Chessboard) ===" << std::endl;
    std::cout << std::endl;
    
    // 创建目录结构 (Create directory structure)
    std::filesystem::create_directories("calibration_data/input/left");
    std::filesystem::create_directories("calibration_data/input/right");
    std::filesystem::create_directories("calibration_data/output");
    
    // 配置参数 - 使用8.2mm棋盘格 (Configuration - using 8.2mm chessboard)
    const int boardWidth = 9;
    const int boardHeight = 6;
    const float squareSize = 0.0082f;  // 8.2mm = 0.0082m
    const int imageWidth = 3264;   // 要求的图像宽度 (Required image width)
    const int imageHeight = 2448;  // 要求的图像高度 (Required image height)
    
    std::cout << "配置参数 (Configuration):" << std::endl;
    std::cout << "- 棋盘格内角点 (Chessboard inner corners): " << boardWidth << "x" << boardHeight << std::endl;
    std::cout << "- 方格边长 (Square size): " << squareSize * 1000 << "mm" << std::endl;
    std::cout << "- 图像尺寸 (Image size): " << imageWidth << "x" << imageHeight << " 像素" << std::endl;
    std::cout << std::endl;

    // 第1步：图像预处理 - 裁剪为3264×2448像素
    std::cout << "第1步：图像预处理 - 裁剪为3264×2448像素" << std::endl;
    std::cout << "Step 1: Image Preprocessing - Crop to 3264×2448 pixels" << std::endl;
    
    bool success1 = ImageUtils::resizeImage(
        "calibration_data/input/left",
        "calibration_data/output/step1_left_resized",
        imageWidth, imageHeight, ImageUtils::LINEAR
    );

    bool success2 = ImageUtils::resizeImage(
        "calibration_data/input/right",
        "calibration_data/output/step1_right_resized", 
        imageWidth, imageHeight, ImageUtils::LINEAR
    );

    if (success1 && success2) {
        std::cout << "✓ 第1步完成：图像预处理成功 (Step 1 completed: Image preprocessing successful)" << std::endl;
    } else {
        std::cout << "○ 第1步：未找到输入图像，跳过处理 (Step 1: No input images found, skipping)" << std::endl;
        std::cout << "  请在 calibration_data/input/left 和 right 中放置棋盘格图像" << std::endl;
    }
    std::cout << std::endl;

    // 第2步：角点检测与可视化
    std::cout << "第2步：角点检测与可视化" << std::endl;
    std::cout << "Step 2: Corner Detection and Visualization" << std::endl;
    
    bool success3 = detectAndDrawCorners(
        "calibration_data/output/step1_left_resized",
        "calibration_data/output/step2_left_corners",
        boardWidth, boardHeight, 1.0f
    );

    bool success4 = detectAndDrawCorners(
        "calibration_data/output/step1_right_resized",
        "calibration_data/output/step2_right_corners",
        boardWidth, boardHeight, 1.0f
    );

    if (success3 && success4) {
        std::cout << "✓ 第2步完成：角点检测与可视化成功 (Step 2 completed: Corner detection and visualization successful)" << std::endl;
    } else {
        std::cout << "○ 第2步：角点检测跳过 (Step 2: Corner detection skipped - no valid images)" << std::endl;
    }
    std::cout << std::endl;

    // 第3步：单目标定（左右相机分别标定）
    std::cout << "第3步：单目标定（左右相机分别标定）" << std::endl;
    std::cout << "Step 3: Monocular Calibration (Left and Right Cameras Separately)" << std::endl;
    
    bool success5 = MonoCalibration::calibrateCamera(
        "calibration_data/output/step2_left_corners",
        "calibration_data/output/step1_left_resized",
        "calibration_data/output/step3_left_calibration",
        boardWidth, boardHeight, squareSize,
        imageWidth, imageHeight,
        true,  // 保存矫正图像 (Save corrected images)
        "calibration_data/output/step3_left_corrected"
    );

    bool success6 = MonoCalibration::calibrateCamera(
        "calibration_data/output/step2_right_corners",
        "calibration_data/output/step1_right_resized",
        "calibration_data/output/step3_right_calibration",
        boardWidth, boardHeight, squareSize,
        imageWidth, imageHeight,
        true,  // 保存矫正图像 (Save corrected images) 
        "calibration_data/output/step3_right_corrected"
    );

    if (success5 && success6) {
        std::cout << "✓ 第3步完成：单目标定成功 (Step 3 completed: Monocular calibration successful)" << std::endl;
        std::cout << "  ✓ 已生成矫正图 (Generated corrected images)" << std::endl;
        std::cout << "  ✓ 已生成残差图 (Generated residual images)" << std::endl;
    } else {
        std::cout << "○ 第3步：单目标定跳过 (Step 3: Monocular calibration skipped)" << std::endl;
    }
    std::cout << std::endl;

    // 第4步：双目标定和立体校正
    std::cout << "第4步：双目标定和立体校正" << std::endl;
    std::cout << "Step 4: Stereo Calibration and Rectification" << std::endl;
    
    bool success7 = StereoCalibration::calibrateStereoCamera(
        "calibration_data/output/step2_left_corners",
        "calibration_data/output/step2_right_corners",
        "calibration_data/output/step4_stereo_calibration",
        boardWidth, boardHeight, squareSize,
        imageWidth, imageHeight
    );

    if (success7) {
        std::cout << "✓ 第4步完成：双目标定和立体校正成功 (Step 4 completed: Stereo calibration and rectification successful)" << std::endl;
    } else {
        std::cout << "○ 第4步：双目标定跳过 (Step 4: Stereo calibration skipped)" << std::endl;
    }
    std::cout << std::endl;

    // 第5步：三维重建（基于立体匹配的3D点云重建）
    std::cout << "第5步：三维重建（基于立体匹配的3D点云重建）" << std::endl;
    std::cout << "Step 5: 3D Reconstruction (3D Point Cloud Reconstruction Based on Stereo Matching)" << std::endl;
    
    // 检查是否有场景图像用于重建 (Check if scene images exist for reconstruction)
    if (std::filesystem::exists("calibration_data/input/scene_left.jpg") &&
        std::filesystem::exists("calibration_data/input/scene_right.jpg")) {
        
        bool success8 = StereoReconstruction::reconstruct3D(
            "calibration_data/input/scene_left.jpg",
            "calibration_data/input/scene_right.jpg",
            "calibration_data/output/step4_stereo_calibration",
            "calibration_data/output/step5_3d_reconstruction",
            StereoReconstruction::PLY_FORMAT,  // PLY格式
            StereoReconstruction::MEDIUM_QUALITY,  // 中等质量
            squareSize,  // 8.2mm棋盘格尺寸
            imageWidth, imageHeight
        );

        if (success8) {
            std::cout << "✓ 第5步完成：3D重建成功 (Step 5 completed: 3D reconstruction successful)" << std::endl;
        } else {
            std::cout << "✗ 第5步：3D重建失败 (Step 5: 3D reconstruction failed)" << std::endl;
        }
    } else {
        std::cout << "○ 第5步：未找到场景图像，跳过3D重建 (Step 5: No scene images found, skipping 3D reconstruction)" << std::endl;
        std::cout << "  请在 calibration_data/input/ 中放置 scene_left.jpg 和 scene_right.jpg" << std::endl;
    }
    std::cout << std::endl;

    // 完成总结 (Completion summary)
    std::cout << "=== 流程完成 (Pipeline Complete) ===" << std::endl;
    std::cout << "生成的文件结构 (Generated file structure):" << std::endl;
    std::cout << "calibration_data/output/" << std::endl;
    std::cout << "├── step1_left_resized/     # 第1步：左图像预处理结果 (3264×2448)" << std::endl;
    std::cout << "├── step1_right_resized/    # 第1步：右图像预处理结果 (3264×2448)" << std::endl;
    std::cout << "├── step2_left_corners/     # 第2步：左图像角点检测可视化" << std::endl;
    std::cout << "├── step2_right_corners/    # 第2步：右图像角点检测可视化" << std::endl;
    std::cout << "├── step3_left_calibration/ # 第3步：左相机单目标定参数和残差图" << std::endl;
    std::cout << "├── step3_right_calibration/# 第3步：右相机单目标定参数和残差图" << std::endl;
    std::cout << "├── step3_left_corrected/   # 第3步：左相机矫正图" << std::endl;
    std::cout << "├── step3_right_corrected/  # 第3步：右相机矫正图" << std::endl;
    std::cout << "├── step4_stereo_calibration/# 第4步：双目标定和立体校正参数" << std::endl;
    std::cout << "└── step5_3d_reconstruction/# 第5步：3D点云重建结果" << std::endl;
    std::cout << std::endl;
    
    std::cout << "期望的输入目录结构 (Expected input directory structure):" << std::endl;
    std::cout << "calibration_data/input/" << std::endl;
    std::cout << "├── left/           # 左相机棋盘格标定图像 (8.2mm方格)" << std::endl;
    std::cout << "├── right/          # 右相机棋盘格标定图像 (8.2mm方格)" << std::endl;
    std::cout << "├── scene_left.jpg  # 左场景图像 (用于3D重建)" << std::endl;
    std::cout << "└── scene_right.jpg # 右场景图像 (用于3D重建)" << std::endl;

    return 0;
}