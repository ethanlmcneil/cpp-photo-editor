/**
 * @file Filters2D.cpp
 * @brief Contains 2D image processing filter implementations.
 * 
 * This file includes various functions that process 2D images, such as applying greyscale conversion,
 * adjusting brightness, performing histogram equalization, and applying thresholding in HSL/HSV space.
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
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include "Filters2D.h"
#include "Image.h"

#include <iostream>

Filters2D::Filters2D(){}

/**
 * @brief Converts an image to greyscale by removing the alpha channel and computing luminance.
 * 
 * The conversion uses standard luminance weights:
 * - Red: 0.2126
 * - Green: 0.7152
 * - Blue: 0.0722
 * 
 * @param img Reference to the image object to be modified.
 */
void Filters2D::apply_Greyscale(Image& img) {
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    // Create a copy of image data for safe modification
    std::vector<unsigned char> grey(width * height);

    for (int j = 0; j < width * height; ++j) {
        const int src_idx = j * channels;
        grey[j] = static_cast<unsigned char>(
            0.2126 * data[src_idx] +
            0.7152 * (channels > 1 ? data[src_idx + 1] : 0) +
            0.0722 * (channels > 2 ? data[src_idx + 2] : 0)
        );
    }
    // Update the image using the setter function
    img.setData(grey.data());
    img.setChannels(1);
}

/**
 * @brief Adjusts the brightness of an image by a given value. 
 * 
 * If the value is set to 0, an automatic adjustment is applied based on image luminance.
 * 
 * @param img Reference to the image object.
 * @param value Brightness adjustment value (-255 to 255). If 0, auto-adjustment is applied.
 */
void Filters2D::apply_Brightness(Image& img, int value) {
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    const int total_pixels = width * height;
    std::vector<unsigned char> output(data, data + (total_pixels * channels));

    // Automatic pattern judgment
    if (value == 0) {
        long sum = 0;
        for (int j = 0; j < total_pixels * channels; j += channels) {
            sum += (channels >= 3) ?
                0.2126 * output[j] + 0.7152 * output[j + 1] + 0.0722 * output[j + 2] :
                output[j];
        }
        value = 128 - (sum / total_pixels);
        value = std::clamp(value, -255, 255); // Limit value range
    }

    // Application brightness adjustment
    for (int j = 0; j < total_pixels * channels; ++j) {
        const int ch = j % channels;
        if (channels == 4 && ch == 3) continue; // keep alpha channel
        output[j] = std::clamp(output[j] + value, 0, 255);
    }
    img.setData(output.data());
}

/**
 * @brief Applies histogram equalization to enhance image contrast.
 * 
 * The method supports grayscale images and RGB images (via HSL/HSV transformations).
 * 
 * @param img Reference to the image object.
 * @param space Defines the color space for equalization ("HSL" or "HSV").
 */
