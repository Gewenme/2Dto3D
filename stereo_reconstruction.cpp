#include "stereo_reconstruction.h"
#include "stereo_calibration.h"
#include <iostream>
#include <filesystem>
#include <fstream>

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
        if (!computeDisparityMap(rectifiedLeft, rightImage, disparityMap, quality)) {
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

        // Apply improved filtering with better scale
        int remainingPoints = filterPointCloud(pointCloud, colors, 10.0f); 
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
            // Use StereoSGBM for higher quality with improved parameters
            stereo = cv::StereoSGBM::create(
                0,       // minDisparity
                160,     // numDisparities (increased for better range)
                3,       // blockSize (smaller for more detail)
                864,     // P1 (8*channels*blockSize^2)
                3456,    // P2 (32*channels*blockSize^2) 
                5,       // disp12MaxDiff (reduced for stricter matching)
                16,      // preFilterCap
                5,       // uniquenessRatio (reduced for more unique matches)
                50,      // speckleWindowSize (reduced to remove smaller noise)
                1,       // speckleRange (reduced for stricter filtering)
                cv::StereoSGBM::MODE_SGBM_3WAY  // Use 3-way mode for better quality
            );
        } else if (quality == MEDIUM_QUALITY) {
            // Use StereoSGBM with medium quality parameters
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
            int blockSize = 21;
            auto stereoBM = cv::StereoBM::create(64, blockSize);
            stereoBM->setPreFilterCap(31);
            stereoBM->setUniquenessRatio(15);
            stereoBM->setSpeckleWindowSize(100);
            stereoBM->setSpeckleRange(32);
            stereo = stereoBM;
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

        if (pointCloud.empty()) {
            std::cout << "Point cloud is empty, nothing to filter" << std::endl;
            return 0;
        }

        // Calculate mean depth for adaptive filtering
        double meanZ = 0;
        int validCount = 0;
        double minZ = std::numeric_limits<double>::max();
        double maxZ = std::numeric_limits<double>::lowest();
        
        for (const auto& point : pointCloud) {
            if (point.z > 0 && point.z < maxDistance && std::isfinite(point.z)) {
                meanZ += point.z;
                validCount++;
                minZ = std::min(minZ, (double)point.z);
                maxZ = std::max(maxZ, (double)point.z);
            }
        }
        
        if (validCount == 0) {
            std::cout << "No valid points found in initial filtering" << std::endl;
            return 0;
        }
        
        meanZ /= validCount;
        std::cout << "Point cloud stats: " << pointCloud.size() << " total points, " 
                  << validCount << " valid, Z range: " << minZ << " to " << maxZ 
                  << ", mean Z: " << meanZ << std::endl;
        
        // Use more permissive thresholds
        double zThresholdLow = std::max(0.1, minZ);
        double zThresholdHigh = std::min((double)maxDistance, maxZ);
        double xyThreshold = maxDistance; // Very permissive X,Y threshold

        for (size_t i = 0; i < pointCloud.size(); i++) {
            cv::Point3f& point = pointCloud[i];
            
            // Much more permissive filtering criteria
            if (std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z) &&
                point.z > zThresholdLow && point.z < zThresholdHigh &&
                std::abs(point.x) < xyThreshold && std::abs(point.y) < xyThreshold) {
                filteredPoints.push_back(point);
                if (i < colors.size()) {
                    filteredColors.push_back(colors[i]);
                } else {
                    filteredColors.push_back(cv::Vec3b(128, 128, 128)); // Default color
                }
            }
        }

        pointCloud = filteredPoints;
        colors = filteredColors;

        std::cout << "Filtering completed: " << filteredPoints.size() << " points kept" << std::endl;
        return static_cast<int>(pointCloud.size());

    } catch (const std::exception& e) {
        std::cerr << "Error filtering point cloud: " << e.what() << std::endl;
        return 0;
    }
}

} // namespace StereoReconstruction