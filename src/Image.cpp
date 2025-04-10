/**
 * @file Image.cpp
 * @brief Implementation of the Image class for loading, manipulating, and saving images.
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
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

/**
 * @brief Constructs an Image object by loading an image from a file.
 * 
 * @param filepath The path to the image file.
 * 
 * @throws std::runtime_error If the image fails to load.
 */
Image::Image(const char* filepath) {
    data = stbi_load(filepath, &width, &height, &channels, 0);
    if (!data) {
        throw std::runtime_error("Failed to load image from filepath: " + std::string(filepath));
    }
}

/**
 * @brief Constructs an Image object from raw image data.
 * 
 * @param input Pointer to the input image data.
 * @param w Image width.
 * @param h Image height.
 * @param c Number of channels.
 */
Image::Image(unsigned char* input, int w, int h, int c) 
    : width(w), height(h), channels(c) {
    data = new unsigned char[w * h * c];
    std::copy(input, input + (w * h * c), data);
}

/**
 * @brief Destructor for the Image class, freeing allocated memory.
 */
Image::~Image() {
    if (data) {
        stbi_image_free(data);
    }
}

/**
 * @brief Writes an image to a file in PNG format.
 * 
 * @param img The image object to write.
 * @param filepath The path where the image should be saved.
 * 
 * @throws std::runtime_error If writing the image fails.
 */
void Image::WriteImage(const Image& img, const char* filepath) {
    if (!img.data) {
        throw std::runtime_error("Error: No image data to save.");
    }
    int success = stbi_write_png(filepath, img.width, img.height, img.channels, img.data, 0); // why 0?
    if (!success) {
        throw std::runtime_error("Error: Failed to write image to " + std::string(filepath));
    }
    std::cout << "Successfully exported transformed image to " + std::string(filepath) << std::endl;
}

/**
 * @brief Sets the image data.
 * 
 * @param newData Pointer to the new image data.
 * 
 * @throws std::runtime_error If newData is null.
 */
void Image::setData(const unsigned char* newData) {
    if (!newData) {
        throw std::runtime_error("Error: Attempted to set null image data.");
    }

    std::copy(newData, newData + (width * height * channels), data);
}

/**
 * @brief Sets the number of channels in the image.
 * 
 * @param newChannels The new number of channels (1, 3, or 4).
 * 
 * @throws std::invalid_argument If the number of channels is invalid.
 */
void Image::setChannels(int newChannels) {
    if (newChannels == channels) {
        return; // No change needed
    }
    if (newChannels <= 0 || newChannels > 4) { // 1 (Grayscale), 3 (RGB), 4 (RGBA)
        throw std::invalid_argument("Invalid number of channels");
    }
    channels = newChannels;
}

