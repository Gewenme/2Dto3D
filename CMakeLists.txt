cmake_minimum_required(VERSION 3.10)
project(2Dto3D)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executables
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

# Add individual step executables for 8.2mm chessboard pipeline
add_executable(step1_image_preprocessing
    step1_image_preprocessing.cpp
    image_resize.cpp
)

add_executable(step2_corner_detection
    step2_corner_detection.cpp
    corner_detection.cpp
)

add_executable(step3_monocular_calibration
    step3_monocular_calibration.cpp
    mono_calibration.cpp
)

add_executable(step4_stereo_rectification
    step4_stereo_rectification.cpp
    stereo_calibration.cpp
    mono_calibration.cpp
)

add_executable(step5_3d_reconstruction
    step5_3d_reconstruction.cpp
    stereo_reconstruction.cpp
    stereo_calibration.cpp
    mono_calibration.cpp
)

# Link libraries
target_link_libraries(2Dto3D ${OpenCV_LIBS})
target_link_libraries(example ${OpenCV_LIBS})
target_link_libraries(demo_8_2mm ${OpenCV_LIBS})
target_link_libraries(step1_image_preprocessing ${OpenCV_LIBS})
target_link_libraries(step2_corner_detection ${OpenCV_LIBS})
target_link_libraries(step3_monocular_calibration ${OpenCV_LIBS})
target_link_libraries(step4_stereo_rectification ${OpenCV_LIBS})
target_link_libraries(step5_3d_reconstruction ${OpenCV_LIBS})

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
set_target_properties(step1_image_preprocessing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(step2_corner_detection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(step3_monocular_calibration PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(step4_stereo_rectification PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(step5_3d_reconstruction PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add install target
install(TARGETS 2Dto3D example demo_8_2mm 
                step1_image_preprocessing step2_corner_detection step3_monocular_calibration
                step4_stereo_rectification step5_3d_reconstruction 
        DESTINATION bin)