void Filters2D::apply_Histogram_Equalisation(Image& img, const std::string &space)
{
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();
    const int total_pixels = width * height;

    if (channels == 1) {
        // Grayscale Image Histogram Equalization
        int hist[256] = {0}, cdf[256] = {0};
        for (int j = 0; j < total_pixels; ++j)
            hist[data[j]]++;
        cdf[0] = hist[0];
        for (int j = 1; j < 256; ++j)
            cdf[j] = cdf[j - 1] + hist[j];
        const int cdf_min = *std::min_element(cdf, cdf + 256);
        std::vector<unsigned char> eq(total_pixels);
        for (int j = 0; j < total_pixels; ++j)
            eq[j] = (255 * (cdf[data[j]] - cdf_min)) / (total_pixels - cdf_min);
        img.setData(eq.data());
    }
    else if (channels == 3 || channels == 4) {
        std::vector<float> values(total_pixels);
        
        // Choose whether to operate on Lightness (HSL) or Value (HSV)
        if (space == "HSL") {
            // Convert RGB to HSL and extract Lightness (L)
            for (int j = 0; j < total_pixels; ++j) {
                float h, s, l;
                RGBtoHSL(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, l);
                values[j] = l * 255;
            }
        }
        else if (space == "HSV") {
            // Convert RGB to HSV and extract Value (V)
            for (int j = 0; j < total_pixels; ++j) {
                float h, s, v;
                RGBtoHSV(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, v);
                values[j] = v * 255;
            }
        }
        else {
            std::cerr << "Unknown equalization space: " << space << ", defaulting to HSL.\n";
            // Default to HSL
            for (int j = 0; j < total_pixels; ++j) {
                float h, s, l;
                RGBtoHSL(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, l);
                values[j] = l * 255;
            }
        }

        // Compute histogram and CDF
        int hist[256] = {0}, cdf[256] = {0};
        for (float v : values)
            hist[static_cast<int>(v)]++;
        std::partial_sum(hist, hist + 256, cdf);
        const int cdf_min = *std::min_element(cdf, cdf + 256);

        // Apply equalisation and convert back to RGB
        std::vector<unsigned char> eq_img(total_pixels * channels);
        for (int j = 0; j < total_pixels; ++j) {
            float h, s, l_or_v;
            if (space == "HSL") {
                RGBtoHSL(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, l_or_v);
            } else {
                RGBtoHSV(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, l_or_v);
            }

            l_or_v = cdf[static_cast<int>(l_or_v * 255)] / 255.0f;
            l_or_v = (l_or_v * 255 - cdf_min) / (total_pixels - cdf_min);
            unsigned char r, g, b;
            
            if (space == "HSL") {
                HSLtoRGB(h, s, l_or_v, r, g, b);
            } else {
                HSVtoRGB(h, s, l_or_v, r, g, b);
            }

            eq_img[j * channels]     = r;
            eq_img[j * channels + 1] = g;
            eq_img[j * channels + 2] = b;

            // Preserve alpha channel if present
            if (channels == 4) {
                eq_img[j * channels + 3] = data[j * channels + 3];
            }
        }
        img.setData(eq_img.data());
    }
    else {
        std::cerr << "Unsupported number of channels: " << channels << std::endl;
    }
}

/**
 * @brief Applies thresholding in the specified color space (HSL/HSV).
 * 
 * @param img The input image to be thresholded.
 * @param threshold The threshold value (0-255).
 * @param space The color space to use ("HSL" or "HSV").
 */
void Filters2D::Threshold(Image& img, int threshold, const std::string &space)
{
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();
    const int total_pixels = width * height;

    if (channels == 1) {
        // Grayscale Image Thresholding
        std::vector<unsigned char> thresh(total_pixels);
        for (int j = 0; j < total_pixels; ++j)
            thresh[j] = (data[j] < threshold) ? 0 : 255;
        img.setData(thresh.data());
    }
    else if (channels == 3 || channels == 4) {  
        // Handle both RGB and RGBA images
        std::vector<unsigned char> thresh(total_pixels * channels);

        if (space == "HSL") {
            for (int j = 0; j < total_pixels; ++j) {
                float h, s, l;
                RGBtoHSL(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, l);
                l = (l * 255 < threshold) ? 0.0f : 1.0f;
                unsigned char r, g, b;
                HSLtoRGB(h, s, l, r, g, b);
                thresh[j * channels]     = r;
                thresh[j * channels + 1] = g;
                thresh[j * channels + 2] = b;

                if (channels == 4) {
                    thresh[j * channels + 3] = data[j * channels + 3];
                }
            }
        }
        else if (space == "HSV") {
            for (int j = 0; j < total_pixels; ++j) {
                float h, s, v;
                RGBtoHSV(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, v);
                v = (v * 255 < threshold) ? 0.0f : 1.0f;
                s = 0.0f;

                unsigned char r, g, b;
                HSVtoRGB(h, s, v, r, g, b);
                thresh[j * channels]     = r;
                thresh[j * channels + 1] = g;
                thresh[j * channels + 2] = b;

                if (channels == 4) {
                    thresh[j * channels + 3] = data[j * channels + 3];
                }
            }
        }
        else {
            std::cerr << "Unknown threshold space: " << space << ", defaulting to HSL.\n";
            for (int j = 0; j < total_pixels; ++j) {
                float h, s, l;
                RGBtoHSL(data[j * channels], data[j * channels + 1], data[j * channels + 2],
                         h, s, l);
                l = (l * 255 < threshold) ? 0.0f : 1.0f;
                unsigned char r, g, b;
                HSLtoRGB(h, s, l, r, g, b);
                thresh[j * channels]     = r;
                thresh[j * channels + 1] = g;
                thresh[j * channels + 2] = b;

                if (channels == 4) {
                    thresh[j * channels + 3] = data[j * channels + 3];
                }
            }
        }

        img.setData(thresh.data());
    }
    else {
        std::cerr << "Unsupported number of channels: " << channels << std::endl;
    }
}

