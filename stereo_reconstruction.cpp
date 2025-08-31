#include "stereo_reconstruction.h"
#include "stereo_calibration.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <cfloat>

namespace StereoReconstruction {

bool reconstruct3D(const std::string& leftImagePath,
                  const std::string& rightImagePath,
                  const std::string& calibrationPath,
                  const std::string& outputPath,
                  int outputFormat,
                  int quality,
                  float squareSize,
                  int imageWidth,
                  int imageHeight) {
    try {
        // Create output directory
        std::filesystem::create_directories(outputPath);

        // Load images
        cv::Mat leftImage = cv::imread(leftImagePath);
        cv::Mat rightImage = cv::imread(rightImagePath);

        if (leftImage.empty() || rightImage.empty()) {
            std::cerr << "Failed to load stereo images" << std::endl;
            return false;
        }

        std::cout << "Loaded stereo images: " << leftImage.size() << std::endl;

        // Load stereo calibration data
        std::string calibFile = calibrationPath + "/stereo_calibration.yml";
        cv::Mat leftCameraMatrix, leftDistCoeffs, rightCameraMatrix, rightDistCoeffs;
        cv::Mat R, T, E, F;

        if (!StereoCalibration::loadStereoCalibrationData(calibFile,
                                                         leftCameraMatrix, leftDistCoeffs,
                                                         rightCameraMatrix, rightDistCoeffs,
                                                         R, T, E, F)) {
            std::cerr << "Failed to load stereo calibration data from: " << calibFile << std::endl;
            return false;
        }

        // Rectify stereo pair
        cv::Mat rectifiedLeft, rectifiedRight;
        if (!StereoCalibration::rectifyStereoPair(leftImage, rightImage,
                                                 rectifiedLeft, rectifiedRight,
                                                 calibFile)) {
            std::cerr << "Failed to rectify stereo pair" << std::endl;
            return false;
        }

        std::cout << "Stereo rectification completed" << std::endl;

        // Save rectified images for debugging
        cv::imwrite(outputPath + "/rectified_left.jpg", rectifiedLeft);
        cv::imwrite(outputPath + "/rectified_right.jpg", rectifiedRight);

        // Compute disparity map
        cv::Mat disparityMap;
        if (!computeDisparityMap(rectifiedLeft, rectifiedRight, disparityMap, quality)) {
            std::cerr << "Failed to compute disparity map" << std::endl;
            return false;
        }

        std::cout << "Disparity map computed" << std::endl;

        // Save disparity map for debugging
        cv::Mat disparityVis;
        cv::normalize(disparityMap, disparityVis, 0, 255, cv::NORM_MINMAX, CV_8U);
        cv::imwrite(outputPath + "/disparity_map.jpg", disparityVis);

        // Load Q matrix for 3D reconstruction
        cv::FileStorage rectifyFs(calibrationPath + "/stereo_rectify.yml", cv::FileStorage::READ);
        cv::Mat Q;
        if (rectifyFs.isOpened()) {
            rectifyFs["Q"] >> Q;
            rectifyFs.release();
        } else {
            std::cerr << "Failed to load Q matrix from rectification data" << std::endl;
            return false;
        }

        // Generate 3D point cloud
        std::vector<cv::Point3f> pointCloud;
        std::vector<cv::Vec3b> colors;
        if (!generatePointCloud(disparityMap, rectifiedLeft, Q, pointCloud, colors)) {
            std::cerr << "Failed to generate point cloud" << std::endl;
            return false;
        }

        std::cout << "Generated point cloud with " << pointCloud.size() << " points" << std::endl;

        // Debug: Print some sample points to understand the scale
        if (!pointCloud.empty()) {
            std::cout << "Sample points (first 5):" << std::endl;
            for (size_t i = 0; i < std::min((size_t)5, pointCloud.size()); i++) {
                std::cout << "  Point " << i << ": (" << pointCloud[i].x << ", " 
                          << pointCloud[i].y << ", " << pointCloud[i].z << ")" << std::endl;
            }
            
            // Auto-scale coordinates to a more reasonable range for visualization
            // Find the range of coordinates
            cv::Point3f minP(FLT_MAX, FLT_MAX, FLT_MAX);
            cv::Point3f maxP(-FLT_MAX, -FLT_MAX, -FLT_MAX);
            
            for (const auto& point : pointCloud) {
                if (std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z)) {
                    minP.x = std::min(minP.x, point.x);
                    minP.y = std::min(minP.y, point.y);
                    minP.z = std::min(minP.z, point.z);
                    maxP.x = std::max(maxP.x, point.x);
                    maxP.y = std::max(maxP.y, point.y);
                    maxP.z = std::max(maxP.z, point.z);
                }
            }
            
            float rangeX = maxP.x - minP.x;
            float rangeY = maxP.y - minP.y;
            float rangeZ = maxP.z - minP.z;
            float maxRange = std::max({rangeX, rangeY, rangeZ});
            
            if (maxRange > 0 && maxRange < 1.0f) {  // If coordinates need scaling
                float scaleFactor = 10.0f / maxRange;  // Scale to ~10 unit range
                std::cout << "Scaling coordinates by factor: " << scaleFactor << std::endl;
                std::cout << "Original range - X:" << rangeX << " Y:" << rangeY << " Z:" << rangeZ << std::endl;
                
                for (auto& point : pointCloud) {
                    point.x *= scaleFactor;
                    point.y *= scaleFactor;
                    point.z *= scaleFactor;
                }
                
                std::cout << "After scaling - Sample points (first 5):" << std::endl;
                for (size_t i = 0; i < std::min((size_t)5, pointCloud.size()); i++) {
                    std::cout << "  Point " << i << ": (" << pointCloud[i].x << ", " 
                              << pointCloud[i].y << ", " << pointCloud[i].z << ")" << std::endl;
                }
            }
        }

        // Filter point cloud with more permissive parameters
        int remainingPoints = filterPointCloud(pointCloud, colors, 10000.0f); // Very permissive
        std::cout << "Filtered point cloud: " << remainingPoints << " points remaining" << std::endl;

        // Save point cloud
        std::string outputFile;
        switch (outputFormat) {
            case OBJ_FORMAT:
                outputFile = outputPath + "/reconstruction.obj";
                break;
            case XYZ_FORMAT:
                outputFile = outputPath + "/reconstruction.xyz";
                break;
            case PLY_FORMAT:
            default:
                outputFile = outputPath + "/reconstruction.ply";
                break;
        }

        if (!savePointCloud(pointCloud, colors, outputFile, outputFormat)) {
            std::cerr << "Failed to save point cloud" << std::endl;
            return false;
        }

        std::cout << "3D reconstruction saved to: " << outputFile << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error in 3D reconstruction: " << e.what() << std::endl;
        return false;
    }
}

