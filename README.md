# 2D to 3D Reconstruction System

A complete pipeline for converting stereo camera images into 3D point clouds using OpenCV.

## Features

- **Image Processing**: Resize and prepare stereo images
- **Corner Detection**: Automatic chessboard corner detection for calibration
- **Camera Calibration**: Both mono and stereo camera calibration
- **3D Reconstruction**: Generate 3D point clouds from stereo image pairs
- **Model Visualization**: View and analyze reconstructed 3D models

## Quick Start

1. **Build the project**:
```bash
mkdir build && cd build
cmake ..
make
```

2. **Run the enhanced demo** (recommended):
```bash
./bin/demo
```

3. **Run the example**:
```bash
./bin/example
```

4. **Or run the main program**:
```bash
./bin/2Dto3D
```

## Demo Programs

### Enhanced Demo (`./bin/demo`)
- Optimized parameters for better 3D reconstruction results
- Automatic coordinate scaling for meaningful visualization
- Comprehensive output analysis and validation
- Robust error handling with progress reporting

### Main Program (`./bin/2Dto3D`) 
- Complete pipeline using relative paths
- Full resolution processing with camera correction
- Shows undistorted/corrected images
- Generates detailed model statistics

### Example Program (`./bin/example`)
- Flexible demonstration with configurable paths
- Educational example showing expected workflow
- Graceful handling of missing input files

## Output Analysis

The enhanced pipeline generates:

### Calibration & Correction
- Camera calibration parameters (YML format)
- Undistorted images showing correction effects
- Stereo calibration and rectification data

### 3D Reconstruction  
- Point cloud in PLY format with color
- Disparity maps visualizing depth information
- Rectified stereo image pairs

### Model Visualization
- Multi-angle projection views (XY, XZ, YZ)
- Coordinate range validation and scaling
- Statistical analysis of 3D model quality

## Documentation

- See [USAGE.md](USAGE.md) for detailed usage instructions and API documentation
- The `example.cpp` file demonstrates the complete pipeline

## Requirements

- CMake 3.10+
- OpenCV 4.0+
- C++17 compiler

## Project Structure

- `*.h` - Header files with API declarations
- `*.cpp` - Implementation files
- `main.cpp` - Main application with hardcoded paths
- `example.cpp` - Flexible example with relative paths
- `CMakeLists.txt` - Build configuration
- `USAGE.md` - Comprehensive documentation