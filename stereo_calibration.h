#ifndef STEREO_CALIBRATION_H
#define STEREO_CALIBRATION_H

#include <string>
#include <opencv2/opencv.hpp>

namespace StereoCalibration {

    /**
     * Calibrate stereo camera system using corner detection results
     * @param leftCornerDataPath Path to left camera corner data
     * @param rightCornerDataPath Path to right camera corner data  
     * @param outputPath Path to save stereo calibration results
     * @param boardWidth Number of inner corners along chessboard width
     * @param boardHeight Number of inner corners along chessboard height
     * @param squareSize Real size of chessboard square in meters
     * @param imageWidth Image width in pixels
     * @param imageHeight Image height in pixels
     * @return true if calibration successful, false otherwise
     */
    bool calibrateStereoCamera(const std::string& leftCornerDataPath,
                              const std::string& rightCornerDataPath,
                              const std::string& outputPath,
                              int boardWidth,
                              int boardHeight,
                              float squareSize,
                              int imageWidth,
                              int imageHeight);

    /**
     * Load stereo calibration parameters from file
     * @param calibrationFile Path to stereo calibration file
     * @param leftCameraMatrix Left camera matrix
     * @param leftDistCoeffs Left distortion coefficients
     * @param rightCameraMatrix Right camera matrix
     * @param rightDistCoeffs Right distortion coefficients
     * @param R Rotation matrix between cameras
     * @param T Translation vector between cameras
     * @param E Essential matrix
     * @param F Fundamental matrix
     * @return true if loaded successfully
     */
    bool loadStereoCalibrationData(const std::string& calibrationFile,
                                  cv::Mat& leftCameraMatrix, cv::Mat& leftDistCoeffs,
                                  cv::Mat& rightCameraMatrix, cv::Mat& rightDistCoeffs,
                                  cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F);

    /**
     * Save stereo calibration parameters to file
     * @param calibrationFile Path to save calibration file
     * @param leftCameraMatrix Left camera matrix
     * @param leftDistCoeffs Left distortion coefficients
     * @param rightCameraMatrix Right camera matrix
     * @param rightDistCoeffs Right distortion coefficients
     * @param R Rotation matrix between cameras
     * @param T Translation vector between cameras
     * @param E Essential matrix
     * @param F Fundamental matrix
     * @param imageSize Image size
     * @param reprojectionError RMS reprojection error
     * @return true if saved successfully
     */
    bool saveStereoCalibrationData(const std::string& calibrationFile,
                                  const cv::Mat& leftCameraMatrix, const cv::Mat& leftDistCoeffs,
                                  const cv::Mat& rightCameraMatrix, const cv::Mat& rightDistCoeffs,
                                  const cv::Mat& R, const cv::Mat& T, const cv::Mat& E, const cv::Mat& F,
                                  const cv::Size& imageSize, double reprojectionError);

    /**
     * Rectify stereo image pair using calibration parameters
     * @param leftImage Left camera image
     * @param rightImage Right camera image
     * @param rectifiedLeft Output rectified left image
     * @param rectifiedRight Output rectified right image
     * @param calibrationFile Path to stereo calibration file
     * @return true if successful
     */
    bool rectifyStereoPair(const cv::Mat& leftImage, const cv::Mat& rightImage,
                          cv::Mat& rectifiedLeft, cv::Mat& rectifiedRight,
                          const std::string& calibrationFile);
}

#endif // STEREO_CALIBRATION_H