#include "image_resize.h"
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace ImageUtils {

ResizeOptions::ResizeOptions(InterpolationType type) 
    : interpolationType(type), enableSharpening(false), 
      reduceNoise(false), preserveAspectRatio(true) {
}

bool resizeImage(const std::string& inputFolder,
                 const std::string& outputFolder,
                 int width,
                 int height,
                 int interpolationType) {
    
    ResizeOptions options(static_cast<InterpolationType>(interpolationType));
    return resizeImage(inputFolder, outputFolder, width, height, options);
}

bool resizeImage(const std::string& inputFolder,
                 const std::string& outputFolder,
                 int width,
                 int height,
                 const ResizeOptions& options) {
    try {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(outputFolder);
        
        // Check if input folder exists
        if (!std::filesystem::exists(inputFolder)) {
            std::cerr << "Input folder does not exist: " << inputFolder << std::endl;
            return false;
        }

        // Get OpenCV interpolation flag
        int cvInterpolation;
        switch (options.interpolationType) {
            case NEAREST: cvInterpolation = cv::INTER_NEAREST; break;
            case CUBIC: cvInterpolation = cv::INTER_CUBIC; break;
            case LINEAR:
            default: cvInterpolation = cv::INTER_LINEAR; break;
        }

        // Process all image files in the input folder
        int processedCount = 0;
        for (const auto& entry : std::filesystem::directory_iterator(inputFolder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                // Convert to lowercase
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

                        // Resize image
                        cv::Mat resizedImage;
                        cv::Size targetSize(width, height);
                        
                        if (options.preserveAspectRatio) {
                            // Calculate aspect ratio preserving dimensions
                            double aspectRatio = static_cast<double>(image.cols) / image.rows;
                            double targetAspectRatio = static_cast<double>(width) / height;
                            
                            if (aspectRatio > targetAspectRatio) {
                                targetSize.height = static_cast<int>(width / aspectRatio);
                            } else {
                                targetSize.width = static_cast<int>(height * aspectRatio);
                            }
                        }
                        
                        cv::resize(image, resizedImage, targetSize, 0, 0, cvInterpolation);

                        // Apply optional post-processing
                        if (options.enableSharpening) {
                            cv::Mat kernel = (cv::Mat_<float>(3,3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
                            cv::filter2D(resizedImage, resizedImage, -1, kernel);
                        }
                        
                        if (options.reduceNoise) {
                            cv::bilateralFilter(resizedImage, resizedImage, 9, 75, 75);
                        }

                        // Save resized image
                        std::string outputPath = outputFolder + "/" + entry.path().filename().string();
                        if (cv::imwrite(outputPath, resizedImage)) {
                            processedCount++;
                        } else {
                            std::cerr << "Failed to save image: " << outputPath << std::endl;
                        }
                        
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing image " << entry.path().string() << ": " << e.what() << std::endl;
                    }
                }
            }
        }
        
        std::cout << "Successfully processed " << processedCount << " images" << std::endl;
        return processedCount > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in resizeImage: " << e.what() << std::endl;
        return false;
    }
}

} // namespace ImageUtils