/**
 * @brief Adds salt-and-pepper noise to an image.
 * 
 * @param img The input image.
 * @param noise_prob The probability of noise occurring (0-100%).
 */
void Filters2D::apply_Salt_and_Pepper_Noise(Image& img, float noise_prob) {
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    std::vector<unsigned char> noisyData(data, data + width * height * channels);

    const int total = width * height * channels;
    const int color_chs = (channels >= 3) ? 3 : 1;

    for (int j = 0; j < total; j += channels) {
        if (dis(gen) < noise_prob/100) {
            const unsigned char val = (dis(gen) < 0.5) ? 0 : 255;
            for (int ch = 0; ch < color_chs; ++ch)
                noisyData[j + ch] = val;
        }
    }
    img.setData(noisyData.data());
}

/**
 * @brief Applies a sharpening filter using the Laplacian kernel.
 * 
 * @param img The input image.
 */
void Filters2D::Sharpen(Image& img) {
    // Define the Laplacian Kernel
    int kernel[3][3] = {
        {  0, -1,  0 },
        { -1,  4, -1 },
        {  0, -1,  0 }
    };

    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    // Copy the original data to avoid modifying it while processing
    std::vector<unsigned char> output(data, data + width * height * channels);

    // Iterate over every pixel including edges
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < std::min(channels, 3); ++c) { // Ignore alpha if channels=4
                int sum = 0;

                // Apply the 3x3 convolution filter
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int neighbor_x = std::min(std::max(x + kx, 0), width - 1);
                        int neighbor_y = std::min(std::max(y + ky, 0), height - 1);
                        int index = (neighbor_y * width + neighbor_x) * channels + c;
                        sum += data[index] * kernel[ky + 1][kx + 1];
                    }
                }

                // Compute new pixel value and clamp it between 0-255
                int index = (y * width + x) * channels + c;
                output[index] = std::clamp(data[index] + sum, 0, 255);
            }
        }
    }
    // Update the image using the setter function
    img.setData(output.data());
}

/**
 * @brief Applies a box blur filter with a specified kernel size.
 * 
 * @param img The input image.
 * @param kernelSize The size of the blur kernel (must be an odd number).
 */
void Filters2D::boxBlur(Image& img, int kernelSize) {
    validateKernelSize(kernelSize);
    int halfKernel = kernelSize / 2;

    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    // Create a copy of image data for safe modification
    std::vector<unsigned char> output(data, data + width * height * channels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0;

            for (int ky = -halfKernel; ky <= halfKernel; ky++) {
                for (int kx = -halfKernel; kx <= halfKernel; kx++) {
                    int nx = std::min(std::max(x + kx, 0), width - 1);
                    int ny = std::min(std::max(y + ky, 0), height - 1);
                    int offset = (ny * width + nx) * channels;
                    r += data[offset];
                    g += data[offset + 1];
                    b += data[offset + 2];
                }
            }

            int offset = (y * width + x) * channels;
            output[offset] = r / (kernelSize * kernelSize);
            output[offset + 1] = g / (kernelSize * kernelSize);
            output[offset + 2] = b / (kernelSize * kernelSize);
        }
    }
    img.setData(output.data());
}

/**
 * @brief Applies a Gaussian blur filter.
 * 
 * @param img The input image.
 * @param kernelSize The size of the Gaussian kernel.
 * @param sigma The standard deviation of the Gaussian function.
 */
