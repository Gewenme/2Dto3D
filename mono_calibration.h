#ifndef MONO_CALIBRATION_H
#define MONO_CALIBRATION_H

#include <string>
#include <opencv2/opencv.hpp>

namespace MonoCalibration {
    
    /**
     * Calibrate a single camera using chessboard images
     * @param cornerDataPath Path to corner data file
     * @param imageFolderPath Path to folder containing images for calibration
     * @param outputPath Path to save calibration results
     * @param boardWidth Number of inner corners along chessboard width
     * @param boardHeight Number of inner corners along chessboard height
     * @param squareSize Real size of chessboard square in meters
     * @param imageWidth Image width in pixels
     * @param imageHeight Image height in pixels
     * @param saveUndistorted Whether to save undistorted images
     * @param undistortedPath Path to save undistorted images (if saveUndistorted is true)
     * @return true if calibration successful, false otherwise
     */
    bool calibrateCamera(const std::string& cornerDataPath,
                         const std::string& imageFolderPath,
                         const std::string& outputPath,
                         int boardWidth,
                         int boardHeight,
                         float squareSize,
                         int imageWidth,
                         int imageHeight,
                         bool saveUndistorted = false,
                         const std::string& undistortedPath = "");

    /**
     * Load camera calibration parameters from file
     * @param calibrationFile Path to calibration file
     * @param cameraMatrix Output camera matrix
     * @param distCoeffs Output distortion coefficients
     * @return true if loaded successfully
     */
    bool loadCalibrationData(const std::string& calibrationFile,
                            cv::Mat& cameraMatrix,
                            cv::Mat& distCoeffs);

    /**
     * Save camera calibration parameters to file
     * @param calibrationFile Path to save calibration file
     * @param cameraMatrix Camera matrix
     * @param distCoeffs Distortion coefficients
     * @param imageSize Image size
     * @param reprojectionError RMS reprojection error
     * @return true if saved successfully
     */
    bool saveCalibrationData(const std::string& calibrationFile,
                            const cv::Mat& cameraMatrix,
                            const cv::Mat& distCoeffs,
                            const cv::Size& imageSize,
                            double reprojectionError);

    /**
     * Undistort an image using calibration parameters
     * @param inputImage Input distorted image
     * @param outputImage Output undistorted image
     * @param cameraMatrix Camera matrix
     * @param distCoeffs Distortion coefficients
     * @return true if successful
     */
    bool undistortImage(const cv::Mat& inputImage,
                        cv::Mat& outputImage,
                        const cv::Mat& cameraMatrix,
                        const cv::Mat& distCoeffs);

    /**
     * Generate reprojection error visualization images
     * @param cornerDataPath Path to corner data file
     * @param imageFolderPath Path to folder containing calibration images
     * @param outputPath Path to save residual error images
     * @param cameraMatrix Camera matrix from calibration
     * @param distCoeffs Distortion coefficients from calibration
     * @param objectPoints 3D object points used in calibration
     * @param imagePoints 2D image points used in calibration
     * @param rvecs Rotation vectors from calibration
     * @param tvecs Translation vectors from calibration
     * @return true if successful
     */
    bool generateResidualImages(const std::string& cornerDataPath,
                               const std::string& imageFolderPath,
                               const std::string& outputPath,
                               const cv::Mat& cameraMatrix,
                               const cv::Mat& distCoeffs,
                               const std::vector<std::vector<cv::Point3f>>& objectPoints,
                               const std::vector<std::vector<cv::Point2f>>& imagePoints,
                               const std::vector<cv::Mat>& rvecs,
                               const std::vector<cv::Mat>& tvecs);
}

#endif // MONO_CALIBRATION_H