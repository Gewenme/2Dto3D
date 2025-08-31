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

int main() {
    std::cout << "=== 2D to 3D Reconstruction Demonstration ===" << std::endl;
    std::cout << "Following complete pipeline workflow..." << std::endl << std::endl;

    // Step 1: 图像预处理 (Image Preprocessing)
    std::cout << "Step 1: 图像预处理..." << std::endl;
    
    bool success1 = ImageUtils::resizeImage(
        "picture/left",           // 左图像文件夹本地地址
        "output/step1_resize/camL_resized",  // 左图像输出地址
        3264, 2448, 2            // 将图片统一为3264*2448像素，2代表双线性插值
    );

    if (success1) {
        std::cout << "✓ 左图像文件夹处理成功!" << std::endl;
    }
    else {
        std::cerr << "✗ 左图像文件夹处理失败!" << std::endl;
        return -1;
    }

    ImageUtils::ResizeOptions rightOptions(ImageUtils::LINEAR);
    rightOptions.enableSharpening = false;
    rightOptions.reduceNoise = false;
    rightOptions.preserveAspectRatio = true;

    bool success2 = ImageUtils::resizeImage(
        "picture/right",         // 右图像文件夹本地地址
        "output/step1_resize/camR_resized",  // 右图像输出地址
        3264, 2448, 2           // 同样参数
    );

    if (success2) {
        std::cout << "✓ 右图像文件夹处理成功!" << std::endl;
    }
    else {
        std::cerr << "✗ 右图像文件夹处理失败!" << std::endl;
        return -1;
    }



    // Step 2: 角点检测与绘制 (Corner Detection)
    std::cout << "\nStep 2: 角点检测与绘制..." << std::endl;
    
    bool success3 = detectAndDrawCorners(
        "output/step1_resize/camL_resized",     // 左图像文件夹地址
        "output/step2_corners/left_corners",    // 左图像角点输出地址
        9,    // 棋盘内角点数
        6,    // 棋盘内角点数
        1.0f  // 图像放大比例因子
    );

    if (success3) {
        std::cout << "✓ 左相机角点检测完成!" << std::endl;
    }
    else {
        std::cout << "⚠ 左相机角点检测失败，尝试使用备用图像..." << std::endl;
        // Continue with workflow even if some corner detection fails
    }

    bool success4 = detectAndDrawCorners(
        "output/step1_resize/camR_resized",     // 右图像文件夹地址
        "output/step2_corners/right_corners",   // 右图像角点输出地址
        9,    // 棋盘内角点数
        6,    // 棋盘内角点数
        1.0f  // 图像放大比例因子
    );

    if (success4) {
        std::cout << "✓ 右相机角点检测完成!" << std::endl;
    }
    else {
        std::cout << "⚠ 右相机角点检测失败，尝试继续流程..." << std::endl;
        // Continue with workflow 
    }


    // Step 3: 单目标定 (Mono Calibration)
    std::cout << "\nStep 3: 单目标定..." << std::endl;
    
    bool success5 = MonoCalibration::calibrateCamera(
        "output/step2_corners/left_corners/corner_data",  // 左图像角点数据地址
        "output/step1_resize/camL_resized",               // 等待矫正的图像文件夹地址
        "output/step3_calibration/left_calibration",      // 左图像标定参数输出地址
        9,
        6,
        0.0082f,
        3264,
        2448,
        true,
        "output/step3_calibration/left_undistorted" // 输出校正后的图像
    );

    if (success5) {
        std::cout << "✓ 左相机标定成功!" << std::endl;
    } else {
        std::cout << "⚠ 左相机标定失败，继续流程..." << std::endl;
    }
    
    // 右相机标定 (Right Camera Calibration)
    bool success5r = MonoCalibration::calibrateCamera(
        "output/step2_corners/right_corners/corner_data", // 右图像角点数据地址
        "output/step1_resize/camR_resized",               // 等待矫正的图像文件夹地址
        "output/step3_calibration/right_calibration",     // 右图像标定参数输出地址
        9,
        6,
        0.0082f,
        3264,
        2448,
        true,
        "output/step3_calibration/right_undistorted" // 输出校正后的图像
    );

    if (success5r) {
        std::cout << "✓ 右相机标定成功!" << std::endl;
    } else {
        std::cout << "⚠ 右相机标定失败，继续流程..." << std::endl;
    }






    // Step 4: 双目标定 (Stereo Calibration)
    std::cout << "\nStep 4: 双目标定..." << std::endl;
    
    bool success6 = StereoCalibration::calibrateStereoCamera(
        "output/step2_corners/left_corners/corner_data",   // 左图像角点数据地址
        "output/step2_corners/right_corners/corner_data",  // 右图像角点数据地址
        "output/step4_stereo_calibration/stereo_calibration", // 双目标定参数输出地址
        9,
        6,
        0.0082f,
        3264,
        2448
    );
    
    if (success6) {
        std::cout << "✓ 双目标定成功!" << std::endl;
    } else {
        std::cout << "⚠ 双目标定失败，尝试使用默认参数继续..." << std::endl;
    }




    // Step 5: 三维重建 (3D Reconstruction)
    std::cout << "\nStep 5: 三维重建..." << std::endl;
    
    bool success7 = StereoReconstruction::reconstruct3D(
        "picture/scene_left.jpg",    // 左场景图像
        "picture/scene_right.jpg",   // 右场景图像
        "output/step4_stereo_calibration/stereo_calibration", // 双目标定参数文件
        "output/step5_reconstruction/3D_model",               // 三维重建结果输出地址
        0,       // PLY格式
        3,       // 中等质量
        0.0082f, // 棋盘格的实际尺寸,单位为米
        3264,    // 图像宽度
        2448     // 图像高度
    );
    
    if (success7) {
        std::cout << "✓ 三维重建成功!" << std::endl;
    } else {
        std::cout << "⚠ 三维重建失败，尝试继续查看现有数据..." << std::endl;
    }
    
    // Step 6: 三维模型查看和分析 (3D Model Viewing and Analysis)
    std::cout << "\nStep 6: 三维模型查看和分析..." << std::endl;
    
    // 尝试加载重建的3D模型
    std::vector<cv::Point3f> points;
    std::vector<cv::Vec3b> colors;
    
    std::string modelPath = "output/step5_reconstruction/3D_model/reconstruction.ply";
    bool modelLoaded = ModelViewer::loadModel(modelPath, points, colors);
    
    if (modelLoaded && !points.empty()) {
        std::cout << "✓ 3D模型加载成功! 包含 " << points.size() << " 个点" << std::endl;
        
        // 生成投影视图
        std::cout << "生成投影视图..." << std::endl;
        bool viewsGenerated = ModelViewer::generateProjectionViews(
            points, colors, 
            "output/step6_model_views"
        );
        
        if (viewsGenerated) {
            std::cout << "✓ 投影视图生成成功!" << std::endl;
        }
        
        // 保存模型统计信息
        std::cout << "保存模型统计信息..." << std::endl;
        bool statsGenerated = ModelViewer::saveModelStatistics(
            points, 
            "output/step6_model_views/model_statistics.txt"
        );
        
        if (statsGenerated) {
            std::cout << "✓ 模型统计信息保存成功!" << std::endl;
        }
        
        // 显示模型信息
        std::cout << "\n=== 3D模型信息 ===" << std::endl;
        
        // 计算边界框
        cv::Point3f minPoint, maxPoint;
        if (ModelViewer::getBoundingBox(points, minPoint, maxPoint)) {
            std::cout << "模型尺寸范围:" << std::endl;
            std::cout << "  X: " << minPoint.x << " 到 " << maxPoint.x 
                      << " (范围: " << (maxPoint.x - minPoint.x) << ")" << std::endl;
            std::cout << "  Y: " << minPoint.y << " 到 " << maxPoint.y 
                      << " (范围: " << (maxPoint.y - minPoint.y) << ")" << std::endl;
            std::cout << "  Z: " << minPoint.z << " 到 " << maxPoint.z 
                      << " (范围: " << (maxPoint.z - minPoint.z) << ")" << std::endl;
        }
        
        std::cout << "✓ 3D模型分析完成!" << std::endl;
        
    } else {
        std::cout << "⚠ 无法加载3D模型，可能重建步骤失败或文件不存在" << std::endl;
        
        // 尝试直接使用场景图像生成简单的深度估计展示
        std::cout << "尝试基础图像处理展示..." << std::endl;
        
        // 这里可以添加一些基础的图像处理展示代码
        cv::Mat leftScene = cv::imread("picture/scene_left.jpg");
        cv::Mat rightScene = cv::imread("picture/scene_right.jpg");
        
        if (!leftScene.empty() && !rightScene.empty()) {
            std::cout << "✓ 场景图像加载成功" << std::endl;
            std::cout << "  左图像尺寸: " << leftScene.cols << "x" << leftScene.rows << std::endl;
            std::cout << "  右图像尺寸: " << rightScene.cols << "x" << rightScene.rows << std::endl;
        }
    }
    
    std::cout << "\n=== 实验演示完成 ===" << std::endl;
    std::cout << "检查 output/ 目录查看所有生成的结果文件" << std::endl;

    return 0;




























}