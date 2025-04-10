#ifndef FILTERS2DTESTS_H 
#define FILTERS2DTESTS_H

#include "../src/Image.h"
#include <iostream>
#include <cassert>

class Filters2DTests {
public:
    Filters2DTests();
    void testApplyGreyscale();
    void testApplyBrightness();
    void testApplyHistogramEqualization();
    void testThreshold();
    void testApplySaltandPepperNoise();
    
    void testBoxBlur();
    void testGaussianBlur();
    void testMedianBlur();
    
    void testSharpen();
    void testEdgeDetection();
private:
    const char* filepath;
    Image img;
};

#endif // FILTERS2DTESTS_H