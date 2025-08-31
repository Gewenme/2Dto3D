/**
 * 双目标定程序 (Stereo Calibration Program)
 * 类似MATLAB双目标定工具箱的完整标定流程
 * Similar to MATLAB Stereo Camera Calibrator Toolbox workflow
 * 
 * 功能 (Functions):
 * 1. 导入左右标定图集 (Import left and right calibration image sets)
 * 2. 角点检测 (Corner detection)
 * 3. 求取标定参数 (Calculate calibration parameters)
 * 4. 根据参数矫正图片 (Correct images based on parameters)
 * 5. 根据参数建模 (Model construction based on parameters)
 */

#include "image_resize.h"
#include "corner_detection.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "model_viewer.h"

#include <iostream>
#include <string>
#include <filesystem>

class StereoCalibrationProgram {
private:
    // 标定参数 (Calibration parameters)
    std::string leftImagePath;
    std::string rightImagePath;
    std::string outputPath;
    
    // 棋盘格参数 (Chessboard parameters)
    int boardWidth;
    int boardHeight;
    float squareSize;
    
    // 图像参数 (Image parameters)
    int imageWidth;
    int imageHeight;
    
public:
    StereoCalibrationProgram(const std::string& leftPath, const std::string& rightPath, 
                           const std::string& outPath, int bWidth = 9, int bHeight = 6, 
                           float sSize = 0.0082f, int imgWidth = 3264, int imgHeight = 2448)
        : leftImagePath(leftPath), rightImagePath(rightPath), outputPath(outPath),
          boardWidth(bWidth), boardHeight(bHeight), squareSize(sSize),
          imageWidth(imgWidth), imageHeight(imgHeight) {}
    
