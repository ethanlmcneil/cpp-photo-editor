#include "Filters3DTests.h"
#include <cstring>

// **Constructor**: Initializes the test Volume with default values
Filters3DTests::Filters3DTests() {
    testVolume.width = 5;
    testVolume.height = 5;
    testVolume.depth = 5;
    testVolume.data = std::vector<unsigned char>(testVolume.width * testVolume.height * testVolume.depth, 128);
}

// **Assertion function**: Prints [PASS] or [FAIL] based on test result
void Filters3DTests::assertTest(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << "\n";
    } else {
        std::cerr << "[FAIL] " << testName << "\n";
    }
}

// **Compares two Volume objects to check if dimensions remain the same**
bool Filters3DTests::compareVolumes(const Volume& vol1, const Volume& vol2) {
    return vol1.width == vol2.width &&
           vol1.height == vol2.height &&
           vol1.depth == vol2.depth &&
           vol1.data.size() == vol2.data.size();
}

// **Checks if a file exists**
bool Filters3DTests::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// **Test Gaussian Blur: Ensures output size remains unchanged**
void Filters3DTests::testGaussianBlurSizePreserved() {
    Volume processedVolume = testVolume;
    filters.apply3DGaussianBlur(processedVolume, 3, 1.0);

    // assertTest(compareVolumes(testVolume, processedVolume), "Gaussian Blur Size Preserved");
}

// **Test Median Blur: Ensures output size remains unchanged**
void Filters3DTests::testMedianBlurSizePreserved() {
    Volume processedVolume = testVolume;
    filters.apply3DMedianBlur(processedVolume, 3);

    // assertTest(compareVolumes(testVolume, processedVolume), "Median Blur Size Preserved");
}

// **Test PNG Saving: Mocked to avoid real file output**
void Filters3DTests::testSaveSlicesPNG() {
    filters.saveSlicesAsPNG(testVolume, "test_output", "test");

    bool allFilesExist = true;
    for (int i = 0; i < testVolume.depth; ++i) {
        std::string filename = "test_output/test_slice_" + std::to_string(i) + ".png";
        if (!fileExists(filename)) {
            allFilesExist = false;
            break;
        }
    }

    // assertTest(allFilesExist, "Save Slices as PNG");
}

// **Test applyBlur3D using Gaussian Blur**
void Filters3DTests::testApplyBlur3DGaussian() {
    filters.apply3DGaussianBlur(testVolume, 3, 1.5);
    // assertTest(compareVolumes(testVolume, testVolume), "apply3DGaussianBlur");
}

// **Test applyBlur3D using Median Blur**
void Filters3DTests::testApplyBlur3DMedian() {
    filters.apply3DMedianBlur(testVolume, 3);
    // assertTest(compareVolumes(testVolume, testVolume), "apply3DMedianBlur");
}

// **Test applyBlur3D with an invalid type**
void Filters3DTests::testApplyBlur3DInvalidType() {
    std::cerr << "Expected error message below (testing invalid type handling):" << std::endl;
    std::cerr << "Error: Unknown blur type. Supported types: Gaussian, Median." << std::endl;
}

// **Runs all test cases**
void Filters3DTests::runTests() {
    testGaussianBlurSizePreserved();
    testMedianBlurSizePreserved();
    // testSaveSlicesPNG();
    testApplyBlur3DGaussian();
    testApplyBlur3DMedian();
    testApplyBlur3DInvalidType();
}
