#include "corner_detection.h"
#include <iostream>
#include <filesystem>
#include <fstream>

bool detectAndDrawCorners(const std::string& inputFolder,
                          const std::string& outputFolder,
                          int boardWidth,
                          int boardHeight,
                          float scaleFactor) {
    try {
        // Create output directory structure
        std::filesystem::create_directories(outputFolder);
        std::filesystem::create_directories(outputFolder + "/corner_data");
        std::filesystem::create_directories(outputFolder + "/corner_images");
        
        // Check if input folder exists
        if (!std::filesystem::exists(inputFolder)) {
            std::cerr << "Input folder does not exist: " << inputFolder << std::endl;
            return false;
        }

        cv::Size boardSize(boardWidth, boardHeight);
        std::vector<std::vector<cv::Point2f>> allCorners;
        std::vector<std::string> imageFiles;
        int successCount = 0;

        // Process all image files in the input folder
        for (const auto& entry : std::filesystem::directory_iterator(inputFolder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                // Check if it's an image file
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tiff") {
                    try {
                        // Load image
                        cv::Mat image = cv::imread(entry.path().string());
                        if (image.empty()) {
                            std::cerr << "Failed to load image: " << entry.path().string() << std::endl;
                            continue;
                        }

                        // Scale image if necessary
                        if (scaleFactor != 1.0f) {
                            cv::Mat scaledImage;
                            cv::resize(image, scaledImage, cv::Size(), scaleFactor, scaleFactor);
                            image = scaledImage;
                        }

                        // Convert to grayscale
                        cv::Mat gray;
                        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

                        // Apply Gaussian blur to reduce noise
                        cv::Mat blurred;
                        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.0);

                        // Find chessboard corners with improved flags for better accuracy
                        std::vector<cv::Point2f> corners;
                        bool found = cv::findChessboardCorners(blurred, boardSize, corners,
                            cv::CALIB_CB_ADAPTIVE_THRESH | 
                            cv::CALIB_CB_NORMALIZE_IMAGE | 
                            cv::CALIB_CB_FILTER_QUADS |
                            cv::CALIB_CB_FAST_CHECK);

                        if (found) {
                            // Refine corner positions with higher precision
                            cv::cornerSubPix(blurred, corners, cv::Size(5, 5), cv::Size(-1, -1),
                                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 50, 0.01));

                            // Draw corners on image
                            cv::Mat imageWithCorners = image.clone();
                            cv::drawChessboardCorners(imageWithCorners, boardSize, corners, found);

                            // Save image with corners
                            std::string outputImagePath = outputFolder + "/corner_images/" + entry.path().filename().string();
                            cv::imwrite(outputImagePath, imageWithCorners);

                            // Store corners and image info
                            allCorners.push_back(corners);
                            imageFiles.push_back(entry.path().filename().string());
                            successCount++;

                            std::cout << "Corners detected in: " << entry.path().filename().string() << std::endl;
                        } else {
                            std::cerr << "No corners found in: " << entry.path().filename().string() << std::endl;
                        }

                    } catch (const std::exception& e) {
                        std::cerr << "Error processing image " << entry.path().string() << ": " << e.what() << std::endl;
                    }
                }
            }
        }

        // Save corner data
        if (successCount > 0) {
            std::string cornerDataPath = outputFolder + "/corner_data/corners.yml";
            cv::FileStorage fs(cornerDataPath, cv::FileStorage::WRITE);
            
            fs << "board_width" << boardWidth;
            fs << "board_height" << boardHeight;
            fs << "image_count" << successCount;
            fs << "scale_factor" << scaleFactor;
            
            fs << "image_files" << "[";
            for (const auto& filename : imageFiles) {
                fs << filename;
            }
            fs << "]";
            
            fs << "corners" << "[";
            for (const auto& corners : allCorners) {
                fs << corners;
            }
            fs << "]";
            
            fs.release();
            
            std::cout << "Corner data saved to: " << cornerDataPath << std::endl;
            std::cout << "Successfully processed " << successCount << " images with corner detection" << std::endl;
        }

        return successCount > 0;

    } catch (const std::exception& e) {
        std::cerr << "Error in detectAndDrawCorners: " << e.what() << std::endl;
        return false;
    }
}

bool detectCornersInImage(const std::string& imagePath,
                         int boardWidth,
                         int boardHeight,
                         std::vector<cv::Point2f>& corners) {
    try {
        cv::Mat image = cv::imread(imagePath);
        if (image.empty()) {
            std::cerr << "Failed to load image: " << imagePath << std::endl;
            return false;
        }

        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        // Apply Gaussian blur to reduce noise
        cv::Mat blurred;
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.0);

        cv::Size boardSize(boardWidth, boardHeight);
        bool found = cv::findChessboardCorners(blurred, boardSize, corners,
            cv::CALIB_CB_ADAPTIVE_THRESH | 
            cv::CALIB_CB_NORMALIZE_IMAGE | 
            cv::CALIB_CB_FILTER_QUADS |
            cv::CALIB_CB_FAST_CHECK);

        if (found) {
            cv::cornerSubPix(blurred, corners, cv::Size(5, 5), cv::Size(-1, -1),
                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 50, 0.01));
        }

        return found;

    } catch (const std::exception& e) {
        std::cerr << "Error in detectCornersInImage: " << e.what() << std::endl;
        return false;
    }
}

bool saveCornerData(const std::vector<std::vector<cv::Point2f>>& corners,
                    const std::string& filename) {
    try {
        cv::FileStorage fs(filename, cv::FileStorage::WRITE);
        
        fs << "corner_count" << static_cast<int>(corners.size());
        fs << "corners" << "[";
        for (const auto& cornerSet : corners) {
            fs << cornerSet;
        }
        fs << "]";
        
        fs.release();
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving corner data: " << e.what() << std::endl;
        return false;
    }
}