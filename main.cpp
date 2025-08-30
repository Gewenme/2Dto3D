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
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/picture_new/point_pic/left",//左图像文件夹本地地址，已存在
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize/camL_resized",//左图像文件夹输出地址，不存在会自动创建，存在则存入
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
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/picture_new/point_pic/right",//同理
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize/camR_resized",//同理
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
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize/camL_resized",//左图像文件夹本地地址，已存在
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/left_jiaodian",//左图像文件夹输出地址，不存在会自动创建，存在则存入
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
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize/camR_resized",//同理
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/right_jiaodian",  //同理
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
    bool success1 = MonoCalibration::calibrateCamera(
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/left_jiaodian/corner_data",//左图像文件夹本地地址，已存在
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step1_imagresize//camL_resized",//等待矫正的图像文件夹地址，用step1的图像
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step3_biaoding/left_calibration",//左图像标定参数文件输出地址，不存在会自动创建，存在则存入
        9,
        6,
        0.0082f,
        3264,
        2448,
        false,
        "  " // 单目标定输出校正后的图像地址，不存在会自动创建，存在则存入
    );

    if (success1) {
        std::cout << "左相机标定成功!" << std::endl;
        );
    } else {
        std::cerr << "左相机标定失败!" << std::endl;
        return -1;
    }
	//同理右相机标定
    
    //。。。。。。。






    //Step 4: 双目标定
	bool success3 = StereoCalibration::calibrateStereoCamera(
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/left_jiaodian/corner_data",//左图像文件夹本地地址，已存在
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step2_jiancejiaodian/right_jiaodian/corner_data",//右图像文件夹本地地址，已存在
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step4_shuangmu_biaoding/stereo_calibration",//双目标定参数文件输出地址，不存在会自动创建，存在则存入
        9,
        6,
        0.0082f,
        3264,
        2448
	);
    if (success3) 
    {
        std::cout << "双目标定成功!" << std::endl;
        
    } else {
        std::cerr << "双目标定失败!" << std::endl;
        return -1;
    }




    //Step 5: 三维重建
    bool success4 = StereoReconstruction::reconstruct3D(
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/picture_new/build_pic/left/6.jpg",//左图像文件夹本地地址，已存在
		"G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/picture_new/build_pic/right/6.jpg",//右图像文件夹本地地址，已存在
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step4_shuangmu_biaoding/stereo_calibration",//双目标定参数文件输出地址，不存在会自动创建，存在则存入
        "G:/MyAll/githubcode/recreat3D/tempcode/p2_3D/steps3/step5_3D_reconstruction/3D_reconstruction",//三维重建结果输出地址，不存在会自动创建，存在则存入
		0, //PLY格式
		3, //中等质量
        0.0082f //棋盘格的实际尺寸,单位为米
        3264,// 图像宽度
		2448// 图像高度
	);
    if (success4) {
        std::cout << "三维重建成功!" << std::endl;
    } else {
        std::cerr << "三维重建失败!" << std::endl;
        return -1;
    }
    //Step 6: 三维模型查看
    


	return 0;




























}