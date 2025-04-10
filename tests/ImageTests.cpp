#include "ImageTests.h"
#include "../src/stb_image.h"
#include "../src/stb_image_write.h"

#include <string>
#include <vector>

ImageTests::ImageTests() : filepath("../Images/small.png"), img(filepath) {}

void ImageTests::testLoadUsingFilePath() {
    Image new_img(filepath);
    if (new_img.getData() == nullptr) {
        throw std::runtime_error("Image Data should not be null");
    }
    if (new_img.getWidth() <= 0) {
        throw std::runtime_error("Image Width should be greater than 0");
    }
    if (new_img.getHeight() <= 0) {
        throw std::runtime_error("Image Height should be greater than 0");
    }
    if (new_img.getChannels() <= 0) {
        throw std::runtime_error("Image Channels should be greater than 0");
    }
}

void ImageTests::testGetWidth() {
    int width = this->img.getWidth();
    if (width != 20) {
        throw std::runtime_error("Width is expected to be 20, but found " + std::to_string(width));
    }
}

void ImageTests::testGetHeight() {
    int height = this->img.getHeight();
    if (height != 20) {
        throw std::runtime_error("Height is expected to be 20, but found " + std::to_string(height));
    }
}

void ImageTests::testGetChannels() {
    int channels = this->img.getChannels();
    if (channels != 4) {
        throw std::runtime_error("Channels is expected to be 4, but found " + std::to_string(channels));
    }
}

void ImageTests::testSetData() {
    Image new_img(filepath);
    if (new_img.getData() == nullptr) {
        throw std::runtime_error("Image data should not be null");
    }

    // Create new dummy data
    int dataSize = new_img.getWidth() * new_img.getHeight() * new_img.getChannels();
    std::vector<unsigned char> dummyData(dataSize, 255); // Fill with white pixels

    // Set new data
    new_img.setData(dummyData.data());

    // Verify that the data is updated
    const unsigned char* updatedData = new_img.getData();

    if (updatedData == nullptr) {
        throw std::runtime_error("Updated image data should not be null");
    }
    if (updatedData[0] != 255) {
        throw std::runtime_error("First pixel should be 255 (white)");
    }
}

void ImageTests::testSetChannels() {
    Image new_img(filepath);

    // Set a valid number of channels
    new_img.setChannels(3);
    if (new_img.getChannels() != 3) {
        throw std::runtime_error("setChannels failed to update correctly");
    }

    try {
        new_img.setChannels(-1);
        throw std::runtime_error("setChannels should have thrown an error for negative channels");
    } catch (const std::invalid_argument&) {
        // test passes
    }
}