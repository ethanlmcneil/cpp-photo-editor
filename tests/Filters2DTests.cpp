#include "Filters2DTests.h"
#include "../src/stb_image.h"
#include "../src/stb_image_write.h"
#include "../src/Image.h"
#include "../src/Filters2D.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstring> 


Filters2DTests::Filters2DTests() : filepath("../Images/small.png"), img(filepath) {}

void Filters2DTests::testApplyGreyscale() {
        Image test_img(filepath);
        Filters2D filter;

        if (test_img.getData() == nullptr) {
            throw std::runtime_error("Image should be loaded before testing Greyscale.");
        }
    
        int original_width = test_img.getWidth();
        int original_height = test_img.getHeight();
        int original_channels = test_img.getChannels();
    
        // Apply Greyscale
        filter.apply_Greyscale(test_img);
        
        if (test_img.getData() == nullptr) {
            throw std::runtime_error("Greyscale image should be generated.");
        }
        if (test_img.getWidth() != original_width) {
            throw std::runtime_error("Width should remain the same.");
        }
        if (test_img.getHeight() != original_height) {
            throw std::runtime_error("Height should remain the same.");
        }
        if (test_img.getChannels() != 1) {
            throw std::runtime_error("Greyscale image should have only 1 channel.");
        }

        // Construct a 2x2 RGB image
        unsigned char imageData[12] = {
            100, 150, 200, 
             50, 100, 150, 
            200, 250, 100,  
              0,  50, 100   
        };

        Image img(imageData, 2, 2, 3);
        filter.apply_Greyscale(img);
        
        // Grayscale calculation formula: 0.2126*R + 0.7152*G + 0.0722*B
        const unsigned char* result = img.getData();
        int expected0 = static_cast<int>(0.2126 * 100 + 0.7152 * 150 + 0.0722 * 200);
        int expected1 = static_cast<int>(0.2126 * 50  + 0.7152 * 100 + 0.0722 * 150);
        int expected2 = static_cast<int>(0.2126 * 200 + 0.7152 * 250 + 0.0722 * 100);
        int expected3 = static_cast<int>(0.2126 * 0   + 0.7152 * 50  + 0.0722 * 100);
        
        if (result[0] != expected0) {
            throw std::runtime_error("Pixel 0 value mismatch in Greyscale conversion.");
        }
        if (result[1] != expected1) {
            throw std::runtime_error("Pixel 1 value mismatch in Greyscale conversion.");
        }
        if (result[2] != expected2) {
            throw std::runtime_error("Pixel 2 value mismatch in Greyscale conversion.");
        }
        if (result[3] != expected3) {
            throw std::runtime_error("Pixel 3 value mismatch in Greyscale conversion.");
        }
        // Additionally, the number of channels should become 1
        if (img.getChannels() != 1) {
            throw std::runtime_error("Number of channels should be 1 after Greyscale conversion.");
        }
    }

void Filters2DTests::testApplyBrightness() {
        Image test_img(filepath);
        Filters2D filter;

        if (test_img.getData() == nullptr) {
            throw std::runtime_error("Image should be loaded before testing Brightness.");
        }
    
        int original_width = test_img.getWidth();
        int original_height = test_img.getHeight();
        int original_channels = test_img.getChannels();
    
        // Apply Brightness
        filter.apply_Brightness(test_img, 50);
    
        if (test_img.getData() == nullptr) {
            throw std::runtime_error("Brightness image should be generated.");
        }
        if (test_img.getWidth() != original_width) {
            throw std::runtime_error("Width should remain the same.");
        }
        if (test_img.getHeight() != original_height) {
            throw std::runtime_error("Height should remain the same.");
        }
        if (test_img.getChannels() != original_channels) {
            throw std::runtime_error("Channels should remain the same.");
        }

        // Construct a 2x2 grayscale image (1 channel)
        unsigned char imageData[4] = { 100, 150, 200, 250 };
        Image img(imageData, 2, 2, 1);
        int brightnessValue = 20;  // Increase brightness by 20
        filter.apply_Brightness(img, brightnessValue);
        
        const unsigned char* result = img.getData();
        // Note: Overflow after addition should be clamped to 255
        if (result[0] != 120) {
            throw std::runtime_error("Pixel 0 value mismatch after Brightness adjustment.");
        }
        if (result[1] != 170) {
            throw std::runtime_error("Pixel 1 value mismatch after Brightness adjustment.");
        }
        if (result[2] != 220) {
            throw std::runtime_error("Pixel 2 value mismatch after Brightness adjustment.");
        }
        if (result[3] != 255) { // 250 + 20 = 270, but the maximum is 255
            throw std::runtime_error("Pixel 3 value mismatch after Brightness adjustment.");
        }
    }


