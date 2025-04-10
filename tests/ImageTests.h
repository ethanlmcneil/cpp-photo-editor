#ifndef IMAGE_TESTS_H
#define IMAGE_TESTS_H

#include "../src/Image.h"
#include <iostream>
#include <cassert>

class ImageTests {
public:
    ImageTests();
    void testLoadUsingFilePath();
    void testGetWidth();
    void testGetHeight();
    void testGetChannels();
    void testSetData();
    void testSetChannels();

private:
    const char* filepath;
    Image img;
};

#endif // IMAGE_TESTS_H