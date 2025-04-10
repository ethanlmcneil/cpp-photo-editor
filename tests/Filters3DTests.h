#ifndef FILTERS3D_TESTS_H
#define FILTERS3D_TESTS_H

#include "Filters3D.h"
#include "Volume.h"
#include <iostream>
#include <fstream>
#include <vector>

class Filters3DTests {
public:
    Filters3DTests();  // Constructor to initialize test Volume

    // Test functions
    void testGaussianBlurSizePreserved();
    void testMedianBlurSizePreserved();
    void testSaveSlicesPNG();
    void testApplyBlur3DGaussian();
    void testApplyBlur3DMedian();
    void testApplyBlur3DInvalidType();
    // Run all test cases
    void runTests();

private:
    Filters3D filters;
    Volume testVolume;

    // Assertion function
    void assertTest(bool condition, const std::string& testName);



    // Helper functions
    bool compareVolumes(const Volume& vol1, const Volume& vol2);
    bool fileExists(const std::string& filename);
};

#endif // FILTERS3D_TESTS_H