void Filters2DTests::testApplyHistogramEqualization() {
        Image test_img(filepath);
        Filters2D filter;

        if (test_img.getData() == nullptr) {
                throw std::runtime_error("Image should be loaded before testing Histogram Equalization.");
        }

        int original_width = test_img.getWidth();
        int original_height = test_img.getHeight();
        int original_channels = test_img.getChannels();
        const unsigned char* original_data = test_img.getData();

        // Compute original histogram
        std::vector<int> hist(256, 0);
        for (int i = 0; i < original_width * original_height * original_channels; i += original_channels) {
                hist[original_data[i]]++;
        }

        // Apply Histogram Equalization
        filter.apply_Histogram_Equalisation(test_img,"HSV");

        // Load modified image
        if (test_img.getData() == nullptr) {
                throw std::runtime_error("Equalized image should be generated.");
        }
        if (test_img.getWidth() != original_width) {
                throw std::runtime_error("Width should remain the same.");
        }
        if (test_img.getHeight() != original_height) {
                throw std::runtime_error("Height should remain the same.");
        }
        if (test_img.getChannels() != original_channels) {
                throw std::runtime_error("Channels should remain the same.");
        }

        // Compute new histogram
        std::vector<int> new_hist(256, 0);
        const unsigned char* new_data = test_img.getData();
        for (int i = 0; i < original_width * original_height * original_channels; i += original_channels) {
                new_hist[new_data[i]]++;
        }

        // (1) Check that histogram is more evenly distributed (no large gaps)
        int original_non_zero_bins = std::count_if(hist.begin(), hist.end(), [](int h) { return h > 0; });
        int new_non_zero_bins = std::count_if(new_hist.begin(), new_hist.end(), [](int h) { return h > 0; });
        if (new_non_zero_bins >= original_non_zero_bins * 0.9) {
                throw std::runtime_error("Histogram should be more evenly distributed.");
        }

        // (2) Ensure all pixel values are within range [0, 255]
        for (int i = 0; i < original_width * original_height * original_channels; i += original_channels) {
                if (new_data[i] < 0 || new_data[i] > 255) {
                        throw std::runtime_error("Pixel values should be in range 0-255.");
                }
        }

        // (3) Ensure image data is not all zero (indicating a valid output)
        int non_zero_pixels = std::count_if(new_data, new_data + (original_width * original_height * original_channels),
                                                                                [](unsigned char p) { return p > 0; });
        if (non_zero_pixels <= 0) {
                throw std::runtime_error("Equalized image should not be completely black.");
        }

        // (4) Check if mean brightness is adjusted (expected to move towards the center)
        float original_mean = std::accumulate(hist.begin(), hist.end(), 0.0f) / (original_width * original_height);
        float new_mean = std::accumulate(new_hist.begin(), new_hist.end(), 0.0f) / (original_width * original_height);

        if (std::abs(new_mean - 128) > std::abs(original_mean - 128)) {
                throw std::runtime_error("Mean brightness should be closer to 128.");
        }
}

void Filters2DTests::testThreshold(){
        Filters2D filter;
        // Construct a 2x2 grayscale image
        unsigned char imageData[4] = { 50, 120, 130, 200 };
        Image img(imageData, 2, 2, 1);
        
        int threshold = 128; // Values less than 128 become 0; values greater than or equal to 128 become 255
        // For grayscale images, the space parameter is not used and can be passed as an empty string
        filter.Threshold(img, threshold, "HSV");
        
        const unsigned char* result = img.getData();
        // Expected: 50,120 -> 0; 130,200 -> 255
        if (result[0] != 0) {
            throw std::runtime_error("Pixel 0 value mismatch after Threshold adjustment.");
        }
        if (result[1] != 0) {
            throw std::runtime_error("Pixel 1 value mismatch after Threshold adjustment.");
        }
        if (result[2] != 255) {
            throw std::runtime_error("Pixel 2 value mismatch after Threshold adjustment.");
        }
        if (result[3] != 255) {
            throw std::runtime_error("Pixel 3 value mismatch after Threshold adjustment.");
        }
}

