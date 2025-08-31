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

        // Generate residual error images
        std::string residualPath = outputPath + "/residual_images";
        if (generateResidualImages(cornerDataPath, imageFolderPath, residualPath,
                                  cameraMatrix, distCoeffs, objectPoints, imagePoints, rvecs, tvecs)) {
            std::cout << "Residual error images saved to: " << residualPath << std::endl;
        }

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

bool generateResidualImages(const std::string& cornerDataPath,
                           const std::string& imageFolderPath,
                           const std::string& outputPath,
                           const cv::Mat& cameraMatrix,
                           const cv::Mat& distCoeffs,
                           const std::vector<std::vector<cv::Point3f>>& objectPoints,
                           const std::vector<std::vector<cv::Point2f>>& imagePoints,
                           const std::vector<cv::Mat>& rvecs,
                           const std::vector<cv::Mat>& tvecs) {
    try {
        // Create output directory
        std::filesystem::create_directories(outputPath);
        
        if (objectPoints.size() != imagePoints.size() || 
            objectPoints.size() != rvecs.size() || 
            objectPoints.size() != tvecs.size()) {
            std::cerr << "Mismatch in calibration data sizes" << std::endl;
            return false;
        }

        int residualCount = 0;
        double totalError = 0.0;

        // Process each calibration image
        for (size_t imgIdx = 0; imgIdx < objectPoints.size(); imgIdx++) {
            // Project 3D points to 2D using calibration parameters
            std::vector<cv::Point2f> projectedPoints;
            cv::projectPoints(objectPoints[imgIdx], rvecs[imgIdx], tvecs[imgIdx],
                            cameraMatrix, distCoeffs, projectedPoints);

            // Calculate reprojection errors for this image
            std::vector<float> errors;
            float maxError = 0.0f;
            for (size_t ptIdx = 0; ptIdx < imagePoints[imgIdx].size(); ptIdx++) {
                cv::Point2f diff = imagePoints[imgIdx][ptIdx] - projectedPoints[ptIdx];
                float error = sqrt(diff.x * diff.x + diff.y * diff.y);
                errors.push_back(error);
                maxError = std::max(maxError, error);
                totalError += error;
            }

            // Load corresponding original image
            std::string imageName = "img_" + std::to_string(imgIdx) + ".jpg";
            std::string imagePath;
            
            // Try to find the corresponding image file
            for (const auto& entry : std::filesystem::directory_iterator(imageFolderPath)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find(std::to_string(imgIdx)) != std::string::npos ||
                        imgIdx == 0) { // Use first available image as fallback
                        imagePath = entry.path().string();
                        break;
                    }
                }
            }

            if (imagePath.empty()) {
                // Fallback: use first available image
                for (const auto& entry : std::filesystem::directory_iterator(imageFolderPath)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                            imagePath = entry.path().string();
                            break;
                        }
                    }
                }
            }

            if (imagePath.empty()) {
                std::cerr << "No image found for index " << imgIdx << std::endl;
                continue;
            }

            cv::Mat image = cv::imread(imagePath);
            if (image.empty()) {
                std::cerr << "Failed to load image: " << imagePath << std::endl;
                continue;
            }

            // Create residual error visualization
            cv::Mat errorVis = image.clone();

            // Draw detected corners in green
            for (size_t ptIdx = 0; ptIdx < imagePoints[imgIdx].size(); ptIdx++) {
                cv::circle(errorVis, imagePoints[imgIdx][ptIdx], 3, cv::Scalar(0, 255, 0), -1);
            }

            // Draw projected corners in red
            for (size_t ptIdx = 0; ptIdx < projectedPoints.size(); ptIdx++) {
                cv::circle(errorVis, projectedPoints[ptIdx], 3, cv::Scalar(0, 0, 255), -1);
            }

            // Draw error vectors
            for (size_t ptIdx = 0; ptIdx < imagePoints[imgIdx].size(); ptIdx++) {
                cv::line(errorVis, imagePoints[imgIdx][ptIdx], projectedPoints[ptIdx], 
                        cv::Scalar(255, 0, 255), 1);
                
                // Color code the error magnitude
                float normalizedError = errors[ptIdx] / maxError;
                cv::Scalar errorColor(0, 255 * (1 - normalizedError), 255 * normalizedError);
                cv::circle(errorVis, imagePoints[imgIdx][ptIdx], 5, errorColor, 2);
            }

            // Add text information
            cv::putText(errorVis, "Green: Detected, Red: Projected", 
                       cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            
            float avgError = 0;
            for (float err : errors) avgError += err;
            avgError /= errors.size();
            
            std::string errorText = "Avg Error: " + std::to_string(avgError) + " px";
            cv::putText(errorVis, errorText, cv::Point(10, 60), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

            // Save residual image
            std::string outputFile = outputPath + "/residual_" + std::to_string(imgIdx) + ".jpg";
            cv::imwrite(outputFile, errorVis);
            residualCount++;
        }

        std::cout << "Generated " << residualCount << " residual error images" << std::endl;
        std::cout << "Average reprojection error: " << (totalError / (objectPoints.size() * objectPoints[0].size())) << " pixels" << std::endl;

        return residualCount > 0;

    } catch (const std::exception& e) {
        std::cerr << "Error generating residual images: " << e.what() << std::endl;
        return false;
    }
}

} // namespace MonoCalibration