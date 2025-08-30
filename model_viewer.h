#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

namespace ModelViewer {

    /**
     * Load 3D model from file
     * @param filename Path to 3D model file (PLY, OBJ, XYZ)
     * @param points Output vector of 3D points
     * @param colors Output vector of point colors (if available)
     * @return true if loaded successfully
     */
    bool loadModel(const std::string& filename,
                   std::vector<cv::Point3f>& points,
                   std::vector<cv::Vec3b>& colors);

    /**
     * Display 3D model in a simple viewer
     * @param points 3D point cloud
     * @param colors Point colors
     * @param windowName Window title
     * @return true if displayed successfully
     */
    bool displayModel(const std::vector<cv::Point3f>& points,
                      const std::vector<cv::Vec3b>& colors,
                      const std::string& windowName = "3D Model Viewer");

    /**
     * Generate 2D projection views of 3D model
     * @param points 3D point cloud
     * @param colors Point colors
     * @param outputPath Path to save projection images
     * @return true if successful
     */
    bool generateProjectionViews(const std::vector<cv::Point3f>& points,
                                 const std::vector<cv::Vec3b>& colors,
                                 const std::string& outputPath);

    /**
     * Get bounding box of 3D model
     * @param points 3D point cloud
     * @param minPoint Output minimum point
     * @param maxPoint Output maximum point
     * @return true if successful
     */
    bool getBoundingBox(const std::vector<cv::Point3f>& points,
                        cv::Point3f& minPoint,
                        cv::Point3f& maxPoint);

    /**
     * Generate mesh from point cloud (simple triangulation)
     * @param points 3D point cloud
     * @param triangles Output triangle indices
     * @return true if successful
     */
    bool generateMesh(const std::vector<cv::Point3f>& points,
                      std::vector<cv::Vec3i>& triangles);

    /**
     * Save model statistics to file
     * @param points 3D point cloud
     * @param filename Output filename for statistics
     * @return true if successful
     */
    bool saveModelStatistics(const std::vector<cv::Point3f>& points,
                            const std::string& filename);
}

#endif // MODEL_VIEWER_H