void Filters2DTests::testApplySaltandPepperNoise(){
        Filters2D filter;
        // Construct a 3x3 grayscale image, all pixels initially set to 100
        unsigned char imageData[9] = { 
            100, 100, 100,
            100, 100, 100,
            100, 100, 100 
        };
        Image img(imageData, 3, 3, 1);
        
        // Set noise probability to 100 to ensure all pixels are affected
        float noise_prob = 100;
        filter.apply_Salt_and_Pepper_Noise(img, noise_prob);
        
        const unsigned char* result = img.getData();
        // Each pixel value should be either 0 or 255
        for (int i = 0; i < 9; ++i) {
            if (result[i] != 0 && result[i] != 255) {
                throw std::runtime_error("Pixel value mismatch after applying Salt and Pepper Noise.");
            }
        }
}


void Filters2DTests::testBoxBlur() {
        Filters2D filter;
        // 1. Test if 1x1 and 2x2 small images can run without issues
        try {
                unsigned char data1x1[] = {255, 255, 255};
                Image img1x1(data1x1, 1, 1, 3);
                filter.boxBlur(img1x1, 3);

                unsigned char data2x2[] = {
                255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255
                };
                Image img2x2(data2x2, 2, 2, 3);
                filter.boxBlur(img2x2, 3);
        } catch (...) {
                throw std::runtime_error("boxBlur should not crash on small images.");
        }

        // 2. kernelSize = 1 should not change the image
        unsigned char originalData[] = {
                100, 150, 200, 50, 100, 150,
                200, 250, 100, 0, 0, 0
        };
        Image imgOriginal(originalData, 2, 2, 3);
        Image imgCopy(originalData, 2, 2, 3);
        filter.boxBlur(imgCopy, 1);
        if (memcmp(imgOriginal.getData(), imgCopy.getData(), 2 * 2 * 3) != 0) {
                throw std::runtime_error("boxBlur kernelSize = 1 should not change the image.");
        }

        // 3. Test grayscale images (channels = 1)
        unsigned char grayData[] = {
                100, 150, 200,
                150, 200, 100,
                50, 100, 150
        };
        Image grayImg(grayData, 3, 3, 1);
        filter.boxBlur(grayImg, 3);

        // 4. Test if kernelSize is even, it is automatically adjusted to odd
        int kernelSize = 4;
        filter.boxBlur(imgCopy, kernelSize); // Should not throw an error, and kernelSize should be adjusted to 5

        // Verify that image size and channels remain unchanged
        if (imgCopy.getWidth() != imgOriginal.getWidth()) {
                throw std::runtime_error("Width should remain unchanged after blur.");
        }
        if (imgCopy.getHeight() != imgOriginal.getHeight()) {
                throw std::runtime_error("Height should remain unchanged after blur.");
        }
        if (imgCopy.getChannels() != imgOriginal.getChannels()) {
                throw std::runtime_error("Channels should remain unchanged after blur.");
        }

        // 5. Center pixel is black, others are white, check if blur is applied
        unsigned char centerTest[] = {
                255, 255, 255,   255, 255, 255,   255, 255, 255,
                255, 255, 255,   0,   0,   0,     255, 255, 255,
                255, 255, 255,   255, 255, 255,   255, 255, 255
        };
        Image centerImg(centerTest, 3, 3, 3);
        filter.boxBlur(centerImg, 3);
        const unsigned char* blurred = centerImg.getData();
        if (!(blurred[4 * 3] > 0 && blurred[4 * 3] < 255)) {
                throw std::runtime_error("Center pixel after boxBlur should be neither black nor white.");
        }

        // 6. Checkerboard test for blur effectiveness (max-min difference decreases)
        unsigned char chessData[49 * 3];
        for (int i = 0; i < 49; ++i) {
                bool isWhite = (i / 7 + i % 7) % 2 == 0;
                unsigned char val = isWhite ? 255 : 0;
                chessData[i * 3 + 0] = val;
                chessData[i * 3 + 1] = val;
                chessData[i * 3 + 2] = val;
        }
        Image chessImg(chessData, 7, 7, 3);
        filter.boxBlur(chessImg, 3);
        unsigned char min_val = 255, max_val = 0;
        for (int i = 0; i < 49; ++i) {
                unsigned char r = chessImg.getData()[i * 3];
                min_val = std::min(min_val, r);
                max_val = std::max(max_val, r);
        }
        if (max_val - min_val >= 255) {
                throw std::runtime_error("boxBlur should reduce the max-min difference in the checkerboard test.");
        }
}


