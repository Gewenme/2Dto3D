#ifndef IMAGE_RESIZE_H
#define IMAGE_RESIZE_H

#include <string>

namespace ImageUtils {
    // Interpolation types
    enum InterpolationType {
        LINEAR = 2,
        CUBIC = 1,
        NEAREST = 0
    };

    // Options for image resizing
    class ResizeOptions {
    public:
        InterpolationType interpolationType;
        bool enableSharpening;
        bool reduceNoise;
        bool preserveAspectRatio;

        explicit ResizeOptions(InterpolationType type = LINEAR);
    };

    /**
     * Resize images in a folder to specified dimensions
     * @param inputFolder Path to input image folder
     * @param outputFolder Path to output image folder (will be created if not exists)
     * @param width Target width in pixels
     * @param height Target height in pixels
     * @param interpolationType Interpolation method (0=nearest, 1=cubic, 2=linear)
     * @return true if successful, false otherwise
     */
    bool resizeImage(const std::string& inputFolder,
                     const std::string& outputFolder,
                     int width,
                     int height,
                     int interpolationType);

    /**
     * Resize images in a folder with advanced options
     * @param inputFolder Path to input image folder
     * @param outputFolder Path to output image folder (will be created if not exists)
     * @param width Target width in pixels
     * @param height Target height in pixels
     * @param options Resize options
     * @return true if successful, false otherwise
     */
    bool resizeImage(const std::string& inputFolder,
                     const std::string& outputFolder,
                     int width,
                     int height,
                     const ResizeOptions& options);
}

#endif // IMAGE_RESIZE_H