void Filters2D::gaussianBlur(Image& img, int kernelSize, float sigma) {
    validateKernelSize(kernelSize);
    int halfKernel = kernelSize / 2;
    std::vector<float> kernel(kernelSize * kernelSize);

    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    // Create a copy of image data for safe modification
    std::vector<unsigned char> output(data, data + width * height * channels);

    float sum = 0.0f;
    for (int y = -halfKernel; y <= halfKernel; y++) {
        for (int x = -halfKernel; x <= halfKernel; x++) {
            float weight = exp(-(x * x + y * y) / (2 * sigma * sigma));
            kernel[(y + halfKernel) * kernelSize + (x + halfKernel)] = weight;
            sum += weight;
        }
    }

    for (int i = 0; i < kernelSize * kernelSize; i++) {
        kernel[i] /= sum;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = 0, g = 0, b = 0;

            for (int ky = -halfKernel; ky <= halfKernel; ky++) {
                for (int kx = -halfKernel; kx <= halfKernel; kx++) {
                    int nx = std::min(std::max(x + kx, 0), width - 1);
                    int ny = std::min(std::max(y + ky, 0), height - 1);
                    int offset = (ny * width + nx) * channels;
                    int kernelOffset = (ky + halfKernel) * kernelSize + (kx + halfKernel);
                    float weight = kernel[kernelOffset];

                    r += data[offset] * weight;
                    g += data[offset + 1] * weight;
                    b += data[offset + 2] * weight;
                }
            }

            int offset = (y * width + x) * channels;
            output[offset] = r;
            output[offset + 1] = g;
            output[offset + 2] = b;
        }
    }

    img.setData(output.data());
}

/**
 * @brief Applies a median blur filter.
 * 
 * @param img The input image.
 * @param kernelSize The size of the median filter kernel.
 */
void Filters2D::medianBlur(Image& img, int kernelSize) {
    validateKernelSize(kernelSize);
    int halfKernel = kernelSize / 2;

    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    const unsigned char* data = img.getData();

    // Create a copy of image data for safe modification
    std::vector<unsigned char> output(data, data + width * height * channels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::vector<int> rVals, gVals, bVals;

            for (int ky = -halfKernel; ky <= halfKernel; ky++) {
                for (int kx = -halfKernel; kx <= halfKernel; kx++) {
                    int nx = std::min(std::max(x + kx, 0), width - 1);
                    int ny = std::min(std::max(y + ky, 0), height - 1);
                    int offset = (ny * width + nx) * channels;

                    rVals.push_back(data[offset]);
                    gVals.push_back(data[offset + 1]);
                    bVals.push_back(data[offset + 2]);
                }
            }

            // Use the custom sorting function
            customSort(rVals);
            customSort(gVals);
            customSort(bVals);

            int offset = (y * width + x) * channels;
            output[offset] = rVals[rVals.size() / 2];
            output[offset + 1] = gVals[gVals.size() / 2];
            output[offset + 2] = bVals[bVals.size() / 2];
        }
    }
    img.setData(output.data());
}

// Helper function to validate kernel size
void Filters2D::validateKernelSize(int& kernelSize) {
    if (kernelSize % 2 == 0) {
        kernelSize += 1; // Ensure kernel size is odd
    }
}

/**
 * @brief Detects edges using a specified edge detection algorithm.
 * 
 * @param img The input image.
 * @param type The edge detection method (Sobel, Prewitt, Scharr, RobertsCross).
 */