void Filters2DTests::testGaussianBlur() {
        Filters2D filter;
        // 1. Test if 1x1 and 2x2 small images can run without issues
        try {
            unsigned char data1x1[] = {255, 255, 255};
            Image img1x1(data1x1, 1, 1, 3);
            filter.gaussianBlur(img1x1, 3);
    
            unsigned char data2x2[] = {
                255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255
            };
            Image img2x2(data2x2, 2, 2, 3);
            filter.gaussianBlur(img2x2, 3);
        } catch (...) {
            throw std::runtime_error("GaussianBlur should not crash on small images.");
        }
    
        // 2. kernelSize = 1 should not change the image
        unsigned char originalData[] = {
            100, 150, 200, 50, 100, 150,
            200, 250, 100, 0, 0, 0
        };
        Image imgOriginal(originalData, 2, 2, 3);
        Image imgCopy(originalData, 2, 2, 3);
        filter.gaussianBlur(imgCopy, 1);
        if (memcmp(imgOriginal.getData(), imgCopy.getData(), 2 * 2 * 3) != 0) {
            throw std::runtime_error("GaussianBlur kernelSize = 1 should not change the image.");
        }
    
        // 3. Test grayscale images (channels = 1)
        unsigned char grayData[] = {
            100, 150, 200,
            150, 200, 100,
            50, 100, 150
        };
        Image grayImg(grayData, 3, 3, 1);
        filter.gaussianBlur(grayImg, 3);

        // 4. Test if kernelSize is even, it is automatically adjusted to odd
        int kernelSize = 4;
        filter.gaussianBlur(imgCopy, kernelSize); // Should not throw an error, and kernelSize should be adjusted to 5

        // Verify that image size and channels remain unchanged
        if (imgCopy.getWidth() != imgOriginal.getWidth()) {
            throw std::runtime_error("Width should remain unchanged after blur.");
        }
        if (imgCopy.getHeight() != imgOriginal.getHeight()) {
            throw std::runtime_error("Height should remain unchanged after blur.");
        }
        if (imgCopy.getChannels() != imgOriginal.getChannels()) {
            throw std::runtime_error("Channels should remain unchanged after blur.");
        }

    
        // 5. Center pixel is black, others are white, check if blur is applied
        unsigned char centerTest[] = {
            255, 255, 255,   255, 255, 255,   255, 255, 255,
            255, 255, 255,   0,   0,   0,     255, 255, 255,
            255, 255, 255,   255, 255, 255,   255, 255, 255
        };
        Image centerImg(centerTest, 3, 3, 3);
        filter.gaussianBlur(centerImg, 3);
        const unsigned char* blurred = centerImg.getData();
        if (!(blurred[4 * 3] > 0 && blurred[4 * 3] < 255)) {
            throw std::runtime_error("Center pixel after GaussianBlur should be neither black nor white.");
        }
    
        // 6. Checkerboard test for blur effectiveness (max-min difference decreases)
        unsigned char chessData[49 * 3];
        for (int i = 0; i < 49; ++i) {
            bool isWhite = (i / 7 + i % 7) % 2 == 0;
            unsigned char val = isWhite ? 255 : 0;
            chessData[i * 3 + 0] = val;
            chessData[i * 3 + 1] = val;
            chessData[i * 3 + 2] = val;
        }
        Image chessImg(chessData, 7, 7, 3);
        filter.gaussianBlur(chessImg, 3);
        unsigned char min_val = 255, max_val = 0;
        for (int i = 0; i < 49; ++i) {
            unsigned char r = chessImg.getData()[i * 3];
            min_val = std::min(min_val, r);
            max_val = std::max(max_val, r);
        }
        if (max_val - min_val >= 255) {
            throw std::runtime_error("GaussianBlur should reduce the max-min difference in the checkerboard test.");
        }
    }


