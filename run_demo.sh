#!/bin/bash

echo "=================================================================="
echo "        2D to 3D Reconstruction System Demonstration"
echo "=================================================================="
echo ""

cd "$(dirname "$0")"

# Build the project if needed
if [ ! -f "build/bin/demo" ]; then
    echo "Building project..."
    mkdir -p build
    cd build
    cmake ..
    make
    cd ..
    echo "Build completed."
    echo ""
fi

# Clean previous output
echo "Cleaning previous output..."
rm -rf demo_output output
echo ""

# Run the enhanced demo
echo "Running Enhanced Demo (recommended)..."
echo "======================================"
./build/bin/demo

echo ""
echo "Generated demonstration files:"
echo "------------------------------"

if [ -d "demo_output" ]; then
    find demo_output -name "*.jpg" -o -name "*.ply" -o -name "*.txt" -o -name "*.yml" | sort
    echo ""
    echo "Key results:"
    echo "- 3D Point Cloud: demo_output/step4_reconstruction/reconstruction.ply"
    echo "- Disparity Map: demo_output/step4_reconstruction/disparity_map.jpg"
    echo "- Model Views: demo_output/step5_views/*.jpg"
    echo "- Statistics: demo_output/model_analysis.txt"
fi

echo ""
echo "=================================================================="
echo "To run other demo programs:"
echo "  ./build/bin/2Dto3D     (Full resolution main program)"
echo "  ./build/bin/example    (Flexible example program)"
echo "=================================================================="