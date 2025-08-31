#include "corner_detection.h"
#include <iostream>
#include <filesystem>

/**
 * 第2步：角点检测 - 检测棋盘格角点并可视化
 * Step 2: Corner Detection - Detect chessboard corners and visualize
 */

int main() {
    std::cout << "=== 第2步：角点检测与可视化 (Step 2: Corner Detection and Visualization) ===" << std::endl;
    std::cout << "目标：检测9×6棋盘格内角点并可视化" << std::endl;
    std::cout << "Target: Detect 9×6 chessboard inner corners and visualize" << std::endl;
    std::cout << std::endl;
    
    // 配置参数 (Configuration)
    const int boardWidth = 9;   // 棋盘格内角点宽度
    const int boardHeight = 6;  // 棋盘格内角点高度
    
    // 创建输出目录 (Create output directories)
    std::filesystem::create_directories("calibration_data/output/step2_left_corners");
    std::filesystem::create_directories("calibration_data/output/step2_right_corners");
    
    std::cout << "检测左相机图像角点 (Detecting left camera image corners)..." << std::endl;
    bool success1 = detectAndDrawCorners(
        "calibration_data/output/step1_left_resized",
        "calibration_data/output/step2_left_corners",
        boardWidth, boardHeight, 1.0f
    );

    std::cout << "检测右相机图像角点 (Detecting right camera image corners)..." << std::endl;
    bool success2 = detectAndDrawCorners(
        "calibration_data/output/step1_right_resized",
        "calibration_data/output/step2_right_corners",
        boardWidth, boardHeight, 1.0f
    );

    if (success1 && success2) {
        std::cout << "✓ 第2步完成：角点检测与可视化成功 (Step 2 completed: Corner detection and visualization successful)" << std::endl;
        std::cout << "输出路径 (Output paths):" << std::endl;
        std::cout << "- 左相机角点: calibration_data/output/step2_left_corners/" << std::endl;
        std::cout << "- 右相机角点: calibration_data/output/step2_right_corners/" << std::endl;
        std::cout << "包含 (Contains):" << std::endl;
        std::cout << "  - 带角点标记的图像 (Images with corner markings)" << std::endl;
        std::cout << "  - corner_data.yml (角点坐标数据)" << std::endl;
    } else {
        std::cout << "✗ 第2步失败：角点检测失败 (Step 2 failed: Corner detection failed)" << std::endl;
        std::cout << "请确保 (Please ensure):" << std::endl;
        std::cout << "- 已完成第1步图像预处理 (Step 1 image preprocessing completed)" << std::endl;
        std::cout << "- 图像包含清晰的9×6棋盘格图案 (Images contain clear 9×6 chessboard patterns)" << std::endl;
        std::cout << "- 棋盘格边长为8.2mm (Chessboard square size is 8.2mm)" << std::endl;
        return -1;
    }

    return 0;
}