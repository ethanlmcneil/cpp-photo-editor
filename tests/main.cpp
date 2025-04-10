#include "Filters2DTests.h"
#include "ImageTests.h"
#include "Projections3DTests.h"
#include "TestRunner.h"
#include <iostream>
#include "Filters3DTests.h"
#include "Slicing3DTests.h"
#include "stb_image.h"

int main() {
    std::cout << "Running all tests...\n";

    // Image Tests
    std::cout << "\n========== Image Tests ==========" << std::endl;
    ImageTests image_tests;
    TestRunner::runTest("IMAGE - Load Image", [&]() { image_tests.testLoadUsingFilePath(); });
    TestRunner::runTest("IMAGE - Get Width", [&]() { image_tests.testGetWidth(); });
    TestRunner::runTest("IMAGE - Get Height", [&]() { image_tests.testGetHeight(); });
    TestRunner::runTest("IMAGE - Get Channels", [&]() { image_tests.testGetChannels(); });
    TestRunner::runTest("IMAGE - Set Data", [&]() { image_tests.testSetData(); });
    TestRunner::runTest("IMAGE - Set Channels", [&]() { image_tests.testSetChannels(); });

    // Filters2D Tests
    std::cout << "\n========== Filters2D Tests ==========" << std::endl;
    Filters2DTests filters2d_tests;
    TestRunner::runTest("FILTERS2D - Apply Greyscale", [&]() { filters2d_tests.testApplyGreyscale(); });
    TestRunner::runTest("FILTERS2D - Apply Brightness", [&]() { filters2d_tests.testApplyBrightness(); });
    TestRunner::runTest("FILTERS2D - Apply Histogram Equalisation", [&]() { filters2d_tests.testApplyHistogramEqualization(); });
    TestRunner::runTest("FILTERS2D - Apply Threshold", [&]() { filters2d_tests.testThreshold(); });
    TestRunner::runTest("FILTERS2D - Apply Salt and Pepper Noise", [&]() { filters2d_tests.testApplySaltandPepperNoise(); });
    TestRunner::runTest("FILTERS2D - Apply Box Blur", [&]() { filters2d_tests.testBoxBlur(); });
    TestRunner::runTest("FILTERS2D - Apply Gaussian Blur", [&]() { filters2d_tests.testGaussianBlur(); });
    TestRunner::runTest("FILTERS2D - Apply Median Blur", [&]() { filters2d_tests.testMedianBlur(); });
    TestRunner::runTest("FILTERS2D - Apply Sharpen", [&]() { filters2d_tests.testSharpen(); });
    TestRunner::runTest("FILTERS2D - Apply Edge Detection", [&]() { filters2d_tests.testEdgeDetection(); });

    // Projections3D Tests
    std::cout << "\n========== Projections3D Tests ==========" << std::endl;
    Projections3DTests projTests;
    TestRunner::runTest("PROJECTIONS - MIP", [&]() { projTests.testMIP(); });
    TestRunner::runTest("PROJECTIONS - MinIP", [&]() { projTests.testMinIP(); });
    TestRunner::runTest("PROJECTIONS - AIP", [&]() { projTests.testAIP(); });
    TestRunner::runTest("PROJECTIONS - MIP Slab", [&]() { projTests.testMIPSlab(); });
    TestRunner::runTest("PROJECTIONS - MinIP Slab", [&]() { projTests.testMinIPSlab(); });
    TestRunner::runTest("PROJECTIONS - AIP Slab", [&]() { projTests.testAIPSlab(); });
    TestRunner::runTest("PROJECTIONS - AIPMedian", [&]() { projTests.testAIPMedian(); });

    // Filters3D Tests
    std::cout << "\n========== Filters3D Tests ==========" << std::endl;
    Filters3DTests filters3d_tests;
    TestRunner::runTest("FILTERS3D - Gaussian Blur - Size Preserved", [&]() { filters3d_tests.testGaussianBlurSizePreserved(); });
    TestRunner::runTest("FILTERS3D - Median Blur - Size Preserved", [&]() { filters3d_tests.testMedianBlurSizePreserved(); });
    TestRunner::runTest("FILTERS3D - Apply 3D Gaussian Blur", [&]() { filters3d_tests.testApplyBlur3DGaussian(); });
    TestRunner::runTest("FILTERS3D - Apply 3D Median Blur", [&]() { filters3d_tests.testApplyBlur3DMedian(); });

    // Slicing3D Tests
    std::cout << "\n========== Slicing3D Tests ==========" << std::endl;
    Slicing3DTests slicing_tests;
    TestRunner::runTest("SLICING3D - XY Plane", [&]() { slicing_tests.testSliceXY(); });
    TestRunner::runTest("SLICING3D - XZ Plane", [&]() { slicing_tests.testSliceXZ(); });
    TestRunner::runTest("SLICING3D - YZ Plane", [&]() { slicing_tests.testSliceYZ(); });
    TestRunner::runTest("SLICING3D - Expected Error - Invalid Plane", [&]() { slicing_tests.testInvalidPlane(); });
    TestRunner::runTest("SLICING3D - Expected Error - Out of Range Coordinate", [&]() { slicing_tests.testOutOfRangeCoordinate(); });

    std::cout << "\n========== All Tests Completed ==========" << std::endl;

    return TestRunner::getFailureCount() > 0 ? 1 : 0;
}
