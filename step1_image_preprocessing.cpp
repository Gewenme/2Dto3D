#include "image_resize.h"
#include <iostream>
#include <filesystem>

/**
 * 第1步：图像预处理 - 将左右标定图组图片裁剪为3264×2448像素
 * Step 1: Image Preprocessing - Crop left and right calibration images to 3264×2448 pixels
 */

int main() {
    std::cout << "=== 第1步：图像预处理 (Step 1: Image Preprocessing) ===" << std::endl;
    std::cout << "目标：将图像调整为 3264×2448 像素" << std::endl;
    std::cout << "Target: Resize images to 3264×2448 pixels" << std::endl;
    std::cout << std::endl;
    
    // 配置参数 (Configuration)
    const int imageWidth = 3264;   // 要求的图像宽度
    const int imageHeight = 2448;  // 要求的图像高度
    
    // 创建输出目录 (Create output directories)
    std::filesystem::create_directories("calibration_data/output/step1_left_resized");
    std::filesystem::create_directories("calibration_data/output/step1_right_resized");
    
    std::cout << "处理左相机图像 (Processing left camera images)..." << std::endl;
    bool success1 = ImageUtils::resizeImage(
        "calibration_data/input/left",
        "calibration_data/output/step1_left_resized",
        imageWidth, imageHeight, ImageUtils::LINEAR
    );

    std::cout << "处理右相机图像 (Processing right camera images)..." << std::endl;
    bool success2 = ImageUtils::resizeImage(
        "calibration_data/input/right",
        "calibration_data/output/step1_right_resized", 
        imageWidth, imageHeight, ImageUtils::LINEAR
    );

    if (success1 && success2) {
        std::cout << "✓ 第1步完成：图像预处理成功 (Step 1 completed: Image preprocessing successful)" << std::endl;
        std::cout << "输出路径 (Output paths):" << std::endl;
        std::cout << "- 左图像: calibration_data/output/step1_left_resized/" << std::endl;
        std::cout << "- 右图像: calibration_data/output/step1_right_resized/" << std::endl;
    } else {
        std::cout << "✗ 第1步失败：请检查输入路径 (Step 1 failed: Please check input paths)" << std::endl;
        std::cout << "需要的输入路径 (Required input paths):" << std::endl;
        std::cout << "- calibration_data/input/left/ (放置左相机棋盘格图像)" << std::endl;
        std::cout << "- calibration_data/input/right/ (放置右相机棋盘格图像)" << std::endl;
        return -1;
    }

    return 0;
}