bool computeDisparityMap(const cv::Mat& leftImage,
                        const cv::Mat& rightImage,
                        cv::Mat& disparityMap,
                        int quality) {
    try {
        cv::Mat leftGray, rightGray;
        cv::cvtColor(leftImage, leftGray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(rightImage, rightGray, cv::COLOR_BGR2GRAY);

        // Create stereo matcher based on quality
        cv::Ptr<cv::StereoMatcher> stereo;
        
        if (quality == HIGH_QUALITY) {
            // Use StereoSGBM for higher quality
            stereo = cv::StereoSGBM::create(
                0,       // minDisparity
                128,     // numDisparities
                5,       // blockSize
                600,     // P1
                2400,    // P2
                10,      // disp12MaxDiff
                16,      // preFilterCap
                1,       // uniquenessRatio
                100,     // speckleWindowSize
                2        // speckleRange
            );
        } else {
            // Use StereoBM for faster processing
            int blockSize = (quality == MEDIUM_QUALITY) ? 15 : 21;
            stereo = cv::StereoBM::create(128, blockSize);
        }

        // Compute disparity
        stereo->compute(leftGray, rightGray, disparityMap);
        
        // Convert to proper format
        disparityMap.convertTo(disparityMap, CV_32F, 1.0/16.0);

        return !disparityMap.empty();

    } catch (const std::exception& e) {
        std::cerr << "Error computing disparity map: " << e.what() << std::endl;
        return false;
    }
}

bool generatePointCloud(const cv::Mat& disparityMap,
                       const cv::Mat& leftImage,
                       const cv::Mat& Q,
                       std::vector<cv::Point3f>& pointCloud,
                       std::vector<cv::Vec3b>& colors) {
    try {
        pointCloud.clear();
        colors.clear();

        for (int y = 0; y < disparityMap.rows; y++) {
            for (int x = 0; x < disparityMap.cols; x++) {
                float disparity = disparityMap.at<float>(y, x);
                
                if (disparity > 0) {  // Valid disparity
                    // Convert to 3D point
                    cv::Mat point3D_h = Q * (cv::Mat_<double>(4,1) << x, y, disparity, 1);
                    cv::Point3f point3D;
                    point3D.x = static_cast<float>(point3D_h.at<double>(0,0) / point3D_h.at<double>(3,0));
                    point3D.y = static_cast<float>(point3D_h.at<double>(1,0) / point3D_h.at<double>(3,0));
                    point3D.z = static_cast<float>(point3D_h.at<double>(2,0) / point3D_h.at<double>(3,0));

                    // Get color from left image
                    cv::Vec3b color = leftImage.at<cv::Vec3b>(y, x);

                    pointCloud.push_back(point3D);
                    colors.push_back(color);
                }
            }
        }

        return !pointCloud.empty();

    } catch (const std::exception& e) {
        std::cerr << "Error generating point cloud: " << e.what() << std::endl;
        return false;
    }
}

bool savePointCloud(const std::vector<cv::Point3f>& pointCloud,
                   const std::vector<cv::Vec3b>& colors,
                   const std::string& filename,
                   int format) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to create output file: " << filename << std::endl;
            return false;
        }

        if (format == PLY_FORMAT) {
            // PLY format
            file << "ply" << std::endl;
            file << "format ascii 1.0" << std::endl;
            file << "element vertex " << pointCloud.size() << std::endl;
            file << "property float x" << std::endl;
            file << "property float y" << std::endl;
            file << "property float z" << std::endl;
            file << "property uchar red" << std::endl;
            file << "property uchar green" << std::endl;
            file << "property uchar blue" << std::endl;
            file << "end_header" << std::endl;

            for (size_t i = 0; i < pointCloud.size(); i++) {
                file << pointCloud[i].x << " " << pointCloud[i].y << " " << pointCloud[i].z << " ";
                file << static_cast<int>(colors[i][2]) << " " << static_cast<int>(colors[i][1]) << " " << static_cast<int>(colors[i][0]) << std::endl;
            }
        } else if (format == XYZ_FORMAT) {
            // XYZ format
            for (size_t i = 0; i < pointCloud.size(); i++) {
                file << pointCloud[i].x << " " << pointCloud[i].y << " " << pointCloud[i].z << std::endl;
            }
        } else if (format == OBJ_FORMAT) {
            // OBJ format (vertices only)
            for (size_t i = 0; i < pointCloud.size(); i++) {
                file << "v " << pointCloud[i].x << " " << pointCloud[i].y << " " << pointCloud[i].z << std::endl;
            }
        }

        file.close();
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving point cloud: " << e.what() << std::endl;
        return false;
    }
}