void Filters2DTests::testMedianBlur() {
        Filters2D filter;
        // 1. Test if 1x1 and 2x2 small images can run without issues
        try {
            unsigned char data1x1[] = {255, 255, 255};
            Image img1x1(data1x1, 1, 1, 3);
            filter.medianBlur(img1x1, 3);
    
            unsigned char data2x2[] = {
                255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255
            };
            Image img2x2(data2x2, 2, 2, 3);
            filter.medianBlur(img2x2, 3);
        } catch (...) {
            throw std::runtime_error("MedianBlur should not crash on small images.");
        }
    
        // 2. kernelSize = 1 should not change the image
        unsigned char originalData[] = {
            100, 150, 200, 50, 100, 150,
            200, 250, 100, 0, 0, 0
        };
        Image imgOriginal(originalData, 2, 2, 3);
        Image imgCopy(originalData, 2, 2, 3);
        filter.medianBlur(imgCopy, 1);
        if (memcmp(imgOriginal.getData(), imgCopy.getData(), 2 * 2 * 3) != 0) {
            throw std::runtime_error("MedianBlur kernelSize = 1 should not change the image.");
        }
    
        // 3. Test grayscale images (channels = 1)
        unsigned char grayData[] = {
            100, 150, 200,
            150, 200, 100,
            50, 100, 150
        };
        Image grayImg(grayData, 3, 3, 1);
        filter.medianBlur(grayImg, 3);

        // 4. Test if kernelSize is even, it is automatically adjusted to odd
        int kernelSize = 4;
        filter.medianBlur(imgCopy, kernelSize); // Should not throw an error, and kernelSize should be adjusted to 5

        // Verify that image size and channels remain unchanged
        if (imgCopy.getWidth() != imgOriginal.getWidth()) {
            throw std::runtime_error("Width should remain unchanged after blur.");
        }
        if (imgCopy.getHeight() != imgOriginal.getHeight()) {
            throw std::runtime_error("Height should remain unchanged after blur.");
        }
        if (imgCopy.getChannels() != imgOriginal.getChannels()) {
            throw std::runtime_error("Channels should remain unchanged after blur.");
        }

    
        // 5. Sequentially arranged pixels, check if median blur correctly blurs the middle value
        unsigned char sequentialTest[] = {
                1, 1, 1,   2, 2, 2,   3, 3, 3,  
                4, 4, 4,   5, 5, 5,   6, 6, 6,  
                7, 7, 7,   8, 8, 8,   9, 9, 9,  
        };
    
        Image sequentialImg(sequentialTest, 3, 3, 3);
        
        // Apply median blur with kernel size of 3
        filter.medianBlur(sequentialImg, 3);
        
        // Get blurred image data
        const unsigned char* blurred = sequentialImg.getData();
        
        // Verify the value of the center pixel (should be 5, as it is the median)
        if (blurred[4 * 3] != 5) {
            throw std::runtime_error("Center pixel value mismatch after MedianBlur.");
        }
        
        // Verify other pixels (should remain consistent with the original image)
        if (blurred[0] != 2 || blurred[1] != 2 || blurred[2] != 2) {
            throw std::runtime_error("Top-left corner pixel value mismatch after MedianBlur.");
        }
        if (blurred[6] != 3 || blurred[7] != 3 || blurred[8] != 3) {
            throw std::runtime_error("Top-right corner pixel value mismatch after MedianBlur.");
        }
        if (blurred[18] != 7 || blurred[19] != 7 || blurred[20] != 7) {
            throw std::runtime_error("Bottom-left corner pixel value mismatch after MedianBlur.");
        }
        if (blurred[24] != 8 || blurred[25] != 8 || blurred[26] != 8) {
            throw std::runtime_error("Bottom-right corner pixel value mismatch after MedianBlur.");
        }

        // 6. Performance Test: Test on high-resolution image (1000x1000) with kernel size 5
        int width = 1000;
        int height = 1000;
        int kS = 5;

        // Generate test image data with random values
        std::vector<unsigned char> largeImageData(width * height * 3);
        for (int i = 0; i < width * height * 3; i++) {
                largeImageData[i] = rand() % 256;  // Random value between 0 and 255
        }
        
        // Create image from data
        Image largeImg(largeImageData.data(), width, height, 3);

        // Start measuring time
        auto start = std::chrono::high_resolution_clock::now();

        // Apply median blur
        filter.medianBlur(largeImg, kS);

        // End measuring time
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate elapsed time
        std::chrono::duration<double> elapsed = end - start;     
    }    

