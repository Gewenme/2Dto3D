#include "stereo_reconstruction.h"
#include <iostream>
#include <filesystem>

/**
 * 第5步：三维重建 - 基于立体匹配的3D点云重建
 * Step 5: 3D Reconstruction - 3D point cloud reconstruction based on stereo matching
 */

int main() {
    std::cout << "=== 第5步：三维重建 (Step 5: 3D Reconstruction) ===" << std::endl;
    std::cout << "目标：基于立体匹配的3D点云重建" << std::endl;
    std::cout << "Target: 3D point cloud reconstruction based on stereo matching" << std::endl;
    std::cout << std::endl;
    
    // 配置参数 - 8.2mm棋盘格 (Configuration - 8.2mm chessboard)
    const float squareSize = 0.0082f;  // 8.2mm = 0.0082m
    const int imageWidth = 3264;
    const int imageHeight = 2448;
    
    // 创建输出目录 (Create output directory)
    std::filesystem::create_directories("calibration_data/output/step5_3d_reconstruction");
    
    // 检查是否有场景图像用于重建 (Check if scene images exist for reconstruction)
    if (std::filesystem::exists("calibration_data/input/scene_left.jpg") &&
        std::filesystem::exists("calibration_data/input/scene_right.jpg")) {
        
        std::cout << "找到场景图像，开始3D重建 (Scene images found, starting 3D reconstruction)..." << std::endl;
        std::cout << "使用8.2mm棋盘格标定参数 (Using 8.2mm chessboard calibration parameters)" << std::endl;
        
        bool success = StereoReconstruction::reconstruct3D(
            "calibration_data/input/scene_left.jpg",
            "calibration_data/input/scene_right.jpg",
            "calibration_data/output/step4_stereo_calibration",
            "calibration_data/output/step5_3d_reconstruction",
            StereoReconstruction::PLY_FORMAT,  // PLY格式
            StereoReconstruction::MEDIUM_QUALITY,  // 中等质量
            squareSize,  // 8.2mm棋盘格尺寸
            imageWidth, imageHeight
        );

        if (success) {
            std::cout << "✓ 第5步完成：3D重建成功 (Step 5 completed: 3D reconstruction successful)" << std::endl;
            std::cout << "生成的文件 (Generated files):" << std::endl;
            std::cout << "- calibration_data/output/step5_3d_reconstruction/reconstruction.ply" << std::endl;
            std::cout << "- calibration_data/output/step5_3d_reconstruction/disparity_map.png" << std::endl;
            std::cout << "- calibration_data/output/step5_3d_reconstruction/rectified_left.jpg" << std::endl;
            std::cout << "- calibration_data/output/step5_3d_reconstruction/rectified_right.jpg" << std::endl;
            std::cout << "3D点云文件可用于查看和进一步处理 (3D point cloud file ready for viewing and further processing)" << std::endl;
        } else {
            std::cout << "✗ 第5步失败：3D重建失败 (Step 5 failed: 3D reconstruction failed)" << std::endl;
            std::cout << "可能原因 (Possible causes):" << std::endl;
            std::cout << "- 场景图像质量不足 (Poor scene image quality)" << std::endl;
            std::cout << "- 场景缺乏纹理特征 (Scene lacks texture features)" << std::endl;
            std::cout << "- 双目标定参数不正确 (Incorrect stereo calibration parameters)" << std::endl;
            return -1;
        }
    } else {
        std::cout << "✗ 第5步失败：未找到场景图像 (Step 5 failed: Scene images not found)" << std::endl;
        std::cout << "需要的文件 (Required files):" << std::endl;
        std::cout << "- calibration_data/input/scene_left.jpg  (左场景图像)" << std::endl;
        std::cout << "- calibration_data/input/scene_right.jpg (右场景图像)" << std::endl;
        std::cout << "依赖 (Dependencies):" << std::endl;
        std::cout << "- 第1步: 图像预处理完成 (Step 1: Image preprocessing completed)" << std::endl;
        std::cout << "- 第2步: 角点检测完成 (Step 2: Corner detection completed)" << std::endl;
        std::cout << "- 第3步: 单目标定完成 (Step 3: Monocular calibration completed)" << std::endl;
        std::cout << "- 第4步: 双目标定完成 (Step 4: Stereo calibration completed)" << std::endl;
        return -1;
    }

    return 0;
}