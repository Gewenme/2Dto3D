#include "stereo_calibration.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace StereoCalibration {

bool calibrateStereoCamera(const std::string& leftCornerDataPath,
                          const std::string& rightCornerDataPath,
                          const std::string& outputPath,
                          int boardWidth,
                          int boardHeight,
                          float squareSize,
                          int imageWidth,
                          int imageHeight) {
    try {
        // Create output directory
        std::filesystem::create_directories(outputPath);

        // Load left corner data
        cv::FileStorage leftFs(leftCornerDataPath + "/corners.yml", cv::FileStorage::READ);
        if (!leftFs.isOpened()) {
            std::cerr << "Failed to open left corner data: " << leftCornerDataPath + "/corners.yml" << std::endl;
            return false;
        }

        std::vector<std::vector<cv::Point2f>> leftImagePoints;
        leftFs["corners"] >> leftImagePoints;
        leftFs.release();

        // Load right corner data
        cv::FileStorage rightFs(rightCornerDataPath + "/corners.yml", cv::FileStorage::READ);
        if (!rightFs.isOpened()) {
            std::cerr << "Failed to open right corner data: " << rightCornerDataPath + "/corners.yml" << std::endl;
            return false;
        }

        std::vector<std::vector<cv::Point2f>> rightImagePoints;
        rightFs["corners"] >> rightImagePoints;
        rightFs.release();

        if (leftImagePoints.empty() || rightImagePoints.empty()) {
            std::cerr << "No corner data found in one or both datasets" << std::endl;
            return false;
        }

        if (leftImagePoints.size() != rightImagePoints.size()) {
            std::cerr << "Left and right corner data sizes don't match" << std::endl;
            return false;
        }

        // Create 3D object points
        std::vector<std::vector<cv::Point3f>> objectPoints;
        cv::Size boardSize(boardWidth, boardHeight);
        
        for (size_t i = 0; i < leftImagePoints.size(); i++) {
            std::vector<cv::Point3f> corners3D;
            for (int y = 0; y < boardHeight; y++) {
                for (int x = 0; x < boardWidth; x++) {
                    corners3D.push_back(cv::Point3f(x * squareSize, y * squareSize, 0));
                }
            }
            objectPoints.push_back(corners3D);
        }

        // Initialize camera matrices and distortion coefficients
        cv::Mat leftCameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat leftDistCoeffs = cv::Mat::zeros(8, 1, CV_64F);
        cv::Mat rightCameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat rightDistCoeffs = cv::Mat::zeros(8, 1, CV_64F);
        
        cv::Size imageSize(imageWidth, imageHeight);

        // Stereo calibration
        cv::Mat R, T, E, F;
        
        double rms = cv::stereoCalibrate(objectPoints, 
                                        leftImagePoints, rightImagePoints,
                                        leftCameraMatrix, leftDistCoeffs,
                                        rightCameraMatrix, rightDistCoeffs,
                                        imageSize, R, T, E, F,
                                        cv::CALIB_FIX_INTRINSIC,
                                        cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5));

        std::cout << "Stereo calibration completed with RMS error: " << rms << std::endl;

        // Save stereo calibration data
        std::string calibFile = outputPath + "/stereo_calibration.yml";
        if (!saveStereoCalibrationData(calibFile, 
                                      leftCameraMatrix, leftDistCoeffs,
                                      rightCameraMatrix, rightDistCoeffs,
                                      R, T, E, F, imageSize, rms)) {
            std::cerr << "Failed to save stereo calibration data" << std::endl;
            return false;
        }

        std::cout << "Stereo calibration data saved to: " << calibFile << std::endl;

        // Compute and save rectification data
        cv::Mat R1, R2, P1, P2, Q;
        cv::Rect leftROI, rightROI;
        
        cv::stereoRectify(leftCameraMatrix, leftDistCoeffs,
                         rightCameraMatrix, rightDistCoeffs,
                         imageSize, R, T, R1, R2, P1, P2, Q,
                         cv::CALIB_ZERO_DISPARITY, 0, imageSize,
                         &leftROI, &rightROI);

        // Save rectification data
        cv::FileStorage rectifyFs(outputPath + "/stereo_rectify.yml", cv::FileStorage::WRITE);
        rectifyFs << "R1" << R1 << "R2" << R2;
        rectifyFs << "P1" << P1 << "P2" << P2;
        rectifyFs << "Q" << Q;
        rectifyFs << "left_roi_x" << leftROI.x << "left_roi_y" << leftROI.y;
        rectifyFs << "left_roi_width" << leftROI.width << "left_roi_height" << leftROI.height;
        rectifyFs << "right_roi_x" << rightROI.x << "right_roi_y" << rightROI.y;
        rectifyFs << "right_roi_width" << rightROI.width << "right_roi_height" << rightROI.height;
        rectifyFs.release();

        std::cout << "Stereo rectification data saved to: " << outputPath + "/stereo_rectify.yml" << std::endl;

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error in stereo calibration: " << e.what() << std::endl;
        return false;
    }
}

