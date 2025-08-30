# 2D to 3D Reconstruction System

This system provides a complete pipeline for converting stereo images into 3D point clouds.

## Features

- Image preprocessing and resizing
- Chessboard corner detection
- Mono and stereo camera calibration
- 3D stereo reconstruction
- 3D model viewing and analysis

## Build Instructions

### Prerequisites
- CMake 3.10 or higher
- OpenCV 4.0 or higher
- C++17 compatible compiler

### Building
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

The main program performs a complete 2D to 3D reconstruction pipeline:

```bash
./bin/2Dto3D
```

### Pipeline Steps

1. **Image Resizing** - Standardizes image dimensions
2. **Corner Detection** - Detects chessboard corners for calibration
3. **Mono Calibration** - Calibrates individual cameras
4. **Stereo Calibration** - Calibrates the stereo camera system
5. **3D Reconstruction** - Generates 3D point cloud from stereo images
6. **Model Viewing** - Displays and analyzes the 3D model

### Input Requirements

- Left and right camera images in a stereo pair
- Images should contain chessboard patterns for calibration
- Standard image formats (JPG, PNG, BMP, TIFF) are supported

### Output

- Camera calibration parameters (YML files)
- 3D point cloud in PLY format
- Rectified stereo images
- Disparity maps
- Model statistics and projection views

## API Documentation

### ImageUtils Namespace

```cpp
// Resize images in a folder
bool resizeImage(const std::string& inputFolder,
                 const std::string& outputFolder,
                 int width, int height, int interpolationType);
```

### Corner Detection

```cpp
// Detect chessboard corners
bool detectAndDrawCorners(const std::string& inputFolder,
                          const std::string& outputFolder,
                          int boardWidth, int boardHeight,
                          float scaleFactor = 1.0f);
```

### MonoCalibration Namespace

```cpp
// Calibrate single camera
bool calibrateCamera(const std::string& cornerDataPath,
                     const std::string& imageFolderPath,
                     const std::string& outputPath,
                     int boardWidth, int boardHeight,
                     float squareSize, int imageWidth, int imageHeight,
                     bool saveUndistorted = false,
                     const std::string& undistortedPath = "");
```

### StereoCalibration Namespace

```cpp
// Calibrate stereo camera system
bool calibrateStereoCamera(const std::string& leftCornerDataPath,
                          const std::string& rightCornerDataPath,
                          const std::string& outputPath,
                          int boardWidth, int boardHeight,
                          float squareSize, int imageWidth, int imageHeight);
```

### StereoReconstruction Namespace

```cpp
// Reconstruct 3D scene from stereo pair
bool reconstruct3D(const std::string& leftImagePath,
                  const std::string& rightImagePath,
                  const std::string& calibrationPath,
                  const std::string& outputPath,
                  int outputFormat = PLY_FORMAT,
                  int quality = MEDIUM_QUALITY,
                  float squareSize = 0.0082f,
                  int imageWidth = 3264,
                  int imageHeight = 2448);
```

### ModelViewer Namespace

```cpp
// Load and display 3D models
bool loadModel(const std::string& filename,
               std::vector<cv::Point3f>& points,
               std::vector<cv::Vec3b>& colors);

bool displayModel(const std::vector<cv::Point3f>& points,
                  const std::vector<cv::Vec3b>& colors,
                  const std::string& windowName = "3D Model Viewer");
```

## Example Usage

### Basic Stereo Reconstruction

```cpp
#include "image_resize.h"
#include "corner_detection.h"
#include "stereo_calibration.h"
#include "stereo_reconstruction.h"

int main() {
    // Step 1: Resize images
    ImageUtils::resizeImage("input/left", "output/left_resized", 640, 480, ImageUtils::LINEAR);
    ImageUtils::resizeImage("input/right", "output/right_resized", 640, 480, ImageUtils::LINEAR);
    
    // Step 2: Detect corners
    detectAndDrawCorners("output/left_resized", "output/left_corners", 9, 6);
    detectAndDrawCorners("output/right_resized", "output/right_corners", 9, 6);
    
    // Step 3: Stereo calibration
    StereoCalibration::calibrateStereoCamera(
        "output/left_corners", "output/right_corners", 
        "output/calibration", 9, 6, 0.025f, 640, 480);
    
    // Step 4: 3D reconstruction
    StereoReconstruction::reconstruct3D(
        "input/scene_left.jpg", "input/scene_right.jpg",
        "output/calibration", "output/3d_model");
    
    return 0;
}
```

### Custom Image Processing

```cpp
// Create custom resize options
ImageUtils::ResizeOptions options(ImageUtils::CUBIC);
options.enableSharpening = true;
options.reduceNoise = true;

ImageUtils::resizeImage("input", "output", 800, 600, options);
```

### Model Analysis

```cpp
#include "model_viewer.h"

// Load and analyze 3D model
std::vector<cv::Point3f> points;
std::vector<cv::Vec3b> colors;

ModelViewer::loadModel("model.ply", points, colors);
ModelViewer::displayModel(points, colors);
ModelViewer::generateProjectionViews(points, colors, "output/views");
ModelViewer::saveModelStatistics(points, "model_stats.txt");
```

## Configuration Parameters

### Chessboard Parameters
- **boardWidth**: Number of inner corners along width (typically 9)
- **boardHeight**: Number of inner corners along height (typically 6)
- **squareSize**: Real size of chessboard squares in meters (e.g., 0.025m for 25mm squares)

### Image Parameters
- **imageWidth**: Image width in pixels
- **imageHeight**: Image height in pixels
- **interpolationType**: Resize interpolation (0=nearest, 1=cubic, 2=linear)

### Quality Settings
- **LOW_QUALITY (1)**: Fast processing, lower accuracy
- **MEDIUM_QUALITY (3)**: Balanced processing and accuracy
- **HIGH_QUALITY (5)**: Slow processing, higher accuracy

### Output Formats
- **PLY_FORMAT (0)**: Stanford PLY format with color
- **OBJ_FORMAT (1)**: Wavefront OBJ format
- **XYZ_FORMAT (2)**: Simple XYZ coordinate format

## Troubleshooting

### Common Issues

1. **Corner detection fails**: Ensure chessboard is clearly visible and well-lit
2. **Calibration error high**: Use more calibration images from different angles
3. **3D reconstruction poor**: Check stereo calibration quality and image alignment
4. **Build errors**: Verify OpenCV installation and CMake configuration

### Tips for Better Results

1. Use high-quality chessboard patterns
2. Capture calibration images from various angles and distances
3. Ensure good lighting and sharp focus
4. Use images with minimal motion blur
5. Maintain consistent camera settings during capture