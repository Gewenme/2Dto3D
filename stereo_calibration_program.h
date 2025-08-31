#ifndef STEREO_CALIBRATION_PROGRAM_H
#define STEREO_CALIBRATION_PROGRAM_H

#include <string>

/**
 * 双目标定程序类 (Stereo Calibration Program Class)
 * 类似MATLAB双目标定工具箱的完整标定流程
 * Similar to MATLAB Stereo Camera Calibrator Toolbox workflow
 */
class StereoCalibrationProgram {
private:
    // 标定参数 (Calibration parameters)
    std::string leftImagePath;
    std::string rightImagePath;
    std::string outputPath;
    
    // 棋盘格参数 (Chessboard parameters)
    int boardWidth;
    int boardHeight;
    float squareSize;
    
    // 图像参数 (Image parameters)
    int imageWidth;
    int imageHeight;
    
public:
    /**
     * 构造函数 (Constructor)
     * @param leftPath 左图像路径 (Left image path)
     * @param rightPath 右图像路径 (Right image path)
     * @param outPath 输出路径 (Output path)
     * @param bWidth 棋盘格宽度内角点数 (Chessboard width inner corners)
     * @param bHeight 棋盘格高度内角点数 (Chessboard height inner corners)
     * @param sSize 方格边长(米) (Square size in meters)
     * @param imgWidth 目标图像宽度 (Target image width)
     * @param imgHeight 目标图像高度 (Target image height)
     */
    StereoCalibrationProgram(const std::string& leftPath, const std::string& rightPath, 
                           const std::string& outPath, int bWidth = 9, int bHeight = 6, 
                           float sSize = 0.0082f, int imgWidth = 3264, int imgHeight = 2448);
    
    /**
     * 执行完整的双目标定流程 (Execute complete stereo calibration workflow)
     * @return true if successful, false otherwise
     */
    bool runCalibration();
    
    // 获取参数的方法 (Parameter getter methods)
    const std::string& getLeftImagePath() const { return leftImagePath; }
    const std::string& getRightImagePath() const { return rightImagePath; }
    const std::string& getOutputPath() const { return outputPath; }
    int getBoardWidth() const { return boardWidth; }
    int getBoardHeight() const { return boardHeight; }
    float getSquareSize() const { return squareSize; }
    int getImageWidth() const { return imageWidth; }
    int getImageHeight() const { return imageHeight; }
    
private:
    // 私有辅助方法 (Private helper methods)
    bool validateInputPaths();
    void createOutputDirectories();
    bool preprocessImages();
    bool detectCorners();
    bool calibrateMonocular();
    bool calibrateStereo();
    bool rectifyImages();
    bool generate3DModel();
    void printResults();
};

#endif // STEREO_CALIBRATION_PROGRAM_H