int filterPointCloud(std::vector<cv::Point3f>& pointCloud,
                    std::vector<cv::Vec3b>& colors,
                    float maxDistance) {
    try {
        std::vector<cv::Point3f> filteredPoints;
        std::vector<cv::Vec3b> filteredColors;

        for (size_t i = 0; i < pointCloud.size(); i++) {
            cv::Point3f& point = pointCloud[i];
            float distance = std::sqrt(point.x * point.x + point.y * point.y + point.z * point.z);
            
            // More permissive filtering for very small coordinates
            // Check for valid (non-NaN, non-infinite) values
            if (std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z) &&
                distance < maxDistance && 
                std::abs(point.x) < maxDistance && 
                std::abs(point.y) < maxDistance &&
                std::abs(point.z) < maxDistance) {
                filteredPoints.push_back(point);
                filteredColors.push_back(colors[i]);
            }
        }

        // If no points pass the filter, try with even more permissive settings
        if (filteredPoints.empty() && !pointCloud.empty()) {
            std::cout << "Warning: No points passed strict filtering, using more permissive filter..." << std::endl;
            
            for (size_t i = 0; i < std::min((size_t)10000, pointCloud.size()); i++) {
                cv::Point3f& point = pointCloud[i];
                
                // Just check for finite values
                if (std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z)) {
                    filteredPoints.push_back(point);
                    filteredColors.push_back(colors[i]);
                }
            }
        }

        pointCloud = filteredPoints;
        colors = filteredColors;

        return static_cast<int>(pointCloud.size());

    } catch (const std::exception& e) {
        std::cerr << "Error filtering point cloud: " << e.what() << std::endl;
        return 0;
    }
}

} // namespace StereoReconstruction