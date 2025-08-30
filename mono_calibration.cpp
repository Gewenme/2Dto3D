#include "mono_calibration.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace MonoCalibration {

bool calibrateCamera(const std::string& cornerDataPath,
                     const std::string& imageFolderPath,
                     const std::string& outputPath,
                     int boardWidth,
                     int boardHeight,
                     float squareSize,
                     int imageWidth,
                     int imageHeight,
                     bool saveUndistorted,
                     const std::string& undistortedPath) {
    try {
        // Create output directories
        std::filesystem::create_directories(outputPath);
        if (saveUndistorted && !undistortedPath.empty()) {
            std::filesystem::create_directories(undistortedPath);
        }

        // Load corner data
        cv::FileStorage fs(cornerDataPath + "/corners.yml", cv::FileStorage::READ);
        if (!fs.isOpened()) {
            std::cerr << "Failed to open corner data file: " << cornerDataPath + "/corners.yml" << std::endl;
            return false;
        }

        std::vector<std::vector<cv::Point2f>> imagePoints;
        fs["corners"] >> imagePoints;
        fs.release();

        if (imagePoints.empty()) {
            std::cerr << "No corner data found" << std::endl;
            return false;
        }

        // Create 3D object points
        std::vector<std::vector<cv::Point3f>> objectPoints;
        cv::Size boardSize(boardWidth, boardHeight);
        
        for (size_t i = 0; i < imagePoints.size(); i++) {
            std::vector<cv::Point3f> corners3D;
            for (int y = 0; y < boardHeight; y++) {
                for (int x = 0; x < boardWidth; x++) {
                    corners3D.push_back(cv::Point3f(x * squareSize, y * squareSize, 0));
                }
            }
            objectPoints.push_back(corners3D);
        }

        // Camera calibration
        cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
        std::vector<cv::Mat> rvecs, tvecs;
        cv::Size imageSize(imageWidth, imageHeight);

        double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize,
                                        cameraMatrix, distCoeffs, rvecs, tvecs);

        std::cout << "Camera calibration completed with RMS error: " << rms << std::endl;

        // Save calibration data
        std::string calibFile = outputPath + "/camera_calibration.yml";
        if (!saveCalibrationData(calibFile, cameraMatrix, distCoeffs, imageSize, rms)) {
            std::cerr << "Failed to save calibration data" << std::endl;
            return false;
        }

        std::cout << "Calibration data saved to: " << calibFile << std::endl;

        // Save undistorted images if requested
        if (saveUndistorted && !undistortedPath.empty()) {
            int undistortedCount = 0;
            for (const auto& entry : std::filesystem::directory_iterator(imageFolderPath)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tiff") {
                        cv::Mat image = cv::imread(entry.path().string());
                        if (!image.empty()) {
                            cv::Mat undistorted;
                            if (undistortImage(image, undistorted, cameraMatrix, distCoeffs)) {
                                std::string outputFile = undistortedPath + "/" + entry.path().filename().string();
                                cv::imwrite(outputFile, undistorted);
                                undistortedCount++;
                            }
                        }
                    }
                }
            }
            std::cout << "Saved " << undistortedCount << " undistorted images to: " << undistortedPath << std::endl;
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error in camera calibration: " << e.what() << std::endl;
        return false;
    }
}

bool loadCalibrationData(const std::string& calibrationFile,
                        cv::Mat& cameraMatrix,
                        cv::Mat& distCoeffs) {
    try {
        cv::FileStorage fs(calibrationFile, cv::FileStorage::READ);
        if (!fs.isOpened()) {
            std::cerr << "Failed to open calibration file: " << calibrationFile << std::endl;
            return false;
        }

        fs["camera_matrix"] >> cameraMatrix;
        fs["distortion_coefficients"] >> distCoeffs;
        fs.release();

        return !cameraMatrix.empty() && !distCoeffs.empty();

    } catch (const std::exception& e) {
        std::cerr << "Error loading calibration data: " << e.what() << std::endl;
        return false;
    }
}

bool saveCalibrationData(const std::string& calibrationFile,
                        const cv::Mat& cameraMatrix,
                        const cv::Mat& distCoeffs,
                        const cv::Size& imageSize,
                        double reprojectionError) {
    try {
        cv::FileStorage fs(calibrationFile, cv::FileStorage::WRITE);
        if (!fs.isOpened()) {
            std::cerr << "Failed to create calibration file: " << calibrationFile << std::endl;
            return false;
        }

        fs << "camera_matrix" << cameraMatrix;
        fs << "distortion_coefficients" << distCoeffs;
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
        std::cerr << "Error saving calibration data: " << e.what() << std::endl;
        return false;
    }
}

bool undistortImage(const cv::Mat& inputImage,
                    cv::Mat& outputImage,
                    const cv::Mat& cameraMatrix,
                    const cv::Mat& distCoeffs) {
    try {
        if (inputImage.empty() || cameraMatrix.empty() || distCoeffs.empty()) {
            return false;
        }

        cv::undistort(inputImage, outputImage, cameraMatrix, distCoeffs);
        return !outputImage.empty();

    } catch (const std::exception& e) {
        std::cerr << "Error undistorting image: " << e.what() << std::endl;
        return false;
    }
}

} // namespace MonoCalibration