void Filters2DTests::testSharpen() {
    Filters2D filter;
    
    // 1. Test if small images can run without issues
    try {
        unsigned char data1x1[] = {100, 100, 100};
        Image img1x1(data1x1, 1, 1, 3);
        filter.Sharpen(img1x1);
    
        unsigned char data2x2[] = {
        100, 100, 100,   100, 100, 100,
        100, 100, 100,   100, 100, 100
        };
        Image img2x2(data2x2, 2, 2, 3);
        filter.Sharpen(img2x2);
    } catch (...) {
        throw std::runtime_error("Sharpen should not crash on small images.");
    }
    
    // 2. Uniform color image should not change significantly after sharpening
    unsigned char uniformData[] = {
        150, 150, 150,   150, 150, 150,   150, 150, 150,
        150, 150, 150,   150, 150, 150,   150, 150, 150,
        150, 150, 150,   150, 150, 150,   150, 150, 150
    };
    Image uniformImg(uniformData, 3, 3, 3);
    filter.Sharpen(uniformImg);
    const unsigned char* result = uniformImg.getData();
    for (int i = 0; i < 3 * 3 * 3; ++i) {
        if (std::abs(result[i] - 150) > 1) {
        throw std::runtime_error("Sharpen on uniform image should not change much.");
        }
    }
    
    // 3. Center black pixel test
    unsigned char centerBlack[] = {
        200, 200, 200,   200, 200, 200,   200, 200, 200,
        200, 200, 200,   0,   0,   0,     200, 200, 200,
        200, 200, 200,   200, 200, 200,   200, 200, 200
    };
    Image centerImg(centerBlack, 3, 3, 3);
    filter.Sharpen(centerImg);
    const unsigned char* sharpened = centerImg.getData();
    // The center pixel should still be close to 0
    if (sharpened[4 * 3] > 50) {
        throw std::runtime_error("Center black pixel should stay dark after sharpening.");
    }
    
    // 4. Checkerboard pattern test: sharpening should increase the max-min difference
    unsigned char chessData[49 * 3];
    for (int i = 0; i < 49; ++i) {
        bool isWhite = (i / 7 + i % 7) % 2 == 0;
        unsigned char val = isWhite ? 255 : 0;
        chessData[i * 3 + 0] = val;
        chessData[i * 3 + 1] = val;
        chessData[i * 3 + 2] = val;
    }
    Image chessImg(chessData, 7, 7, 3);
    filter.Sharpen(chessImg);
    unsigned char min_val = 255, max_val = 0;
    for (int i = 0; i < 49; ++i) {
        unsigned char r = chessImg.getData()[i * 3];
        min_val = std::min(min_val, r);
        max_val = std::max(max_val, r);
    }
    if (max_val - min_val < 255) {
        throw std::runtime_error("Sharpen should increase contrast in the checkerboard pattern.");
    }
    
    // 5. Test if grayscale images work correctly
    unsigned char grayData[] = {
        100, 150, 200,
        150, 200, 100,
        50, 100, 150
    };
    Image grayImg(grayData, 3, 3, 1);
    try {
        filter.Sharpen(grayImg);
    } catch (...) {
        throw std::runtime_error("Sharpen should support grayscale images (channels = 1).");
    }
}
     

