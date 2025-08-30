#ifndef STEREO_RECONSTRUCTION_H
#define STEREO_RECONSTRUCTION_H

#include <string>
#include <opencv2/opencv.hpp>

namespace StereoReconstruction {

    // Output format enumeration
    enum OutputFormat {
        PLY_FORMAT = 0,
        OBJ_FORMAT = 1,
        XYZ_FORMAT = 2
    };

    // Quality settings
    enum Quality {
        LOW_QUALITY = 1,
        MEDIUM_QUALITY = 3,
        HIGH_QUALITY = 5
    };

    /**
     * Reconstruct 3D scene from stereo image pair
     * @param leftImagePath Path to left camera image
     * @param rightImagePath Path to right camera image
     * @param calibrationPath Path to stereo calibration data
     * @param outputPath Path to save 3D reconstruction results
     * @param outputFormat Output format (0=PLY, 1=OBJ, 2=XYZ)
     * @param quality Quality setting (1=low, 3=medium, 5=high)
     * @param squareSize Chessboard square size in meters (for scale)
     * @param imageWidth Image width in pixels
     * @param imageHeight Image height in pixels
     * @return true if reconstruction successful, false otherwise
     */
    bool reconstruct3D(const std::string& leftImagePath,
                      const std::string& rightImagePath,
                      const std::string& calibrationPath,
                      const std::string& outputPath,
                      int outputFormat = PLY_FORMAT,
                      int quality = MEDIUM_QUALITY,
                      float squareSize = 0.0082f,
                      int imageWidth = 3264,
                      int imageHeight = 2448);

    /**
     * Compute disparity map from rectified stereo pair
     * @param leftImage Left rectified image
     * @param rightImage Right rectified image
     * @param disparityMap Output disparity map
     * @param quality Quality parameter for stereo matching
     * @return true if successful
     */
    bool computeDisparityMap(const cv::Mat& leftImage,
                            const cv::Mat& rightImage,
                            cv::Mat& disparityMap,
                            int quality = MEDIUM_QUALITY);

    /**
     * Generate 3D point cloud from disparity map
     * @param disparityMap Input disparity map
     * @param leftImage Left image for texture
     * @param Q Disparity-to-depth mapping matrix
     * @param pointCloud Output 3D point cloud
     * @param colors Output point colors
     * @return true if successful
     */
    bool generatePointCloud(const cv::Mat& disparityMap,
                           const cv::Mat& leftImage,
                           const cv::Mat& Q,
                           std::vector<cv::Point3f>& pointCloud,
                           std::vector<cv::Vec3b>& colors);

    /**
     * Save point cloud to file
     * @param pointCloud 3D point coordinates
     * @param colors Point colors
     * @param filename Output filename
     * @param format Output format (PLY_FORMAT, OBJ_FORMAT, XYZ_FORMAT)
     * @return true if saved successfully
     */
    bool savePointCloud(const std::vector<cv::Point3f>& pointCloud,
                       const std::vector<cv::Vec3b>& colors,
                       const std::string& filename,
                       int format = PLY_FORMAT);

    /**
     * Filter point cloud to remove outliers and noise
     * @param pointCloud Input/output point cloud
     * @param colors Input/output colors
     * @param maxDistance Maximum allowed distance from origin
     * @return number of points remaining after filtering
     */
    int filterPointCloud(std::vector<cv::Point3f>& pointCloud,
                        std::vector<cv::Vec3b>& colors,
                        float maxDistance = 10.0f);
}

#endif // STEREO_RECONSTRUCTION_H