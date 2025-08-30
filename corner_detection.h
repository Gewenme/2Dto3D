#ifndef CORNER_DETECTION_H
#define CORNER_DETECTION_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

/**
 * Detect and draw corners on chessboard images
 * @param inputFolder Path to input image folder containing chessboard images
 * @param outputFolder Path to output folder for results (will be created if not exists)
 * @param boardWidth Number of inner corners along the width of chessboard
 * @param boardHeight Number of inner corners along the height of chessboard
 * @param scaleFactor Scale factor for images (1.0 = original size)
 * @return true if successful, false otherwise
 */
bool detectAndDrawCorners(const std::string& inputFolder,
                          const std::string& outputFolder,
                          int boardWidth,
                          int boardHeight,
                          float scaleFactor = 1.0f);

/**
 * Detect corners on a single image
 * @param imagePath Path to the image file
 * @param boardWidth Number of inner corners along the width
 * @param boardHeight Number of inner corners along the height
 * @param corners Output vector of detected corners
 * @return true if corners were detected successfully
 */
bool detectCornersInImage(const std::string& imagePath,
                         int boardWidth,
                         int boardHeight,
                         std::vector<cv::Point2f>& corners);

/**
 * Save corner data to file
 * @param corners Vector of corner points
 * @param filename Output filename for corner data
 * @return true if saved successfully
 */
bool saveCornerData(const std::vector<std::vector<cv::Point2f>>& corners,
                    const std::string& filename);

#endif // CORNER_DETECTION_H