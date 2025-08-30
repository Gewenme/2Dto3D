#include "image_resize.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "corner_detection.h"
#include "model_viewer.h"

#include <iostream>
#include <fstream>

int main() {

    bool success1 = ImageUtils::resizeImage(
		"picture/left",//左图像文件夹本地地址，已存在
		"output/step1_imagresize/camL_resized",//左图像文件夹输出地址，不存在会自动创建，存在则存入
		3264, 2448, 2  //代表将图片统一为3264*2448像素，2代表双线性插值

    );

    if (success1) {
        std::cout << "左图像文件夹处理成功!" << std::endl;
    }
    else {
        std::cerr << "左图像文件夹处理失败!" << std::endl;
        return -1;
    }

    ImageUtils::ResizeOptions rightOptions(ImageUtils::LINEAR);  // 对右图像使用相同的设置
    rightOptions.enableSharpening = false;
    rightOptions.reduceNoise = false;
    rightOptions.preserveAspectRatio = true;

    bool success2 = ImageUtils::resizeImage(
        "picture/right",//同理
        "output/step1_imagresize/camR_resized",//同理
		3264, 2448, 2 //同理
    );

    if (success2) {
        std::cout << "右图像文件夹处理成功!" << std::endl;
    }
    else {
        std::cerr << "右图像文件夹处理失败!" << std::endl;
        return -1;
    }



     //Step 2: 角点检测与绘制
    bool success3 = detectAndDrawCorners(
        "output/step1_imagresize/camL_resized",//左图像文件夹本地地址，已存在
        "output/step2_jiancejiaodian/left_jiaodian",//左图像文件夹输出地址，不存在会自动创建，存在则存入
        9,    // 棋盘内角点数
        6,    // 棋盘内角点数
		1.0f  // 图像放大比例因子，以防图像太小检测不到角点
    );

    if (success3) {
        std::cout << "左相机角点检测完成!" << std::endl;
    }
    else {
        std::cerr << "左相机角点检测失败!" << std::endl;
        return -1;
    }

    bool success4 =detectAndDrawCorners(
        "output/step1_imagresize/camR_resized",//同理
		"output/step2_jiancejiaodian/right_jiaodian",  //同理
        9,    // 棋盘内角点数
        6,    // 棋盘内角点数
		1.0f  // 图像放大比例因子，以防图像太小检测不到角点
    );

    if (success4) {
        std::cout << "右相机角点检测完成!" << std::endl;
    }
    else {
        std::cerr << "右相机角点检测失败!" << std::endl;
        return -1;
    }


     //Step 3: 单目标定
    bool success5 = MonoCalibration::calibrateCamera(
        "output/step2_jiancejiaodian/left_jiaodian/corner_data",//左图像文件夹本地地址，已存在
		"output/step1_imagresize/camL_resized",//等待矫正的图像文件夹地址，用step1的图像
		"output/step3_biaoding/left_calibration",//左图像标定参数文件输出地址，不存在会自动创建，存在则存入
        9,
        6,
        0.0082f,
        3264,
        2448,
        false,
        "" // 单目标定输出校正后的图像地址，不存在会自动创建，存在则存入
    );

    if (success5) {
        std::cout << "左相机标定成功!" << std::endl;
    } else {
        std::cerr << "左相机标定失败!" << std::endl;
        return -1;
    }
    
	//Step 3.2: 右相机单目标定
    bool success5_2 = MonoCalibration::calibrateCamera(
        "output/step2_jiancejiaodian/right_jiaodian/corner_data",//右图像文件夹本地地址，已存在
		"output/step1_imagresize/camR_resized",//等待矫正的图像文件夹地址，用step1的图像
		"output/step3_biaoding/right_calibration",//右图像标定参数文件输出地址，不存在会自动创建，存在则存入
        9,
        6,
        0.0082f,
        3264,
        2448,
        false,
        "" // 单目标定输出校正后的图像地址，不存在会自动创建，存在则存入
    );

    if (success5_2) {
        std::cout << "右相机标定成功!" << std::endl;
    } else {
        std::cerr << "右相机标定失败!" << std::endl;
        return -1;
    }






    //Step 4: 双目标定
	bool success6 = StereoCalibration::calibrateStereoCamera(
        "output/step2_jiancejiaodian/left_jiaodian/corner_data",//左图像文件夹本地地址，已存在
		"output/step2_jiancejiaodian/right_jiaodian/corner_data",//右图像文件夹本地地址，已存在
        "output/step4_shuangmu_biaoding/stereo_calibration",//双目标定参数文件输出地址，不存在会自动创建，存在则存入
        9,
        6,
        0.0082f,
        3264,
        2448
	);
    if (success6) 
    {
        std::cout << "双目标定成功!" << std::endl;
        
    } else {
        std::cerr << "双目标定失败!" << std::endl;
        return -1;
    }




    //Step 5: 三维重建
    bool success7 = StereoReconstruction::reconstruct3D(
		"picture/scene_left.jpg",//左图像文件夹本地地址，已存在
		"picture/scene_right.jpg",//右图像文件夹本地地址，已存在
        "output/step4_shuangmu_biaoding/stereo_calibration",//双目标定参数文件输出地址，不存在会自动创建，存在则存入
        "output/step5_3D_reconstruction/3D_reconstruction",//三维重建结果输出地址，不存在会自动创建，存在则存入
		0, //PLY格式
		1, //高质量
        0.0082f, //棋盘格的实际尺寸,单位为米
        3264,// 图像宽度
		2448// 图像高度
	);
    if (success7) {
        std::cout << "三维重建成功!" << std::endl;
        
        //Step 6: 三维模型查看和可视化
        std::vector<cv::Point3f> points;
        std::vector<cv::Vec3b> colors;
        
        // 加载生成的点云模型
        bool loadSuccess = ModelViewer::loadModel("output/step5_3D_reconstruction/3D_reconstruction/reconstruction.ply", points, colors);
        if (loadSuccess && !points.empty()) {
            std::cout << "成功加载点云模型，包含 " << points.size() << " 个点" << std::endl;
            
            // 显示3D模型
            bool displaySuccess = ModelViewer::displayModel(points, colors, "3D Reconstruction Result");
            if (displaySuccess) {
                std::cout << "3D模型显示成功!" << std::endl;
                
                // 生成模型统计信息
                ModelViewer::saveModelStatistics(points, "output/step5_3D_reconstruction/model_statistics.txt");
                
                // 生成投影视图
                ModelViewer::generateProjectionViews(points, colors, "output/step5_3D_reconstruction/projection_views");
                
                std::cout << "实验完成！所有步骤成功执行。" << std::endl;
            } else {
                std::cerr << "3D模型显示失败!" << std::endl;
                return -1;
            }
        } else {
            std::cerr << "无法加载点云模型!" << std::endl;
            return -1;
        }
        
    } else {
        std::cerr << "三维重建失败!" << std::endl;
        
        // 尝试重新建模，使用不同参数
        std::cout << "尝试重新建模，使用中等质量参数..." << std::endl;
        bool retry_success = StereoReconstruction::reconstruct3D(
            "picture/scene_left.jpg",
            "picture/scene_right.jpg", 
            "output/step4_shuangmu_biaoding/stereo_calibration",
            "output/step5_3D_reconstruction/3D_reconstruction_retry",
            0, //PLY格式
            2, //中等质量
            0.0082f,
            3264,
            2448
        );
        
        if (retry_success) {
            std::cout << "重新建模成功！" << std::endl;
            
            // 尝试显示重试的模型
            std::vector<cv::Point3f> retry_points;
            std::vector<cv::Vec3b> retry_colors;
            
            bool retry_load = ModelViewer::loadModel("output/step5_3D_reconstruction/3D_reconstruction_retry/reconstruction.ply", retry_points, retry_colors);
            if (retry_load && !retry_points.empty()) {
                ModelViewer::displayModel(retry_points, retry_colors, "3D Reconstruction Result (Retry)");
                ModelViewer::saveModelStatistics(retry_points, "output/step5_3D_reconstruction/model_statistics_retry.txt");
                std::cout << "重试实验完成！" << std::endl;
            }
        } else {
            std::cerr << "重新建模也失败!" << std::endl;
            return -1;
        }
    }
    


    
    // 生成实验文档报告
    std::ofstream report("output/experiment_report.md");
    if (report.is_open()) {
        report << "# 2D到3D重建实验报告\n\n";
        report << "## 实验概述\n";
        report << "本实验使用立体视觉技术，基于12组标定图像对和一对目标场景图像，完成了从2D图像到3D点云模型的重建过程。\n\n";
        report << "## 实验步骤\n";
        report << "1. **图像预处理**: " << (success1 && success2 ? "成功" : "失败") << "\n";
        report << "2. **角点检测**: " << (success3 && success4 ? "成功" : "失败") << "\n";
        report << "3. **单目标定**: " << (success5 && success5_2 ? "成功" : "失败") << "\n";
        report << "4. **双目标定**: " << (success6 ? "成功" : "失败") << "\n";
        report << "5. **三维重建**: " << (success7 ? "成功" : "失败") << "\n";
        report << "6. **模型显示**: " << (success7 ? "成功" : "失败") << "\n\n";
        report << "## 输出文件\n";
        report << "- 标定参数: `output/step4_shuangmu_biaoding/stereo_calibration/`\n";
        report << "- 3D点云模型: `output/step5_3D_reconstruction/3D_reconstruction/reconstruction.ply`\n";
        report << "- 模型统计: `output/step5_3D_reconstruction/model_statistics.txt`\n";
        report << "- 投影视图: `output/step5_3D_reconstruction/projection_views/`\n\n";
        report << "## 实验结果\n";
        if (success7) {
            report << "实验成功完成，生成了完整的3D点云模型并进行了可视化显示。\n";
        } else {
            report << "实验过程中遇到问题，已尝试重新建模。\n";
        }
        report.close();
        std::cout << "实验报告已生成: output/experiment_report.md" << std::endl;
    }

	return 0;




























}