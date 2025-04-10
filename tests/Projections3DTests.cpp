#include "Projections3DTests.h"
#include "Projections3D.h"
#include "stb_image.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

/**
 * Constructor sets up a very small synthetic volume.
 * Example: 3 wide (x), 2 high (y), 2 deep (z).
 * So width=3, height=2, depth=2 => total 12 voxels per slice, 24 overall.
 * We'll store a simple pattern of intensities for projection testing.
 */
Projections3DTests::Projections3DTests()
    : outDir("./") // or "testsOutput/" etc., as you prefer
{
    vol.width = 3;
    vol.height = 2;
    vol.depth = 2;
    vol.channels = 1; // grayscale
    vol.data.resize(vol.width * vol.height * vol.depth);

    // Fill the volume data with some pattern
    // For z=0 slice, let's store 10..21. For z=1 slice, store 50..61, for instance.

    // z=0 slice
    // row-major: y in [0..1], x in [0..2]
    // data index = x + width*(y + height*z)
    // We'll do: [10,11,12,  13,14,15] => 6 pixels for (y=0..1, x=0..2)
    {
        vol.data[0] = 10;  // x=0,y=0,z=0
        vol.data[1] = 11;  // x=1,y=0,z=0
        vol.data[2] = 12;  // x=2,y=0,z=0
        vol.data[3] = 13;  // x=0,y=1,z=0
        vol.data[4] = 14;  // x=1,y=1,z=0
        vol.data[5] = 15;  // x=2,y=1,z=0
    }
    // z=1 slice
    {
        // index offset for z=1 => 1*(width*height) = 1* (3*2)= 6
        // We'll store [50,51,52,  53,54,55]
        vol.data[6]  = 50;
        vol.data[7]  = 51;
        vol.data[8]  = 52;
        vol.data[9]  = 53;
        vol.data[10] = 54;
        vol.data[11] = 55;
    }
}

void Projections3DTests::verifyPNG(const std::string& filename, int expectedW, int expectedH) {
    int wTest, hTest, cTest;
    if (!stbi_info(filename.c_str(), &wTest, &hTest, &cTest)) {
        std::cerr << "verifyPNG: stbi_info failed for " << filename << std::endl;
        assert(false && "PNG file not found or invalid");
    }
    // We expect a 3-channel (RGB) image because writeGrayPNG replicates grayscale into RGB
    assert(wTest == expectedW && "PNG width mismatch");
    assert(hTest == expectedH && "PNG height mismatch");
    assert(cTest == 3 && "PNG channels should be 3 (RGB) after writeGrayPNG");
}

// Test MIP on the entire volume
void Projections3DTests::testMIP() {
    std::string outFile = outDir + "testMIP.png";
    // We call MIP on the entire small volume
    Projections3D::MIP(vol, outFile);
    // The output should be 3 wide x 2 high, so check that
    verifyPNG(outFile, vol.width, vol.height);
}

// Test MinIP on the entire volume
void Projections3DTests::testMinIP() {
    std::string outFile = outDir + "testMinIP.png";
    Projections3D::MinIP(vol, outFile);
    verifyPNG(outFile, vol.width, vol.height);
}

// Test AIP on the entire volume
void Projections3DTests::testAIP() {
    std::string outFile = outDir + "testAIP.png";
    Projections3D::AIP(vol, outFile);
    verifyPNG(outFile, vol.width, vol.height);
}

// Test MIP with a partial slab [z=1..1], i.e. just the second slice
void Projections3DTests::testMIPSlab() {
    std::string outFile = outDir + "testMIPSlab.png";
    // We'll do zStart=1, zEnd=1 => only slice #1
    Projections3D::MIPSlab(vol, 1, 1, outFile);
    verifyPNG(outFile, vol.width, vol.height);
}

// Test MinIP with a partial slab [z=0..0], i.e. just the first slice
void Projections3DTests::testMinIPSlab() {
    std::string outFile = outDir + "testMinIPSlab.png";
    Projections3D::MinIPSlab(vol, 0, 0, outFile);
    verifyPNG(outFile, vol.width, vol.height);
}

// Test AIP with a partial slab [z=0..1] => entire volume, but we show the slab usage
void Projections3DTests::testAIPSlab() {
    std::string outFile = outDir + "testAIPSlab.png";
    Projections3D::AIPSlab(vol, 0, 1, outFile);
    verifyPNG(outFile, vol.width, vol.height);
}

// Test the median-based AIP over the entire volume
void Projections3DTests::testAIPMedian() {
    std::string outFile = outDir + "testAIPMedian.png";
    Projections3D::AIPMedian(vol, outFile);
    verifyPNG(outFile, vol.width, vol.height);
}
