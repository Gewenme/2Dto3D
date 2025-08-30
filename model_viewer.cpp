#include "model_viewer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace ModelViewer {

bool loadModel(const std::string& filename,
               std::vector<cv::Point3f>& points,
               std::vector<cv::Vec3b>& colors) {
    try {
        points.clear();
        colors.clear();

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open model file: " << filename << std::endl;
            return false;
        }

        std::string extension = filename.substr(filename.find_last_of(".") + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == "ply") {
            // Parse PLY format
            std::string line;
            int vertexCount = 0;
            bool headerEnded = false;

            // Parse header
            while (std::getline(file, line)) {
                if (line.find("element vertex") != std::string::npos) {
                    std::istringstream iss(line);
                    std::string temp1, temp2;
                    iss >> temp1 >> temp2 >> vertexCount;
                }
                if (line == "end_header") {
                    headerEnded = true;
                    break;
                }
            }

            if (!headerEnded || vertexCount == 0) {
                std::cerr << "Invalid PLY file format" << std::endl;
                return false;
            }

            // Parse vertices
            for (int i = 0; i < vertexCount && std::getline(file, line); i++) {
                std::istringstream iss(line);
                float x, y, z;
                int r, g, b;
                
                if (iss >> x >> y >> z >> r >> g >> b) {
                    points.push_back(cv::Point3f(x, y, z));
                    colors.push_back(cv::Vec3b(b, g, r));  // BGR format
                } else if (iss.clear(), iss.seekg(0), iss >> x >> y >> z) {
                    points.push_back(cv::Point3f(x, y, z));
                    colors.push_back(cv::Vec3b(255, 255, 255));  // Default white
                }
            }

        } else if (extension == "xyz") {
            // Parse XYZ format
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                float x, y, z;
                if (iss >> x >> y >> z) {
                    points.push_back(cv::Point3f(x, y, z));
                    colors.push_back(cv::Vec3b(255, 255, 255));  // Default white
                }
            }

        } else if (extension == "obj") {
            // Parse OBJ format (vertices only)
            std::string line;
            while (std::getline(file, line)) {
                if (line.substr(0, 2) == "v ") {
                    std::istringstream iss(line.substr(2));
                    float x, y, z;
                    if (iss >> x >> y >> z) {
                        points.push_back(cv::Point3f(x, y, z));
                        colors.push_back(cv::Vec3b(255, 255, 255));  // Default white
                    }
                }
            }

        } else {
            std::cerr << "Unsupported file format: " << extension << std::endl;
            return false;
        }

        file.close();
        std::cout << "Loaded " << points.size() << " points from: " << filename << std::endl;
        return !points.empty();

    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

bool displayModel(const std::vector<cv::Point3f>& points,
                  const std::vector<cv::Vec3b>& colors,
                  const std::string& windowName) {
    try {
        if (points.empty()) {
            std::cerr << "No points to display" << std::endl;
            return false;
        }

        // Get bounding box
        cv::Point3f minPoint, maxPoint;
        if (!getBoundingBox(points, minPoint, maxPoint)) {
            return false;
        }

        // Create a simple orthographic projection view
        int imageSize = 800;
        cv::Mat image = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

        float scaleX = imageSize / (maxPoint.x - minPoint.x);
        float scaleZ = imageSize / (maxPoint.z - minPoint.z);
        float scale = std::min(scaleX, scaleZ) * 0.9f;  // Leave some margin

        // Project points to XZ plane (top-down view)
        for (size_t i = 0; i < points.size(); i++) {
            int x = static_cast<int>((points[i].x - minPoint.x) * scale + imageSize * 0.05f);
            int y = static_cast<int>((points[i].z - minPoint.z) * scale + imageSize * 0.05f);
            
            if (x >= 0 && x < imageSize && y >= 0 && y < imageSize) {
                cv::Vec3b color = colors.size() > i ? colors[i] : cv::Vec3b(255, 255, 255);
                cv::circle(image, cv::Point(x, y), 1, cv::Scalar(color[0], color[1], color[2]), -1);
            }
        }

        // Save the image instead of displaying in headless environment
        std::string outputImagePath = "output/3d_model_visualization.jpg";
        bool saved = cv::imwrite(outputImagePath, image);
        if (saved) {
            std::cout << "3D model visualization saved to: " << outputImagePath << std::endl;
            std::cout << "Model contains " << points.size() << " points" << std::endl;
            
            // Print some sample points for verification
            std::cout << "Sample points (first 5):" << std::endl;
            for (int i = 0; i < std::min(5, (int)points.size()); i++) {
                std::cout << "  Point " << i << ": (" << points[i].x << ", " << points[i].y << ", " << points[i].z << ")" << std::endl;
            }
        }
        
        // Skip OpenCV display functions entirely in headless mode
        std::cout << "3D model processing completed successfully" << std::endl;

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error displaying model: " << e.what() << std::endl;
        return false;
    }
}

