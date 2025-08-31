#include "image_resize.h"
#include "corner_detection.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "model_viewer.h"

#include <iostream>
#include <filesystem>

/**
 * Demonstration script for camera calibration and 3D reconstruction
 * Using 8.2mm chessboard squares as specified in the requirements
 * Generates: corrected images (矫正图), residual images (残差图), and 3D models (模型)
 */
int main() {
    std::cout << "=== 相机标定与3D重建演示 (8.2mm方格) ===" << std::endl;
    std::cout << "=== Camera Calibration & 3D Reconstruction Demo (8.2mm Squares) ===" << std::endl;
    std::cout << std::endl;
    std::cout << "本演示将生成 (This demo will generate):" << std::endl;
    std::cout << "- 矫正图 (Corrected images): 去畸变后的图像" << std::endl;
    std::cout << "- 残差图 (Residual images): 标定误差可视化图像" << std::endl;
    std::cout << "- 模型 (3D Models): 三维重建点云模型" << std::endl;
    std::cout << std::endl;
    
    // Create demo directory structure
    std::filesystem::create_directories("demo_8_2mm/input/left");
    std::filesystem::create_directories("demo_8_2mm/input/right");
    std::filesystem::create_directories("demo_8_2mm/output");
    
    // Configuration using 8.2mm squares as required
    const int boardWidth = 9;
    const int boardHeight = 6;
    const float squareSize = 0.0082f;  // 8.2mm squares - 关键参数
    const int imageWidth = 3264;   // Updated to meet requirement: 3264×2448像素
    const int imageHeight = 2448;  // Updated to meet requirement: 3264×2448像素
    
    std::cout << "配置参数 (Configuration):" << std::endl;
    std::cout << "- 棋盘格 (Chessboard): " << boardWidth << "x" << boardHeight << " 内角点" << std::endl;
    std::cout << "- 方格边长 (Square size): " << squareSize * 1000 << "mm" << std::endl;
    std::cout << "- 图像尺寸 (Image size): " << imageWidth << "x" << imageHeight << " (要求:3264×2448像素)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. 图像调整 (Image Resizing)..." << std::endl;
    
    // Step 1: Resize images (if input images exist)
    if (std::filesystem::exists("demo_8_2mm/input/left") && 
        !std::filesystem::is_empty("demo_8_2mm/input/left")) {
        
        bool success1 = ImageUtils::resizeImage(
            "demo_8_2mm/input/left",
            "demo_8_2mm/output/left_resized",
            imageWidth, imageHeight, 
            ImageUtils::LINEAR
        );
        
        bool success2 = ImageUtils::resizeImage(
            "demo_8_2mm/input/right",
            "demo_8_2mm/output/right_resized",
            imageWidth, imageHeight, 
            ImageUtils::LINEAR
        );
        
        if (success1 && success2) {
            std::cout << "✓ 图像调整完成 (Image resizing completed)" << std::endl;
            
            std::cout << "\n2. 角点检测 (Corner Detection)..." << std::endl;
            
            // Step 2: Corner detection
            bool corners1 = detectAndDrawCorners(
                "demo_8_2mm/output/left_resized",
                "demo_8_2mm/output/left_corners",
                boardWidth, boardHeight, 1.0f
            );
            
            bool corners2 = detectAndDrawCorners(
                "demo_8_2mm/output/right_resized", 
                "demo_8_2mm/output/right_corners",
                boardWidth, boardHeight, 1.0f
            );
            
            if (corners1 && corners2) {
                std::cout << "✓ 角点检测完成 (Corner detection completed)" << std::endl;
                
                std::cout << "\n3. 相机标定 (Camera Calibration) - 生成矫正图和残差图..." << std::endl;
                
                // Step 3: Mono calibration with corrected image generation
                bool leftCalib = MonoCalibration::calibrateCamera(
                    "demo_8_2mm/output/left_corners",
                    "demo_8_2mm/output/left_resized", 
                    "demo_8_2mm/output/left_calibration",
                    boardWidth, boardHeight, squareSize,
                    imageWidth, imageHeight,
                    true,  // Generate corrected (undistorted) images
                    "demo_8_2mm/output/left_corrected"
                );
                
                bool rightCalib = MonoCalibration::calibrateCamera(
                    "demo_8_2mm/output/right_corners",
                    "demo_8_2mm/output/right_resized", 
                    "demo_8_2mm/output/right_calibration",
                    boardWidth, boardHeight, squareSize,
                    imageWidth, imageHeight,
                    true,  // Generate corrected (undistorted) images
                    "demo_8_2mm/output/right_corrected"
                );
                
                if (leftCalib && rightCalib) {
                    std::cout << "✓ 单目标定完成 (Mono calibration completed)" << std::endl;
                    std::cout << "  ✓ 已生成矫正图 (Generated corrected images)" << std::endl;
                    std::cout << "  ✓ 已生成残差图 (Generated residual error images)" << std::endl;
                    
                    std::cout << "\n4. 双目标定 (Stereo Calibration)..." << std::endl;
                    
                    // Step 4: Stereo calibration
                    bool stereoCalib = StereoCalibration::calibrateStereoCamera(
                        "demo_8_2mm/output/left_corners",
                        "demo_8_2mm/output/right_corners",
                        "demo_8_2mm/output/stereo_calibration",
                        boardWidth, boardHeight, squareSize,
                        imageWidth, imageHeight
                    );
                    
                    if (stereoCalib) {
                        std::cout << "✓ 双目标定完成 (Stereo calibration completed)" << std::endl;
                        
                        // For 3D reconstruction, we need actual scene images
                        // Check if scene images exist
                        if (std::filesystem::exists("demo_8_2mm/input/scene_left.jpg") &&
                            std::filesystem::exists("demo_8_2mm/input/scene_right.jpg")) {
                            
                            std::cout << "\n5. 3D模型重建 (3D Model Reconstruction)..." << std::endl;
                            
                            bool reconstruction = StereoReconstruction::reconstruct3D(
                                "demo_8_2mm/input/scene_left.jpg",
                                "demo_8_2mm/input/scene_right.jpg",
                                "demo_8_2mm/output/stereo_calibration",
                                "demo_8_2mm/output/3d_model",
                                StereoReconstruction::PLY_FORMAT,
                                StereoReconstruction::MEDIUM_QUALITY,
                                squareSize, imageWidth, imageHeight
                            );
                            
                            if (reconstruction) {
                                std::cout << "✓ 3D模型重建完成 (3D model reconstruction completed)" << std::endl;
                                
                                std::cout << "\n6. 模型分析 (Model Analysis)..." << std::endl;
                                
                                // Load and analyze the 3D model
                                std::vector<cv::Point3f> points;
                                std::vector<cv::Vec3b> colors;
                                
                                if (ModelViewer::loadModel("demo_8_2mm/output/3d_model/reconstruction.ply", 
                                                         points, colors)) {
                                    std::cout << "✓ 3D模型加载成功 (3D model loaded) - " << points.size() << " 个点" << std::endl;
                                    
                                    // Generate projection views
                                    ModelViewer::generateProjectionViews(points, colors, 
                                                                        "demo_8_2mm/output/model_views");
                                    
                                    // Save statistics
                                    ModelViewer::saveModelStatistics(points, 
                                                                    "demo_8_2mm/output/model_statistics.txt");
                                    
                                    std::cout << "✓ 模型分析完成 (Model analysis completed)" << std::endl;
                                    
                                    std::cout << "\n🎉 所有输出生成成功! (All outputs generated successfully!)" << std::endl;
                                    std::cout << "\n生成的文件 (Generated files):" << std::endl;
                                    std::cout << "📁 demo_8_2mm/output/left_corrected/ - 左相机矫正图" << std::endl;
                                    std::cout << "📁 demo_8_2mm/output/right_corrected/ - 右相机矫正图" << std::endl;
                                    std::cout << "📁 demo_8_2mm/output/left_calibration/residual_images/ - 左相机残差图" << std::endl;
                                    std::cout << "📁 demo_8_2mm/output/right_calibration/residual_images/ - 右相机残差图" << std::endl;
                                    std::cout << "📁 demo_8_2mm/output/3d_model/ - 3D重建模型" << std::endl;
                                }
                            } else {
                                std::cerr << "✗ 3D模型重建失败 (3D model reconstruction failed)" << std::endl;
                            }
                        } else {
                            std::cout << "\nℹ  未找到场景图像 (Scene images not found)" << std::endl;
                            std::cout << "   在 demo_8_2mm/input/ 放置 scene_left.jpg 和 scene_right.jpg 以测试重建" << std::endl;
                            std::cout << "✓ 标定和矫正图/残差图生成成功! (Calibration and corrected/residual images generated!)" << std::endl;
                        }
                    } else {
                        std::cerr << "✗ 双目标定失败 (Stereo calibration failed)" << std::endl;
                    }
                } else {
                    std::cerr << "✗ 相机标定失败 (Camera calibration failed)" << std::endl;
                }
            } else {
                std::cerr << "✗ 角点检测失败 (Corner detection failed)" << std::endl;
                std::cout << "   确保输入图像包含可见的棋盘格图案 (Make sure input images contain visible chessboard patterns)" << std::endl;
            }
        } else {
            std::cerr << "✗ 图像调整失败 (Image resizing failed)" << std::endl;
        }
    } else {
        std::cout << "\nℹ  在 demo_8_2mm/input/left 和 demo_8_2mm/input/right 中未找到输入图像" << std::endl;
        std::cout << "   请添加棋盘格标定图像以测试完整流程" << std::endl;
        std::cout << "   (Please add chessboard calibration images to test the complete pipeline)" << std::endl;
        
        // Create sample directory structure info
        std::cout << "\n期望的目录结构 (Expected directory structure):" << std::endl;
        std::cout << "demo_8_2mm/" << std::endl;
        std::cout << "├── input/" << std::endl;
        std::cout << "│   ├── left/           # 左相机棋盘格图像 (8.2mm方格)" << std::endl;
        std::cout << "│   ├── right/          # 右相机棋盘格图像 (8.2mm方格)" << std::endl;
        std::cout << "│   ├── scene_left.jpg  # 左场景图像 (用于3D重建)" << std::endl;
        std::cout << "│   └── scene_right.jpg # 右场景图像 (用于3D重建)" << std::endl;
        std::cout << "└── output/" << std::endl;
        std::cout << "    ├── left_corrected/    # 左相机矫正图" << std::endl;
        std::cout << "    ├── right_corrected/   # 右相机矫正图" << std::endl;
        std::cout << "    ├── left_calibration/residual_images/  # 左相机残差图" << std::endl;
        std::cout << "    ├── right_calibration/residual_images/ # 右相机残差图" << std::endl;
        std::cout << "    └── 3d_model/          # 3D重建模型" << std::endl;
    }
    
    std::cout << "\n=== 演示完成 (Demo Complete) ===" << std::endl;
    std::cout << "关键输出 (Key outputs generated when input data is available):" << std::endl;
    std::cout << "1. 矫正图 (Corrected Images): 去除镜头畸变的图像" << std::endl;
    std::cout << "2. 残差图 (Residual Images): 标定精度/误差的可视化" << std::endl;
    std::cout << "3. 3D模型 (3D Model): 从立体图像重建的点云" << std::endl;
    
    return 0;
}