void Filters2D::DetectEdges(Image& img, EdgeDetectorType type) {
    int Gx[3][3], Gy[3][3];

    bool isRobertsCross = false; // Special case for Roberts' Cross

    int norm_fact = 1;  // Default
    switch (type) {
        case EdgeDetectorType::Sobel:
            Gx[0][0] = -1; Gx[0][1] =  0; Gx[0][2] =  1;
            Gx[1][0] = -2; Gx[1][1] =  0; Gx[1][2] =  2;
            Gx[2][0] = -1; Gx[2][1] =  0; Gx[2][2] =  1;

            Gy[0][0] = -1; Gy[0][1] = -2; Gy[0][2] = -1;
            Gy[1][0] =  0; Gy[1][1] =  0; Gy[1][2] =  0;
            Gy[2][0] =  1; Gy[2][1] =  2; Gy[2][2] =  1;

            norm_fact = 4;
            break;
        
        case EdgeDetectorType::Prewitt:
            Gx[0][0] = -1; Gx[0][1] =  0; Gx[0][2] =  1;
            Gx[1][0] = -1; Gx[1][1] =  0; Gx[1][2] =  1;
            Gx[2][0] = -1; Gx[2][1] =  0; Gx[2][2] =  1;

            Gy[0][0] = -1; Gy[0][1] = -1; Gy[0][2] = -1;
            Gy[1][0] =  0; Gy[1][1] =  0; Gy[1][2] =  0;
            Gy[2][0] =  1; Gy[2][1] =  1; Gy[2][2] =  1;

            norm_fact = 3;
            break;

        case EdgeDetectorType::Scharr:
            Gx[0][0] = -3; Gx[0][1] =  0; Gx[0][2] =  3;
            Gx[1][0] = -10; Gx[1][1] =  0; Gx[1][2] = 10;
            Gx[2][0] = -3; Gx[2][1] =  0; Gx[2][2] =  3;

            Gy[0][0] = -3; Gy[0][1] = -10; Gy[0][2] = -3;
            Gy[1][0] =  0; Gy[1][1] =   0; Gy[1][2] =  0;
            Gy[2][0] =  3; Gy[2][1] =  10; Gy[2][2] =  3;
            norm_fact = 16;
            break;

        case EdgeDetectorType::RobertsCross:
            isRobertsCross = true; // Use special 2x2 kernel case
            norm_fact = 1;
            break;
    }

    int width = img.getWidth();
    int height = img.getHeight();
    const unsigned char* data = img.getData();

    std::vector<unsigned char> output(width * height);

    // Apply convolution
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int Gx_sum = 0, Gy_sum = 0;

            if (!isRobertsCross) {
                // Apply 3x3 convolution for Sobel, Prewitt, Scharr
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int neighbor_x = std::min(std::max(x + kx, 0), width - 1);
                        int neighbor_y = std::min(std::max(y + ky, 0), height - 1);
                        int index = neighbor_y * width + neighbor_x;
                        
                        Gx_sum += data[index] * Gx[ky + 1][kx + 1];
                        Gy_sum += data[index] * Gy[ky + 1][kx + 1];
                    }
                }

            } else {
                // Apply 2x2 Roberts Cross filter
                int index1 = y * width + x;
                int index2 = (y + 1) * width + (x + 1);
                int index3 = (y + 1) * width + x;
                int index4 = y * width + (x + 1);

                Gx_sum = data[index1] - data[index2]; // G1 kernel
                Gy_sum = data[index3] - data[index4]; // G2 kernel
            }

            // Compute final edge magnitude
            int edge_strength = static_cast<int>(std::sqrt(Gx_sum * Gx_sum + Gy_sum * Gy_sum));

            edge_strength = static_cast<int>(edge_strength / norm_fact);
            output[y * width + x] = std::clamp(edge_strength, 0, 255);
        }
    }
    img.setData(output.data());
}

/**
 * @brief Converts a string to an EdgeDetectorType enum.
 * 
 * @param st The string representing the edge detector type.
 * @return EdgeDetectorType The corresponding enum value.
 */
EdgeDetectorType Filters2D::GetEdgeDetectorType(const std::string& st) {
    if (st == "Sobel") return EdgeDetectorType::Sobel;
    if (st == "Prewitt") return EdgeDetectorType::Prewitt;
    if (st == "Scharr") return EdgeDetectorType::Scharr;
    if (st == "RobertsCross") return EdgeDetectorType::RobertsCross;
    
    std::cerr << "WARNING: Unknown edge detection type: " << st << ", defaulting to Sobel\n";
    return EdgeDetectorType::Sobel;
}

/**
 * @brief Converts RGB values to HSV color space.
 * 
 * @param r Red channel value (0-255).
 * @param g Green channel value (0-255).
 * @param b Blue channel value (0-255).
 * @param h Output hue (0-360).
 * @param s Output saturation (0-1).
 * @param v Output value (0-1).
 */
void Filters2D::RGBtoHSV(unsigned char r, unsigned char g, unsigned char b, float& h, float& s, float& v) {
    float fr = r / 255.0f, fg = g / 255.0f, fb = b / 255.0f;
    float cmax = std::max({fr, fg, fb});
    float cmin = std::min({fr, fg, fb});
    float delta = cmax - cmin;

    v = cmax;
    if (delta < 1e-5) {
        h = s = 0;
    } else {
        s = delta / cmax;
        if (cmax == fr) h = fmod((fg - fb) / delta, 6);
        else if (cmax == fg) h = (fb - fr) / delta + 2;
        else h = (fr - fg) / delta + 4;
        h /= 6.0;
    }
}