void Filters2DTests::testEdgeDetection() {
    Filters2D filter;
    
    // 1. Test if small images can run without issues
    try {
        unsigned char data1x1[] = {100, 100, 100};
        Image img1x1(data1x1, 1, 1, 3);
        filter.DetectEdges(img1x1, EdgeDetectorType::Sobel);

        unsigned char data2x2[] = {
            100, 100, 100,   100, 100, 100,
            100, 100, 100,   100, 100, 100
        };
        Image img2x2(data2x2, 2, 2, 3);
        filter.DetectEdges(img2x2, EdgeDetectorType::Sobel);
    } catch (...) {
        throw std::runtime_error("DetectEdges should not crash on small images.");
    }

    // 2. Test on uniform color image: Edge detection should result in low values across the image
    unsigned char uniformData[] = {
        150, 150, 150,   150, 150, 150,   150, 150, 150,
        150, 150, 150,   150, 150, 150,   150, 150, 150,
        150, 150, 150,   150, 150, 150,   150, 150, 150
    };
    Image uniformImg(uniformData, 3, 3, 3);
    filter.DetectEdges(uniformImg, EdgeDetectorType::Sobel);
    const unsigned char* result = uniformImg.getData();
    for (int i = 0; i < 3 * 3; ++i) {
        if (result[i] > 50) {  // Edge value should be low for a uniform image
            throw std::runtime_error("Edge detection on uniform image should result in low edge values.");
        }
    }

    // 3. Test on image with a clear edge: A simple 2x2 image with a sharp boundary should show high edge values
    unsigned char edgeData[] = {
        0, 0, 0,   255, 255, 255,
        0, 0, 0,   255, 255, 255
    };
    Image edgeImg(edgeData, 2, 2, 3);
    filter.DetectEdges(edgeImg, EdgeDetectorType::Sobel);
    result = edgeImg.getData();
    bool edgeDetected = false;
    for (int i = 0; i < 4; ++i) {
        if (result[i] > 150) {  // There should be a noticeable edge strength
            edgeDetected = true;
            break;
        }
    }
    if (!edgeDetected) {
        throw std::runtime_error("Edge detection on simple edge image should show high edge values.");
    }

    // 4. Test Roberts Cross edge detection on 3x3 image with diagonal edge
    unsigned char diagonalData[] = {
        255, 0,   0,
        0, 255,  0,
        0, 0,   255
    };
    Image diagonalImg(diagonalData, 3, 3, 3);
    filter.DetectEdges(diagonalImg, EdgeDetectorType::RobertsCross);
    result = diagonalImg.getData();
    bool diagonalEdgeDetected = false;
    for (int i = 0; i < 9; ++i) {
        if (result[i] > 100) {  // A diagonal edge should have noticeable edge strength
            diagonalEdgeDetected = true;
            break;
        }
    }
    if (!diagonalEdgeDetected) {
        throw std::runtime_error("Edge detection on diagonal edge image with Roberts Cross should show noticeable edges.");
    }

    // 5. Test different edge detection types on a single image (for comparison)
    unsigned char testData[] = {
        100, 100, 100,   150, 150, 150,   200, 200, 200,
        50,  50,  50,    100, 100, 100,   150, 150, 150,
        0,   0,   0,     50,  50,  50,    100, 100, 100
    };
    Image testImg(testData, 3, 3, 3);
    
    // Test Sobel edge detector
    filter.DetectEdges(testImg, EdgeDetectorType::Sobel);
    const unsigned char* sobelResult = testImg.getData();
    for (int i = 0; i < 9; ++i) {
        if (sobelResult[i] < 0 || sobelResult[i] > 255) {
            throw std::runtime_error("Sobel edge detection should produce values in the range 0-255.");
        }
    }

    // Test Prewitt edge detector
    filter.DetectEdges(testImg, EdgeDetectorType::Prewitt);
    const unsigned char* prewittResult = testImg.getData();
    for (int i = 0; i < 9; ++i) {
        if (prewittResult[i] < 0 || prewittResult[i] > 255) {
            throw std::runtime_error("Prewitt edge detection should produce values in the range 0-255.");
        }
    }

    // Test Scharr edge detector
    filter.DetectEdges(testImg, EdgeDetectorType::Scharr);
    const unsigned char* scharrResult = testImg.getData();
    for (int i = 0; i < 9; ++i) {
        if (scharrResult[i] < 0 || scharrResult[i] > 255) {
            throw std::runtime_error("Scharr edge detection should produce values in the range 0-255.");
        }
    }

    // 6. Test if grayscale images work correctly (single channel)
    unsigned char grayData[] = {
        100, 150, 200,
        150, 200, 100,
        50, 100, 150
    };
    Image grayImg(grayData, 3, 3, 1);
    try {
        filter.DetectEdges(grayImg, EdgeDetectorType::Sobel);
    } catch (...) {
        throw std::runtime_error("Edge detection should support grayscale images (channels = 1).");
    }
}