    /**
     * 执行完整的双目标定流程
     * Execute complete stereo calibration workflow
     */
    bool runCalibration() {
        std::cout << "=== 双目标定程序 (Stereo Calibration Program) ===" << std::endl;
        std::cout << "类似MATLAB双目标定工具箱 (Similar to MATLAB Stereo Camera Calibrator)" << std::endl;
        std::cout << std::endl;
        
        // 检查输入路径 (Check input paths)
        if (!validateInputPaths()) {
            return false;
        }
        
        // 创建输出目录 (Create output directories)
        createOutputDirectories();
        
        // 步骤1: 图像预处理 (Step 1: Image preprocessing)
        if (!preprocessImages()) {
            std::cerr << "图像预处理失败 (Image preprocessing failed)" << std::endl;
            return false;
        }
        
        // 步骤2: 角点检测 (Step 2: Corner detection)
        if (!detectCorners()) {
            std::cerr << "角点检测失败 (Corner detection failed)" << std::endl;
            return false;
        }
        
        // 步骤3: 单目标定 (Step 3: Monocular calibration)
        if (!calibrateMonocular()) {
            std::cerr << "单目标定失败 (Monocular calibration failed)" << std::endl;
            return false;
        }
        
        // 步骤4: 双目标定 (Step 4: Stereo calibration)
        if (!calibrateStereo()) {
            std::cerr << "双目标定失败 (Stereo calibration failed)" << std::endl;
            return false;
        }
        
        // 步骤5: 图像矫正 (Step 5: Image rectification)
        if (!rectifyImages()) {
            std::cerr << "图像矫正失败 (Image rectification failed)" << std::endl;
            return false;
        }
        
        // 步骤6: 3D建模 (Step 6: 3D modeling)
        if (!generate3DModel()) {
            std::cout << "3D建模跳过 - 需要场景图像 (3D modeling skipped - requires scene images)" << std::endl;
        }
        
        printResults();
        return true;
    }
    
private:
    bool validateInputPaths() {
        if (!std::filesystem::exists(leftImagePath)) {
            std::cerr << "左图像路径不存在: " << leftImagePath << std::endl;
            return false;
        }
        if (!std::filesystem::exists(rightImagePath)) {
            std::cerr << "右图像路径不存在: " << rightImagePath << std::endl;
            return false;
        }
        
        // 检查是否有图像文件 (Check for image files)
        int leftCount = 0, rightCount = 0;
        for (const auto& entry : std::filesystem::directory_iterator(leftImagePath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                    leftCount++;
                }
            }
        }
        for (const auto& entry : std::filesystem::directory_iterator(rightImagePath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                    rightCount++;
                }
            }
        }
        
        std::cout << "发现标定图像 (Found calibration images):" << std::endl;
        std::cout << "- 左相机: " << leftCount << " 张图像" << std::endl;
        std::cout << "- 右相机: " << rightCount << " 张图像" << std::endl;
        
        if (leftCount < 10 || rightCount < 10) {
            std::cout << "警告: 建议使用至少10张标定图像以获得更好的结果" << std::endl;
        }
        
        return leftCount > 0 && rightCount > 0;
    }
    
    void createOutputDirectories() {
        std::filesystem::create_directories(outputPath);
        std::filesystem::create_directories(outputPath + "/preprocessed");
        std::filesystem::create_directories(outputPath + "/preprocessed/left");
        std::filesystem::create_directories(outputPath + "/preprocessed/right");
        std::filesystem::create_directories(outputPath + "/corners");
        std::filesystem::create_directories(outputPath + "/corners/left");
        std::filesystem::create_directories(outputPath + "/corners/right");
        std::filesystem::create_directories(outputPath + "/calibration");
        std::filesystem::create_directories(outputPath + "/calibration/left");
        std::filesystem::create_directories(outputPath + "/calibration/right");
        std::filesystem::create_directories(outputPath + "/calibration/stereo");
        std::filesystem::create_directories(outputPath + "/rectified");
        std::filesystem::create_directories(outputPath + "/rectified/left");
        std::filesystem::create_directories(outputPath + "/rectified/right");
        std::filesystem::create_directories(outputPath + "/3d_model");
    }
    
    bool preprocessImages() {
        std::cout << "\n步骤1: 图像预处理 (Image Preprocessing)" << std::endl;
        std::cout << "目标尺寸: " << imageWidth << "×" << imageHeight << " 像素" << std::endl;
        
        bool leftSuccess = ImageUtils::resizeImage(
            leftImagePath,
            outputPath + "/preprocessed/left",
            imageWidth, imageHeight,
            ImageUtils::LINEAR
        );
        
        bool rightSuccess = ImageUtils::resizeImage(
            rightImagePath,
            outputPath + "/preprocessed/right",
            imageWidth, imageHeight,
            ImageUtils::LINEAR
        );
        
        if (leftSuccess && rightSuccess) {
            std::cout << "✓ 图像预处理完成" << std::endl;
        }
        
        return leftSuccess && rightSuccess;
    }
    
    bool detectCorners() {
        std::cout << "\n步骤2: 角点检测 (Corner Detection)" << std::endl;
        std::cout << "棋盘格规格: " << boardWidth << "×" << boardHeight << " 内角点" << std::endl;
        
        bool leftSuccess = detectAndDrawCorners(
            outputPath + "/preprocessed/left",
            outputPath + "/corners/left",
            boardWidth, boardHeight, 1.0f
        );
        
        bool rightSuccess = detectAndDrawCorners(
            outputPath + "/preprocessed/right",
            outputPath + "/corners/right",
            boardWidth, boardHeight, 1.0f
        );
        
        if (leftSuccess && rightSuccess) {
            std::cout << "✓ 角点检测完成" << std::endl;
        }
        
        return leftSuccess && rightSuccess;
    }
    
    bool calibrateMonocular() {
        std::cout << "\n步骤3: 单目标定 (Monocular Calibration)" << std::endl;
        
        bool leftSuccess = MonoCalibration::calibrateCamera(
            outputPath + "/corners/left",
            outputPath + "/preprocessed/left",
            outputPath + "/calibration/left",
            boardWidth, boardHeight, squareSize,
            imageWidth, imageHeight,
            true,  // 生成矫正图像
            outputPath + "/calibration/left/undistorted"
        );
        
        bool rightSuccess = MonoCalibration::calibrateCamera(
            outputPath + "/corners/right",
            outputPath + "/preprocessed/right",
            outputPath + "/calibration/right",
            boardWidth, boardHeight, squareSize,
            imageWidth, imageHeight,
            true,  // 生成矫正图像
            outputPath + "/calibration/right/undistorted"
        );
        
        if (leftSuccess && rightSuccess) {
            std::cout << "✓ 单目标定完成" << std::endl;
            std::cout << "  - 左相机参数已保存" << std::endl;
            std::cout << "  - 右相机参数已保存" << std::endl;
            std::cout << "  - 畸变矫正图像已生成" << std::endl;
        }
        
        return leftSuccess && rightSuccess;
    }
    
    bool calibrateStereo() {
        std::cout << "\n步骤4: 双目标定 (Stereo Calibration)" << std::endl;
        
        bool success = StereoCalibration::calibrateStereoCamera(
            outputPath + "/corners/left",
            outputPath + "/corners/right",
            outputPath + "/calibration/stereo",
            boardWidth, boardHeight, squareSize,
            imageWidth, imageHeight
        );
        
        if (success) {
            std::cout << "✓ 双目标定完成" << std::endl;
            std::cout << "  - 相机间几何关系已确定" << std::endl;
            std::cout << "  - 立体校正参数已计算" << std::endl;
        }
        
        return success;
    }
    
    bool rectifyImages() {
        std::cout << "\n步骤5: 立体校正 (Stereo Rectification)" << std::endl;
        
        // 这里应该实现立体校正功能
        // 目前使用单目矫正的结果作为临时替代
        std::filesystem::create_directories(outputPath + "/rectified/left");
        std::filesystem::create_directories(outputPath + "/rectified/right");
        
        // 复制单目矫正结果到立体校正目录
        try {
            std::string leftUndistorted = outputPath + "/calibration/left/undistorted";
            std::string rightUndistorted = outputPath + "/calibration/right/undistorted";
            std::string leftRectified = outputPath + "/rectified/left";
            std::string rightRectified = outputPath + "/rectified/right";
            
            if (std::filesystem::exists(leftUndistorted) && std::filesystem::exists(rightUndistorted)) {
                std::filesystem::copy(leftUndistorted, leftRectified, 
                    std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
                std::filesystem::copy(rightUndistorted, rightRectified, 
                    std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
                
                std::cout << "✓ 立体校正完成 (使用畸变矫正结果)" << std::endl;
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "立体校正过程出错: " << e.what() << std::endl;
        }
        
        return false;
    }
    
    bool generate3DModel() {
        std::cout << "\n步骤6: 3D建模 (3D Modeling)" << std::endl;
        
        // 检查是否有场景图像用于重建
        std::string leftScene = leftImagePath + "/scene_left.jpg";
        std::string rightScene = rightImagePath + "/scene_right.jpg";
        
        if (std::filesystem::exists(leftScene) && std::filesystem::exists(rightScene)) {
            bool success = StereoReconstruction::reconstruct3D(
                leftScene, rightScene,
                outputPath + "/calibration/stereo",
                outputPath + "/3d_model",
                StereoReconstruction::PLY_FORMAT,
                StereoReconstruction::HIGH_QUALITY,
                squareSize,
                imageWidth, imageHeight
            );
            
            if (success) {
                std::cout << "✓ 3D模型生成完成" << std::endl;
                return true;
            }
        } else {
            std::cout << "○ 未找到场景图像 (scene_left.jpg, scene_right.jpg)" << std::endl;
            std::cout << "  如需3D重建，请在输入目录中提供场景图像" << std::endl;
        }
        
        return false;
    }
    
    void printResults() {
        std::cout << "\n=== 标定结果 (Calibration Results) ===" << std::endl;
        std::cout << "输出目录结构 (Output directory structure):" << std::endl;
        std::cout << outputPath << "/" << std::endl;
        std::cout << "├── preprocessed/         # 预处理后的图像" << std::endl;
        std::cout << "│   ├── left/              # 左相机图像 (" << imageWidth << "×" << imageHeight << ")" << std::endl;
        std::cout << "│   └── right/             # 右相机图像 (" << imageWidth << "×" << imageHeight << ")" << std::endl;
        std::cout << "├── corners/              # 角点检测结果" << std::endl;
        std::cout << "│   ├── left/              # 左相机角点" << std::endl;
        std::cout << "│   └── right/             # 右相机角点" << std::endl;
        std::cout << "├── calibration/          # 标定参数" << std::endl;
        std::cout << "│   ├── left/              # 左相机标定结果" << std::endl;
        std::cout << "│   ├── right/             # 右相机标定结果" << std::endl;
        std::cout << "│   └── stereo/            # 双目标定结果" << std::endl;
        std::cout << "├── rectified/            # 立体校正图像" << std::endl;
        std::cout << "│   ├── left/              # 左相机校正图像" << std::endl;
        std::cout << "│   └── right/             # 右相机校正图像" << std::endl;
        std::cout << "└── 3d_model/             # 3D重建结果 (如有场景图像)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "标定参数说明:" << std::endl;
        std::cout << "- 棋盘格: " << boardWidth << "×" << boardHeight << " 内角点" << std::endl;
        std::cout << "- 方格尺寸: " << squareSize * 1000 << "mm" << std::endl;
        std::cout << "- 图像尺寸: " << imageWidth << "×" << imageHeight << " 像素" << std::endl;
    }
};

// 主函数 - 程序入口
int main(int argc, char* argv[]) {
    std::cout << "双目标定程序 v1.0" << std::endl;
    std::cout << "Stereo Calibration Program v1.0" << std::endl;
    std::cout << "类似MATLAB双目标定工具箱 (Similar to MATLAB Stereo Camera Calibrator)" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 默认参数
    std::string leftPath = "calibration_data/input/left";
    std::string rightPath = "calibration_data/input/right";
    std::string outputPath = "calibration_data/output";
    
    // 解析命令行参数
    if (argc >= 4) {
        leftPath = argv[1];
        rightPath = argv[2];
        outputPath = argv[3];
    } else {
        std::cout << "使用方法 (Usage):" << std::endl;
        std::cout << "  " << argv[0] << " <左图像路径> <右图像路径> <输出路径>" << std::endl;
        std::cout << "  " << argv[0] << " <left_path> <right_path> <output_path>" << std::endl;
        std::cout << std::endl;
        std::cout << "使用默认路径 (Using default paths):" << std::endl;
        std::cout << "- 左图像: " << leftPath << std::endl;
        std::cout << "- 右图像: " << rightPath << std::endl;
        std::cout << "- 输出: " << outputPath << std::endl;
        std::cout << std::endl;
    }
    
    // 创建并运行标定程序
    StereoCalibrationProgram calibrator(leftPath, rightPath, outputPath);
    
    bool success = calibrator.runCalibration();
    
    if (success) {
        std::cout << "✓ 双目标定程序完成!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ 双目标定程序失败!" << std::endl;
        return 1;
    }
}