/**
 * @brief Converts HSV values to RGB color space.
 * 
 * @param h Hue (0-360).
 * @param s Saturation (0-1).
 * @param v Value (0-1).
 * @param r Output red channel value (0-255).
 * @param g Output green channel value (0-255).
 * @param b Output blue channel value (0-255).
 */
void Filters2D::HSVtoRGB(float h, float s, float v,
                                unsigned char& r, unsigned char& g, unsigned char& b) {
    float c = v * s;
    float x = c * (1 - std::abs(std::fmod(h * 6, 2) - 1));
    float m = v - c;

    float fr, fg, fb;
    if (h < 1/6.0f)      { fr = c; fg = x; fb = 0; }
    else if (h < 2/6.0f) { fr = x; fg = c; fb = 0; }
    else if (h < 3/6.0f) { fr = 0; fg = c; fb = x; }
    else if (h < 4/6.0f) { fr = 0; fg = x; fb = c; }
    else if (h < 5/6.0f) { fr = x; fg = 0; fb = c; }
    else                  { fr = c; fg = 0; fb = x; }

    r = (fr + m) * 255;
    g = (fg + m) * 255;
    b = (fb + m) * 255;
}

/**
 * @brief Converts RGB values to HSL color space.
 * 
 * @param r Red channel value (0-255).
 * @param g Green channel value (0-255).
 * @param b Blue channel value (0-255).
 * @param h Output hue (0-360).
 * @param s Output saturation (0-1).
 * @param l Output lightness (0-1).
 */
void Filters2D::RGBtoHSL(unsigned char r, unsigned char g, unsigned char b,
                                float& h, float& s, float& l) {
    float fr = r/255.0f, fg = g/255.0f, fb = b/255.0f;
    float cmax = std::max({fr, fg, fb});
    float cmin = std::min({fr, fg, fb});
    float delta = cmax - cmin;

    l = (cmax + cmin) / 2;
    if (delta < 1e-5) {
        h = s = 0;
    } else {
        s = (l > 0.5) ? delta/(2 - cmax - cmin) : delta/(cmax + cmin);
        if (cmax == fr)      h = (fg - fb)/delta + (fg < fb ? 6 : 0);
        else if (cmax == fg) h = (fb - fr)/delta + 2;
        else                 h = (fr - fg)/delta + 4;
        h /= 6;
    }
}

/**
 * @brief Converts HSL values to RGB color space.
 * 
 * @param h Hue (0-360).
 * @param s Saturation (0-1).
 * @param l Lightness (0-1).
 * @param r Output red channel value (0-255).
 * @param g Output green channel value (0-255).
 * @param b Output blue channel value (0-255).
 */
void Filters2D::HSLtoRGB(float h, float s, float l,
                                unsigned char& r, unsigned char& g, unsigned char& b) {
    auto hue2rgb = [](float p, float q, float t) {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1/6.0f) return p + (q - p) * 6 * t;
        if (t < 0.5f) return q;
        if (t < 2/3.0f) return p + (q - p) * (2/3.0f - t) * 6;
        return p;
    };

    float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    r = hue2rgb(p, q, h + 1/3.0f) * 255;
    g = hue2rgb(p, q, h) * 255;
    b = hue2rgb(p, q, h - 1/3.0f) * 255;
}
 
// Quick sort function
template <typename T>
void Filters2D::quickSort(std::vector<T>& vec, int low, int high) {
    if (low < high) {
        int pi = partition(vec, low, high);
        quickSort(vec, low, pi - 1);  // Left subarray
        quickSort(vec, pi + 1, high); // Right subarray
    }
}

// Partition function, returns the position of the pivot element after partitioning
template <typename T>
int Filters2D::partition(std::vector<T>& vec, int low, int high) {
    T pivot = vec[high];  // Choose the rightmost element as the pivot
    int i = low - 1;  // i is the right boundary of elements smaller than the pivot

    for (int j = low; j < high; j++) {
        if (vec[j] < pivot) {
            i++;
            std::swap(vec[i], vec[j]);
        }
    }
    std::swap(vec[i + 1], vec[high]);
    return i + 1;
}

// Sorting interface
template <typename T>
void Filters2D::customSort(std::vector<T>& vec) {
    quickSort(vec, 0, vec.size() - 1);
}
