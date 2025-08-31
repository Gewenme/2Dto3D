#include "image_resize.h"
#include "corner_detection.h"
#include "mono_calibration.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"
#include "model_viewer.h"

#include <iostream>
#include <filesystem>

/**
 * Simple example demonstrating the 2D to 3D reconstruction pipeline
 * This example uses relative paths and creates all necessary directories
 */
int main() {
    std::cout << "=== 2D to 3D Reconstruction Example ===" << std::endl;
    
    // Create example directory structure
    std::filesystem::create_directories("example/input/left");
    std::filesystem::create_directories("example/input/right");
    std::filesystem::create_directories("example/output");
    
    // Example configuration - Updated to use 8.2mm squares
    const int boardWidth = 9;
    const int boardHeight = 6;
    const float squareSize = 0.0082f;  // 8.2mm squares
    const int imageWidth = 640;
    const int imageHeight = 480;
    
    std::cout << "\n1. Image Resizing..." << std::endl;
    
    // Step 1: Resize images (if input images exist)
    if (std::filesystem::exists("example/input/left") && 
        !std::filesystem::is_empty("example/input/left")) {
        
        bool success1 = ImageUtils::resizeImage(
            "example/input/left",
            "example/output/left_resized",
            imageWidth, imageHeight, 
            ImageUtils::LINEAR
        );
        
        bool success2 = ImageUtils::resizeImage(
            "example/input/right",
            "example/output/right_resized",
            imageWidth, imageHeight, 
            ImageUtils::LINEAR
        );
        
        if (success1 && success2) {
            std::cout << "✓ Image resizing completed successfully" << std::endl;
            
            std::cout << "\n2. Corner Detection..." << std::endl;
            
            // Step 2: Corner detection
            bool corners1 = detectAndDrawCorners(
                "example/output/left_resized",
                "example/output/left_corners",
                boardWidth, boardHeight, 1.0f
            );
            
            bool corners2 = detectAndDrawCorners(
                "example/output/right_resized", 
                "example/output/right_corners",
                boardWidth, boardHeight, 1.0f
            );
            
            if (corners1 && corners2) {
                std::cout << "✓ Corner detection completed successfully" << std::endl;
                
                std::cout << "\n3. Camera Calibration..." << std::endl;
                
                // Step 3: Mono calibration with undistorted image generation
                bool leftCalib = MonoCalibration::calibrateCamera(
                    "example/output/left_corners",
                    "example/output/left_resized", 
                    "example/output/left_calibration",
                    boardWidth, boardHeight, squareSize,
                    imageWidth, imageHeight,
                    true,  // Save undistorted images
                    "example/output/left_undistorted"  // Path for undistorted images
                );
                
                bool rightCalib = MonoCalibration::calibrateCamera(
                    "example/output/right_corners",
                    "example/output/right_resized", 
                    "example/output/right_calibration",
                    boardWidth, boardHeight, squareSize,
                    imageWidth, imageHeight,
                    true,  // Save undistorted images
                    "example/output/right_undistorted"  // Path for undistorted images
                );
                
                if (leftCalib && rightCalib) {
                    std::cout << "✓ Mono calibration completed for both cameras" << std::endl;
                    std::cout << "  - Generated undistorted (corrected) images" << std::endl;
                    std::cout << "  - Generated residual error visualization images" << std::endl;
                } else {
                    std::cout << "⚠ Mono calibration had issues, but continuing..." << std::endl;
                }
                
                std::cout << "\n4. Stereo Calibration..." << std::endl;
                
                // Step 4: Stereo calibration
                bool stereoCalib = StereoCalibration::calibrateStereoCamera(
                    "example/output/left_corners",
                    "example/output/right_corners",
                    "example/output/stereo_calibration",
                    boardWidth, boardHeight, squareSize,
                    imageWidth, imageHeight
                );
                
                if (stereoCalib) {
                    std::cout << "✓ Stereo calibration completed successfully" << std::endl;
                    
                    // For 3D reconstruction, we need actual scene images
                    // Check if scene images exist
                    if (std::filesystem::exists("example/input/scene_left.jpg") &&
                        std::filesystem::exists("example/input/scene_right.jpg")) {
                        
                        std::cout << "\n5. 3D Reconstruction..." << std::endl;
                        
                        bool reconstruction = StereoReconstruction::reconstruct3D(
                            "example/input/scene_left.jpg",
                            "example/input/scene_right.jpg",
                            "example/output/stereo_calibration",
                            "example/output/3d_model",
                            StereoReconstruction::PLY_FORMAT,
                            StereoReconstruction::MEDIUM_QUALITY,
                            squareSize, imageWidth, imageHeight
                        );
                        
                        if (reconstruction) {
                            std::cout << "✓ 3D reconstruction completed successfully" << std::endl;
                            
                            std::cout << "\n6. Model Viewing..." << std::endl;
                            
                            // Load and display the 3D model
                            std::vector<cv::Point3f> points;
                            std::vector<cv::Vec3b> colors;
                            
                            if (ModelViewer::loadModel("example/output/3d_model/reconstruction.ply", 
                                                     points, colors)) {
                                std::cout << "✓ 3D model loaded with " << points.size() << " points" << std::endl;
                                
                                // Generate projection views
                                ModelViewer::generateProjectionViews(points, colors, 
                                                                    "example/output/model_views");
                                
                                // Save statistics
                                ModelViewer::saveModelStatistics(points, 
                                                                "example/output/model_statistics.txt");
                                
                                std::cout << "✓ Model analysis completed" << std::endl;
                                
                                // Optionally display the model (requires display)
                                // ModelViewer::displayModel(points, colors);
                            }
                        } else {
                            std::cerr << "✗ 3D reconstruction failed" << std::endl;
                        }
                    } else {
                        std::cout << "\nℹ  Scene images not found. Skipping 3D reconstruction." << std::endl;
                        std::cout << "   Place scene_left.jpg and scene_right.jpg in example/input/ to test reconstruction." << std::endl;
                    }
                } else {
                    std::cerr << "✗ Stereo calibration failed" << std::endl;
                }
            } else {
                std::cerr << "✗ Corner detection failed" << std::endl;
                std::cout << "   Make sure input images contain visible chessboard patterns." << std::endl;
            }
        } else {
            std::cerr << "✗ Image resizing failed" << std::endl;
        }
    } else {
        std::cout << "\nℹ  No input images found in example/input/left and example/input/right" << std::endl;
        std::cout << "   Please add chessboard calibration images to test the complete pipeline." << std::endl;
        
        // Create sample directory structure info
        std::cout << "\nExpected directory structure:" << std::endl;
        std::cout << "example/" << std::endl;
        std::cout << "├── input/" << std::endl;
        std::cout << "│   ├── left/           # Left camera chessboard images" << std::endl;
        std::cout << "│   ├── right/          # Right camera chessboard images" << std::endl;
        std::cout << "│   ├── scene_left.jpg  # Left scene image for 3D reconstruction" << std::endl;
        std::cout << "│   └── scene_right.jpg # Right scene image for 3D reconstruction" << std::endl;
        std::cout << "└── output/             # Generated output files" << std::endl;
    }
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    return 0;
}