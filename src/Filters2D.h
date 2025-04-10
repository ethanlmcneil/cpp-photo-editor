/*
 * @file Filters2D.h
 *
 * Group Members:
 * - Jiaqi    (GitHub: esemsc-jc1424)
 * - Daicong  (GitHub: esemsc-c730ef50)
 * - Ida      (GitHub: esemsc-ifc24)
 * - Zhuyi    (GitHub: esemsc-zf1124)
 * - Dany     (GitHub: esemsc-dh324)
 * - Ethan    (GitHub: edsml-elm224)
 * - Keyun    (GitHub: esemsc-km824)
 */

#ifndef FILTERS2D_h
#define FILTERS2D_h

#include <vector>
#include <iostream>
#include <string>
#include "Image.h"

/**
 * @enum EdgeDetectorType
 * @brief Enum for different edge detection algorithms.
 */
enum class EdgeDetectorType {
    Sobel, 
    Prewitt, 
    Scharr, 
    RobertsCross
};

/**
 * @class Filters2D
 * @brief Class that implements various 2D image filters such as blur, brightness, sharpening, and edge detection.
 */
class Filters2D {
public:
    /**
     * @brief Default constructor.
     */
    Filters2D();
    
    /**
     * @brief Applies greyscale filter to the image.
     * @param img The image object to apply the filter on.
     */
    void apply_Greyscale(Image& img);
    
    /**
     * @brief Adjusts the brightness of the image.
     * @param img The image object to apply the filter on.
     * @param value The value to adjust brightness by (default is 0).
     */
    void apply_Brightness(Image& img, int value = 0);
    
    /**
     * @brief Applies histogram equalization to the image.
     * @param img The image object to apply the filter on.
     * @param space The color space for histogram equalization (e.g., RGB, HSV).
     */
    void apply_Histogram_Equalisation(Image& img, const std::string &space);
    
    /**
     * @brief Applies a threshold filter to the image.
     * @param img The image object to apply the filter on.
     * @param threshold The threshold value for binarization.
     * @param space The color space for thresholding (e.g., RGB, HSV).
     */
    void Threshold(Image& img, int threshold, const std::string &space);
    
    /**
     * @brief Applies salt-and-pepper noise to the image.
     * @param img The image object to apply the noise on.
     * @param noise_prob The probability of salt-and-pepper noise.
     */
    void apply_Salt_and_Pepper_Noise(Image& img, float noise_prob);

    /**
     * @brief Applies box blur filter to the image.
     * @param img The image object to apply the filter on.
     * @param kernelSize The size of the kernel (default is 3).
     */
    void boxBlur(Image& img, int kernelSize = 3);
    
    /**
     * @brief Applies Gaussian blur filter to the image.
     * @param img The image object to apply the filter on.
     * @param kernelSize The size of the kernel (default is 3).
     * @param sigma The standard deviation for Gaussian distribution (default is 2.0f).
     */
    void gaussianBlur(Image& img, int kernelSize = 3, float sigma = 2.0f);
    
    /**
     * @brief Applies median blur filter to the image.
     * @param img The image object to apply the filter on.
     * @param kernelSize The size of the kernel (default is 3).
     */
    void medianBlur(Image& img, int kernelSize = 3);

    /**
     * @brief Sharpens the image using a sharpening filter.
     * @param img The image object to apply the filter on.
     */
    void Sharpen(Image& img);

    /**
     * @brief Detects edges in the image using a selected edge detection algorithm.
     * @param img The image object to apply the filter on.
     * @param type The edge detection algorithm to use.
     */
    void DetectEdges(Image& img, EdgeDetectorType type);

    /**
     * @brief Converts string to the corresponding EdgeDetectorType enum.
     * @param st The string representing the edge detector type.
     * @return The corresponding EdgeDetectorType enum.
     */
    EdgeDetectorType GetEdgeDetectorType(const std::string& st);

private:
    /**
     * @brief Converts RGB to HSV color space.
     * @param r The red channel.
     * @param g The green channel.
     * @param b The blue channel.
     * @param h The hue channel (output).
     * @param s The saturation channel (output).
     * @param v The value channel (output).
     */
    void RGBtoHSV(unsigned char r, unsigned char g, unsigned char b, float& h, float& s, float& v);
    
    /**
     * @brief Converts HSV to RGB color space.
     * @param h The hue channel.
     * @param s The saturation channel.
     * @param v The value channel.
     * @param r The red channel (output).
     * @param g The green channel (output).
     * @param b The blue channel (output).
     */
    void HSVtoRGB(float h, float s, float v, unsigned char& r, unsigned char& g, unsigned char& b);
    
    /**
     * @brief Converts RGB to HSL color space.
     * @param r The red channel.
     * @param g The green channel.
     * @param b The blue channel.
     * @param h The hue channel (output).
     * @param s The saturation channel (output).
     * @param l The lightness channel (output).
     */
    void RGBtoHSL(unsigned char r, unsigned char g, unsigned char b, float& h, float& s, float& l);
    
    /**
     * @brief Converts HSL to RGB color space.
     * @param h The hue channel.
     * @param s The saturation channel.
     * @param l The lightness channel.
     * @param r The red channel (output).
     * @param g The green channel (output).
     * @param b The blue channel (output).
     */
    void HSLtoRGB(float h, float s, float l, unsigned char& r, unsigned char& g, unsigned char& b);
    
    /**
     * @brief Validates the kernel size to ensure it's odd and greater than 1.
     * @param kernelSize The kernel size to validate.
     */
    void validateKernelSize(int& kernelSize); 
    
    /**
     * @brief Implements the quick sort algorithm to sort a vector.
     * @param vec The vector to be sorted.
     * @param low The low index for partitioning.
     * @param high The high index for partitioning.
     */
    template <typename T>
    void quickSort(std::vector<T>& vec, int low, int high);
    
    /**
     * @brief Partitions the vector for the quick sort algorithm.
     * @param vec The vector to be partitioned.
     * @param low The low index.
     * @param high The high index.
     * @return The partition index.
     */
    template <typename T>
    int partition(std::vector<T>& vec, int low, int high);
    
    /**
     * @brief Custom sorting algorithm for the vector.
     * @param vec The vector to be sorted.
     */
    template <typename T>
    void customSort(std::vector<T>& vec);
};

#endif