bool generateProjectionViews(const std::vector<cv::Point3f>& points,
                             const std::vector<cv::Vec3b>& colors,
                             const std::string& outputPath) {
    try {
        if (points.empty()) {
            return false;
        }

        // Create output directory
        std::filesystem::create_directories(outputPath);

        // Get bounding box
        cv::Point3f minPoint, maxPoint;
        if (!getBoundingBox(points, minPoint, maxPoint)) {
            return false;
        }

        int imageSize = 800;

        // Generate XY view (front view)
        cv::Mat xyView = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
        float scaleX = imageSize / (maxPoint.x - minPoint.x);
        float scaleY = imageSize / (maxPoint.y - minPoint.y);
        float xyScale = std::min(scaleX, scaleY) * 0.9f;

        for (size_t i = 0; i < points.size(); i++) {
            int x = static_cast<int>((points[i].x - minPoint.x) * xyScale + imageSize * 0.05f);
            int y = static_cast<int>((maxPoint.y - points[i].y) * xyScale + imageSize * 0.05f);  // Flip Y
            
            if (x >= 0 && x < imageSize && y >= 0 && y < imageSize) {
                cv::Vec3b color = colors.size() > i ? colors[i] : cv::Vec3b(255, 255, 255);
                cv::circle(xyView, cv::Point(x, y), 1, cv::Scalar(color[0], color[1], color[2]), -1);
            }
        }

        // Generate XZ view (top view)
        cv::Mat xzView = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
        float scaleZ = imageSize / (maxPoint.z - minPoint.z);
        float xzScale = std::min(scaleX, scaleZ) * 0.9f;

        for (size_t i = 0; i < points.size(); i++) {
            int x = static_cast<int>((points[i].x - minPoint.x) * xzScale + imageSize * 0.05f);
            int z = static_cast<int>((points[i].z - minPoint.z) * xzScale + imageSize * 0.05f);
            
            if (x >= 0 && x < imageSize && z >= 0 && z < imageSize) {
                cv::Vec3b color = colors.size() > i ? colors[i] : cv::Vec3b(255, 255, 255);
                cv::circle(xzView, cv::Point(x, z), 1, cv::Scalar(color[0], color[1], color[2]), -1);
            }
        }

        // Generate YZ view (side view)
        cv::Mat yzView = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
        float yzScale = std::min(scaleY, scaleZ) * 0.9f;

        for (size_t i = 0; i < points.size(); i++) {
            int y = static_cast<int>((maxPoint.y - points[i].y) * yzScale + imageSize * 0.05f);  // Flip Y
            int z = static_cast<int>((points[i].z - minPoint.z) * yzScale + imageSize * 0.05f);
            
            if (y >= 0 && y < imageSize && z >= 0 && z < imageSize) {
                cv::Vec3b color = colors.size() > i ? colors[i] : cv::Vec3b(255, 255, 255);
                cv::circle(yzView, cv::Point(z, y), 1, cv::Scalar(color[0], color[1], color[2]), -1);
            }
        }

        // Save views
        cv::imwrite(outputPath + "/xy_view.jpg", xyView);
        cv::imwrite(outputPath + "/xz_view.jpg", xzView);
        cv::imwrite(outputPath + "/yz_view.jpg", yzView);

        std::cout << "Generated projection views in: " << outputPath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error generating projection views: " << e.what() << std::endl;
        return false;
    }
}

bool getBoundingBox(const std::vector<cv::Point3f>& points,
                    cv::Point3f& minPoint,
                    cv::Point3f& maxPoint) {
    if (points.empty()) {
        return false;
    }

    minPoint = maxPoint = points[0];

    for (const auto& point : points) {
        minPoint.x = std::min(minPoint.x, point.x);
        minPoint.y = std::min(minPoint.y, point.y);
        minPoint.z = std::min(minPoint.z, point.z);

        maxPoint.x = std::max(maxPoint.x, point.x);
        maxPoint.y = std::max(maxPoint.y, point.y);
        maxPoint.z = std::max(maxPoint.z, point.z);
    }

    return true;
}

bool generateMesh(const std::vector<cv::Point3f>& points,
                  std::vector<cv::Vec3i>& triangles) {
    // Simple mesh generation would require more complex algorithms
    // like Delaunay triangulation or Poisson surface reconstruction
    // For now, this is a placeholder
    
    std::cerr << "Mesh generation not implemented yet" << std::endl;
    return false;
}

bool saveModelStatistics(const std::vector<cv::Point3f>& points,
                        const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to create statistics file: " << filename << std::endl;
            return false;
        }

        if (points.empty()) {
            file << "No points in model" << std::endl;
            file.close();
            return true;
        }

        cv::Point3f minPoint, maxPoint;
        getBoundingBox(points, minPoint, maxPoint);

        // Calculate center
        cv::Point3f center;
        for (const auto& point : points) {
            center.x += point.x;
            center.y += point.y;
            center.z += point.z;
        }
        center.x /= points.size();
        center.y /= points.size();
        center.z /= points.size();

        // Calculate distances and statistics
        float totalDistance = 0;
        float maxDistance = 0;
        for (const auto& point : points) {
            float dist = std::sqrt(std::pow(point.x - center.x, 2) + 
                                 std::pow(point.y - center.y, 2) + 
                                 std::pow(point.z - center.z, 2));
            totalDistance += dist;
            maxDistance = std::max(maxDistance, dist);
        }

        file << "3D Model Statistics" << std::endl;
        file << "===================" << std::endl;
        file << "Total points: " << points.size() << std::endl;
        file << "Bounding box:" << std::endl;
        file << "  Min: (" << minPoint.x << ", " << minPoint.y << ", " << minPoint.z << ")" << std::endl;
        file << "  Max: (" << maxPoint.x << ", " << maxPoint.y << ", " << maxPoint.z << ")" << std::endl;
        file << "  Size: (" << (maxPoint.x - minPoint.x) << ", " << (maxPoint.y - minPoint.y) << ", " << (maxPoint.z - minPoint.z) << ")" << std::endl;
        file << "Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
        file << "Average distance from center: " << (totalDistance / points.size()) << std::endl;
        file << "Maximum distance from center: " << maxDistance << std::endl;

        file.close();
        std::cout << "Model statistics saved to: " << filename << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving model statistics: " << e.what() << std::endl;
        return false;
    }
}

} // namespace ModelViewer