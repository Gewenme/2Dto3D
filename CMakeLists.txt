cmake_minimum_required(VERSION 3.10)
project(2Dto3D)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executables using existing modular code
add_executable(2Dto3D
    main.cpp
    image_resize.cpp
    corner_detection.cpp
    mono_calibration.cpp
    stereo_calibration.cpp
    stereo_reconstruction.cpp
    model_viewer.cpp
)

add_executable(example
    example.cpp
    image_resize.cpp
    corner_detection.cpp
    mono_calibration.cpp
    stereo_calibration.cpp
    stereo_reconstruction.cpp
    model_viewer.cpp
)

add_executable(demo_8_2mm
    demo_8_2mm.cpp
    image_resize.cpp
    corner_detection.cpp
    mono_calibration.cpp
    stereo_calibration.cpp
    stereo_reconstruction.cpp
    model_viewer.cpp
)

# Link libraries
target_link_libraries(2Dto3D ${OpenCV_LIBS})
target_link_libraries(example ${OpenCV_LIBS})
target_link_libraries(demo_8_2mm ${OpenCV_LIBS})

# Set output directory
set_target_properties(2Dto3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(demo_8_2mm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add install target
install(TARGETS 2Dto3D example demo_8_2mm 
        DESTINATION bin)