#include "mono_calibration.h"
#include <iostream>
#include <filesystem>

/**
 * 第3步：单目标定 - 对左右相机分别进行单目标定
 * Step 3: Monocular Calibration - Calibrate left and right cameras separately
 */

int main() {
    std::cout << "=== 第3步：单目标定 (Step 3: Monocular Calibration) ===" << std::endl;
    std::cout << "目标：对左右相机分别进行单目标定，生成矫正图和残差图" << std::endl;
    std::cout << "Target: Calibrate left and right cameras separately, generate corrected and residual images" << std::endl;
    std::cout << std::endl;
    
    // 配置参数 - 8.2mm棋盘格 (Configuration - 8.2mm chessboard)
    const int boardWidth = 9;
    const int boardHeight = 6;
    const float squareSize = 0.0082f;  // 8.2mm = 0.0082m
    const int imageWidth = 3264;
    const int imageHeight = 2448;
    
    // 创建输出目录 (Create output directories)
    std::filesystem::create_directories("calibration_data/output/step3_left_calibration");
    std::filesystem::create_directories("calibration_data/output/step3_right_calibration");
    std::filesystem::create_directories("calibration_data/output/step3_left_corrected");
    std::filesystem::create_directories("calibration_data/output/step3_right_corrected");
    
    std::cout << "标定左相机 (Calibrating left camera)..." << std::endl;
    bool success1 = MonoCalibration::calibrateCamera(
        "calibration_data/output/step2_left_corners",
        "calibration_data/output/step1_left_resized",
        "calibration_data/output/step3_left_calibration",
        boardWidth, boardHeight, squareSize,
        imageWidth, imageHeight,
        true,  // 保存矫正图像 (Save corrected images)
        "calibration_data/output/step3_left_corrected"
    );

    if (success1) {
        std::cout << "✓ 左相机标定完成 (Left camera calibration completed)" << std::endl;
    } else {
        std::cout << "✗ 左相机标定失败 (Left camera calibration failed)" << std::endl;
    }

    std::cout << "标定右相机 (Calibrating right camera)..." << std::endl;
    bool success2 = MonoCalibration::calibrateCamera(
        "calibration_data/output/step2_right_corners",
        "calibration_data/output/step1_right_resized",
        "calibration_data/output/step3_right_calibration",
        boardWidth, boardHeight, squareSize,
        imageWidth, imageHeight,
        true,  // 保存矫正图像 (Save corrected images) 
        "calibration_data/output/step3_right_corrected"
    );

    if (success2) {
        std::cout << "✓ 右相机标定完成 (Right camera calibration completed)" << std::endl;
    } else {
        std::cout << "✗ 右相机标定失败 (Right camera calibration failed)" << std::endl;
    }

    if (success1 && success2) {
        std::cout << "\n✓ 第3步完成：单目标定成功 (Step 3 completed: Monocular calibration successful)" << std::endl;
        std::cout << "生成的文件 (Generated files):" << std::endl;
        std::cout << "矫正图 (Corrected images):" << std::endl;
        std::cout << "- calibration_data/output/step3_left_corrected/" << std::endl;
        std::cout << "- calibration_data/output/step3_right_corrected/" << std::endl;
        std::cout << "残差图 (Residual images):" << std::endl;
        std::cout << "- calibration_data/output/step3_left_calibration/residual_images/" << std::endl;
        std::cout << "- calibration_data/output/step3_right_calibration/residual_images/" << std::endl;
        std::cout << "标定参数 (Calibration parameters):" << std::endl;
        std::cout << "- calibration_data/output/step3_left_calibration/camera_calibration.yml" << std::endl;
        std::cout << "- calibration_data/output/step3_right_calibration/camera_calibration.yml" << std::endl;
    } else {
        std::cout << "✗ 第3步失败：请确保前面步骤成功完成 (Step 3 failed: Please ensure previous steps completed successfully)" << std::endl;
        std::cout << "依赖 (Dependencies):" << std::endl;
        std::cout << "- 第1步: 图像预处理完成 (Step 1: Image preprocessing completed)" << std::endl;
        std::cout << "- 第2步: 角点检测完成 (Step 2: Corner detection completed)" << std::endl;
        return -1;
    }

    return 0;
}