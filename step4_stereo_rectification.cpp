#include "stereo_calibration.h"
#include <iostream>
#include <filesystem>

/**
 * 第4步：立体校正 - 双目标定和立体校正
 * Step 4: Stereo Rectification - Stereo calibration and rectification
 */

int main() {
    std::cout << "=== 第4步：双目标定和立体校正 (Step 4: Stereo Calibration and Rectification) ===" << std::endl;
    std::cout << "目标：进行双目标定，生成立体校正参数" << std::endl;
    std::cout << "Target: Perform stereo calibration and generate rectification parameters" << std::endl;
    std::cout << std::endl;
    
    // 配置参数 - 8.2mm棋盘格 (Configuration - 8.2mm chessboard)
    const int boardWidth = 9;
    const int boardHeight = 6;
    const float squareSize = 0.0082f;  // 8.2mm = 0.0082m
    const int imageWidth = 3264;
    const int imageHeight = 2448;
    
    // 创建输出目录 (Create output directory)
    std::filesystem::create_directories("calibration_data/output/step4_stereo_calibration");
    
    std::cout << "执行双目标定 (Performing stereo calibration)..." << std::endl;
    std::cout << "使用8.2mm棋盘格参数 (Using 8.2mm chessboard parameters)" << std::endl;
    
    bool success = StereoCalibration::calibrateStereoCamera(
        "calibration_data/output/step2_left_corners",
        "calibration_data/output/step2_right_corners",
        "calibration_data/output/step4_stereo_calibration",
        boardWidth, boardHeight, squareSize,
        imageWidth, imageHeight
    );

    if (success) {
        std::cout << "✓ 第4步完成：双目标定和立体校正成功 (Step 4 completed: Stereo calibration and rectification successful)" << std::endl;
        std::cout << "生成的文件 (Generated files):" << std::endl;
        std::cout << "- calibration_data/output/step4_stereo_calibration/stereo_calibration.yml" << std::endl;
        std::cout << "- calibration_data/output/step4_stereo_calibration/stereo_rectify.yml" << std::endl;
        std::cout << "内容包含 (Content includes):" << std::endl;
        std::cout << "  - 左右相机的内参矩阵和畸变系数" << std::endl;
        std::cout << "  - 相机之间的旋转和平移矩阵" << std::endl;
        std::cout << "  - 立体校正参数" << std::endl;
    } else {
        std::cout << "✗ 第4步失败：双目标定失败 (Step 4 failed: Stereo calibration failed)" << std::endl;
        std::cout << "请确保 (Please ensure):" << std::endl;
        std::cout << "- 第1步: 图像预处理完成 (Step 1: Image preprocessing completed)" << std::endl;
        std::cout << "- 第2步: 角点检测完成 (Step 2: Corner detection completed)" << std::endl;
        std::cout << "- 左右图像有足够的角点数据 (Left and right images have sufficient corner data)" << std::endl;
        std::cout << "- 棋盘格边长确实为8.2mm (Chessboard square size is indeed 8.2mm)" << std::endl;
        return -1;
    }

    return 0;
}