bool loadStereoCalibrationData(const std::string& calibrationFile,
                              cv::Mat& leftCameraMatrix, cv::Mat& leftDistCoeffs,
                              cv::Mat& rightCameraMatrix, cv::Mat& rightDistCoeffs,
                              cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F) {
    try {
        cv::FileStorage fs(calibrationFile, cv::FileStorage::READ);
        if (!fs.isOpened()) {
            std::cerr << "Failed to open stereo calibration file: " << calibrationFile << std::endl;
            return false;
        }

        fs["left_camera_matrix"] >> leftCameraMatrix;
        fs["left_distortion_coefficients"] >> leftDistCoeffs;
        fs["right_camera_matrix"] >> rightCameraMatrix;
        fs["right_distortion_coefficients"] >> rightDistCoeffs;
        fs["rotation_matrix"] >> R;
        fs["translation_vector"] >> T;
        fs["essential_matrix"] >> E;
        fs["fundamental_matrix"] >> F;
        
        fs.release();

        return !leftCameraMatrix.empty() && !rightCameraMatrix.empty() && 
               !R.empty() && !T.empty();

    } catch (const std::exception& e) {
        std::cerr << "Error loading stereo calibration data: " << e.what() << std::endl;
        return false;
    }
}

bool saveStereoCalibrationData(const std::string& calibrationFile,
                              const cv::Mat& leftCameraMatrix, const cv::Mat& leftDistCoeffs,
                              const cv::Mat& rightCameraMatrix, const cv::Mat& rightDistCoeffs,
                              const cv::Mat& R, const cv::Mat& T, const cv::Mat& E, const cv::Mat& F,
                              const cv::Size& imageSize, double reprojectionError) {
    try {
        cv::FileStorage fs(calibrationFile, cv::FileStorage::WRITE);
        if (!fs.isOpened()) {
            std::cerr << "Failed to create stereo calibration file: " << calibrationFile << std::endl;
            return false;
        }

        fs << "left_camera_matrix" << leftCameraMatrix;
        fs << "left_distortion_coefficients" << leftDistCoeffs;
        fs << "right_camera_matrix" << rightCameraMatrix;
        fs << "right_distortion_coefficients" << rightDistCoeffs;
        fs << "rotation_matrix" << R;
        fs << "translation_vector" << T;
        fs << "essential_matrix" << E;
        fs << "fundamental_matrix" << F;
        fs << "image_width" << imageSize.width;
        fs << "image_height" << imageSize.height;
        fs << "reprojection_error" << reprojectionError;
        
        // Save additional info
        time_t rawtime;
        time(&rawtime);
        fs << "calibration_time" << asctime(localtime(&rawtime));

        fs.release();
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving stereo calibration data: " << e.what() << std::endl;
        return false;
    }
}

bool rectifyStereoPair(const cv::Mat& leftImage, const cv::Mat& rightImage,
                      cv::Mat& rectifiedLeft, cv::Mat& rectifiedRight,
                      const std::string& calibrationFile) {
    try {
        // Load calibration data
        cv::Mat leftCameraMatrix, leftDistCoeffs, rightCameraMatrix, rightDistCoeffs;
        cv::Mat R, T, E, F;
        
        if (!loadStereoCalibrationData(calibrationFile,
                                      leftCameraMatrix, leftDistCoeffs,
                                      rightCameraMatrix, rightDistCoeffs,
                                      R, T, E, F)) {
            return false;
        }

        // Load rectification data
        std::string rectifyFile = calibrationFile.substr(0, calibrationFile.find_last_of("/\\") + 1) + "stereo_rectify.yml";
        cv::FileStorage rectifyFs(rectifyFile, cv::FileStorage::READ);
        if (!rectifyFs.isOpened()) {
            std::cerr << "Failed to open rectification file: " << rectifyFile << std::endl;
            return false;
        }

        cv::Mat R1, R2, P1, P2;
        rectifyFs["R1"] >> R1;
        rectifyFs["R2"] >> R2;
        rectifyFs["P1"] >> P1;
        rectifyFs["P2"] >> P2;
        rectifyFs.release();

        // Compute rectification maps
        cv::Mat leftMapX, leftMapY, rightMapX, rightMapY;
        cv::initUndistortRectifyMap(leftCameraMatrix, leftDistCoeffs, R1, P1,
                                   leftImage.size(), CV_16SC2, leftMapX, leftMapY);
        cv::initUndistortRectifyMap(rightCameraMatrix, rightDistCoeffs, R2, P2,
                                   rightImage.size(), CV_16SC2, rightMapX, rightMapY);

        // Rectify images
        cv::remap(leftImage, rectifiedLeft, leftMapX, leftMapY, cv::INTER_LINEAR);
        cv::remap(rightImage, rectifiedRight, rightMapX, rightMapY, cv::INTER_LINEAR);

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error rectifying stereo pair: " << e.what() << std::endl;
        return false;
    }
}

} // namespace StereoCalibration