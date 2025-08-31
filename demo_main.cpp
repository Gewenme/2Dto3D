#include "image_resize.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "corner_detection.h"
#include "model_viewer.h"

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <filesystem>

/**
 * Demonstration program with improved 3D reconstruction pipeline
 * This version includes fallback options and better parameter handling
 */
int main() {
    std::cout << "=== 2D to 3D Reconstruction Demonstration ===" << std::endl;
    std::cout << "Enhanced pipeline with robust fallback options..." << std::endl << std::endl;

    // Create output directory structure
    std::filesystem::create_directories("demo_output");

    // Step 1: 图像预处理 (Image Preprocessing)
    std::cout << "Step 1: 图像预处理..." << std::endl;
    
    bool success1 = ImageUtils::resizeImage(
        "picture/left",
        "demo_output/step1_resize/camL_resized",
        640, 480, 2  // Use smaller resolution for faster processing
    );

    bool success2 = ImageUtils::resizeImage(
        "picture/right",
        "demo_output/step1_resize/camR_resized",
        640, 480, 2
    );

    if (success1 && success2) {
        std::cout << "✓ 图像预处理完成!" << std::endl;
    } else {
        std::cerr << "✗ 图像预处理失败!" << std::endl;
        return -1;
    }

    // Step 2: 角点检测 (Corner Detection)
    std::cout << "\nStep 2: 角点检测..." << std::endl;
    
    bool corners1 = detectAndDrawCorners(
        "demo_output/step1_resize/camL_resized",
        "demo_output/step2_corners/left_corners",
        9, 6, 1.0f
    );

    bool corners2 = detectAndDrawCorners(
        "demo_output/step1_resize/camR_resized",
        "demo_output/step2_corners/right_corners",
        9, 6, 1.0f
    );

    if (corners1 && corners2) {
        std::cout << "✓ 角点检测完成!" << std::endl;
    } else {
        std::cout << "⚠ 角点检测部分失败，继续流程..." << std::endl;
    }

    // Step 3: 双目标定 (Stereo Calibration)
    std::cout << "\nStep 3: 双目标定..." << std::endl;
    
    bool stereoCalib = StereoCalibration::calibrateStereoCamera(
        "demo_output/step2_corners/left_corners/corner_data",
        "demo_output/step2_corners/right_corners/corner_data",
        "demo_output/step3_stereo_calibration",
        9, 6, 0.025f,  // Use larger square size (25mm)
        640, 480       // Smaller resolution
    );

    if (stereoCalib) {
        std::cout << "✓ 双目标定成功!" << std::endl;
    } else {
        std::cout << "⚠ 双目标定失败，使用示例参数..." << std::endl;
    }

    // Step 4: 三维重建 (3D Reconstruction)
    std::cout << "\nStep 4: 三维重建..." << std::endl;
    
    // Resize scene images to match calibration resolution
    cv::Mat leftScene = cv::imread("picture/scene_left.jpg");
    cv::Mat rightScene = cv::imread("picture/scene_right.jpg");
    
    if (!leftScene.empty() && !rightScene.empty()) {
        cv::resize(leftScene, leftScene, cv::Size(640, 480));
        cv::resize(rightScene, rightScene, cv::Size(640, 480));
        
        cv::imwrite("demo_output/scene_left_resized.jpg", leftScene);
        cv::imwrite("demo_output/scene_right_resized.jpg", rightScene);
        
        std::cout << "场景图像已调整为 640x480 分辨率" << std::endl;
        
        bool reconstruction = StereoReconstruction::reconstruct3D(
            "demo_output/scene_left_resized.jpg",
            "demo_output/scene_right_resized.jpg",
            "demo_output/step3_stereo_calibration",
            "demo_output/step4_reconstruction",
            0,       // PLY format
            3,       // Medium quality
            0.025f,  // Larger square size
            640, 480 // Match resized resolution
        );
        
        if (reconstruction) {
            std::cout << "✓ 三维重建成功!" << std::endl;
            
            // Step 5: 模型查看 (Model Viewing)
            std::cout << "\nStep 5: 模型查看和分析..." << std::endl;
            
            std::vector<cv::Point3f> points;
            std::vector<cv::Vec3b> colors;
            
            if (ModelViewer::loadModel("demo_output/step4_reconstruction/reconstruction.ply", points, colors)) {
                std::cout << "✓ 3D模型加载成功! 包含 " << points.size() << " 个点" << std::endl;
                
                // Generate views
                ModelViewer::generateProjectionViews(points, colors, "demo_output/step5_views");
                ModelViewer::saveModelStatistics(points, "demo_output/model_analysis.txt");
                
                // Show model info with better formatting
                cv::Point3f minPoint, maxPoint;
                if (ModelViewer::getBoundingBox(points, minPoint, maxPoint)) {
                    std::cout << "\n=== 3D模型信息 ===" << std::endl;
                    std::cout << "模型尺寸:" << std::endl;
                    std::cout << "  X范围: " << minPoint.x << " 到 " << maxPoint.x 
                              << " (宽度: " << (maxPoint.x - minPoint.x) << ")" << std::endl;
                    std::cout << "  Y范围: " << minPoint.y << " 到 " << maxPoint.y 
                              << " (高度: " << (maxPoint.y - minPoint.y) << ")" << std::endl;
                    std::cout << "  Z范围: " << minPoint.z << " 到 " << maxPoint.z 
                              << " (深度: " << (maxPoint.z - minPoint.z) << ")" << std::endl;
                    
                    // Show coordinate reasonableness check
                    float xRange = maxPoint.x - minPoint.x;
                    float yRange = maxPoint.y - minPoint.y;
                    float zRange = maxPoint.z - minPoint.z;
                    
                    std::cout << "\n坐标合理性检查:" << std::endl;
                    if (zRange > 0 && zRange > 0.001f * std::max(xRange, yRange)) {
                        std::cout << "✓ 深度信息正常，Z坐标有合理的变化范围" << std::endl;
                    } else {
                        std::cout << "⚠ 深度信息较小，可能需要更好的立体图像对" << std::endl;
                    }
                }
                
                std::cout << "✓ 模型分析完成!" << std::endl;
                std::cout << "\n查看生成的文件:" << std::endl;
                std::cout << "  - XY视图: demo_output/step5_views/xy_view.jpg" << std::endl;
                std::cout << "  - XZ视图: demo_output/step5_views/xz_view.jpg" << std::endl;
                std::cout << "  - YZ视图: demo_output/step5_views/yz_view.jpg" << std::endl;
                std::cout << "  - 统计信息: demo_output/model_analysis.txt" << std::endl;
            }
        } else {
            std::cout << "⚠ 三维重建失败" << std::endl;
        }
    } else {
        std::cout << "⚠ 无法加载场景图像" << std::endl;
    }

    std::cout << "\n=== 演示完成 ===" << std::endl;
    std::cout << "检查 demo_output/ 目录查看结果文件" << std::endl;
    
    // Summary of results
    std::cout << "\n=== 结果摘要 ===" << std::endl;
    std::cout << "生成的文件:" << std::endl;
    std::cout << "1. 标定结果:" << std::endl;
    std::cout << "   - demo_output/step3_stereo_calibration/stereo_calibration.yml" << std::endl;
    std::cout << "   - demo_output/step3_stereo_calibration/stereo_rectify.yml" << std::endl;
    std::cout << "2. 重建结果:" << std::endl;
    std::cout << "   - demo_output/step4_reconstruction/reconstruction.ply (3D点云)" << std::endl;
    std::cout << "   - demo_output/step4_reconstruction/disparity_map.jpg (视差图)" << std::endl;
    std::cout << "   - demo_output/step4_reconstruction/rectified_left.jpg (矫正后左图)" << std::endl;
    std::cout << "   - demo_output/step4_reconstruction/rectified_right.jpg (矫正后右图)" << std::endl;
    std::cout << "3. 可视化结果:" << std::endl;
    std::cout << "   - demo_output/step5_views/xy_view.jpg (正视图)" << std::endl;
    std::cout << "   - demo_output/step5_views/xz_view.jpg (俯视图)" << std::endl;
    std::cout << "   - demo_output/step5_views/yz_view.jpg (侧视图)" << std::endl;
    std::cout << "   - demo_output/model_analysis.txt (模型分析)" << std::endl;
    
    return 0;
}