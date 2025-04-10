#include "Slicing3DTests.h"
#include "Slicing3D.h"
#include "stb_image.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <cstdio>
#include <vector>
#include <algorithm>

/**
 * @brief A helper RAII class to simultaneously capture std::cout and std::cerr.
 * When constructed, it redirects both std::cout and std::cerr to an internal buffer.
 * When destructed, it restores them to the original buffers.
 */
class CoutCerrRedirect {
private:
    // We will store the old streambuf pointers so we can restore them later
    std::streambuf* oldCoutBuf;
    std::streambuf* oldCerrBuf;

    // A single buffer receiving everything from both streams
    std::ostringstream mergedStream;

public:
    CoutCerrRedirect()
    {
        // Redirect both
        oldCoutBuf = std::cout.rdbuf(mergedStream.rdbuf());
        oldCerrBuf = std::cerr.rdbuf(mergedStream.rdbuf());
    }

    ~CoutCerrRedirect()
    {
        // Restore
        std::cout.rdbuf(oldCoutBuf);
        std::cerr.rdbuf(oldCerrBuf);
    }

    // Retrieve everything that was printed to either std::cout or std::cerr
    std::string getMergedString() const {
        return mergedStream.str();
    }
};

/**
 * Constructor sets up a small synthetic volume. E.g., width=4, height=3, depth=2.
 */
Slicing3DTests::Slicing3DTests()
    : outDir("./")
{
    vol.width    = 4;
    vol.height   = 3;
    vol.depth    = 2;
    vol.channels = 1; // grayscale
    vol.data.resize(vol.width * vol.height * vol.depth, 50); // Initialize with 50

    // Create patterns to help identify orientation
    for (int z = 0; z < vol.depth; z++) {
        for (int y = 0; y < vol.height; y++) {
            for (int x = 0; x < vol.width; x++) {
                size_t idx = x + (size_t)vol.width * (y + (size_t)vol.height * z);
                // Default based on z
                vol.data[idx] = (z == 0) ? 50 : 200;
                // Override for x=1 or y=1
                if (x == 1) vol.data[idx] = 100;
                if (y == 1) vol.data[idx] = 150;
            }
        }
    }
}

void Slicing3DTests::verifyPNG(const std::string& filename, int expectedW, int expectedH) {
    int wTest, hTest, cTest;
    if (!stbi_info(filename.c_str(), &wTest, &hTest, &cTest)) {
        std::cerr << "verifyPNG: stbi_info failed for " << filename << std::endl;
        assert(false && "PNG file not found or invalid");
    }

    // We expect a 1-channel (grayscale) image
    assert(wTest == expectedW && "PNG width mismatch");
    assert(hTest == expectedH && "PNG height mismatch");
    assert(cTest == 1 && "PNG channels should be 1 (grayscale)");

    // If we get here, the test passed for file validity
    std::cout << "Verified " << filename << ": "
              << wTest << "x" << hTest << " with " << cTest << " channels" << std::endl;
}

// Test XY plane slice
void Slicing3DTests::testSliceXY() {
    // Capture both stdout and stderr from Slicing3D
    CoutCerrRedirect capture;

    std::string outFile = outDir + "testSliceXY.png";
    std::remove(outFile.c_str());

    // Perform the slice
    Slicing3D::slice3D(vol, "XY", 1, outFile);

    // Verify output
    verifyPNG(outFile, vol.width, vol.height);

    // Print final "[PASS]"
    std::cout << "[PASS] SLICING3D - XY Plane";

    // Retrieve all logs from both std::cout and std::cerr
    std::string logs = capture.getMergedString();

    // Print them, indented, line by line
    if (!logs.empty()) {
        std::istringstream iss(logs);
        std::string line;
        while (std::getline(iss, line)) {
            std::cout << "\n       " << line;
        }
    }
    std::cout << std::endl;
}

// Test XZ plane slice
void Slicing3DTests::testSliceXZ() {
    CoutCerrRedirect capture;

    std::string outFile = outDir + "testSliceXZ.png";
    std::remove(outFile.c_str());

    Slicing3D::slice3D(vol, "XZ", 1, outFile);
    verifyPNG(outFile, vol.width, vol.depth);

    std::cout << "[PASS] SLICING3D - XZ Plane";

    std::string logs = capture.getMergedString();
    if (!logs.empty()) {
        std::istringstream iss(logs);
        std::string line;
        while (std::getline(iss, line)) {
            std::cout << "\n       " << line;
        }
    }
    std::cout << std::endl;
}

// Test YZ plane slice
void Slicing3DTests::testSliceYZ() {
    CoutCerrRedirect capture;

    std::string outFile = outDir + "testSliceYZ.png";
    std::remove(outFile.c_str());

    Slicing3D::slice3D(vol, "YZ", 1, outFile);
    verifyPNG(outFile, vol.height, vol.depth);

    std::cout << "[PASS] SLICING3D - YZ Plane";

    std::string logs = capture.getMergedString();
    if (!logs.empty()) {
        std::istringstream iss(logs);
        std::string line;
        while (std::getline(iss, line)) {
            std::cout << "\n       " << line;
        }
    }
    std::cout << std::endl;
}

// Test invalid plane parameter
void Slicing3DTests::testInvalidPlane() {
    CoutCerrRedirect capture;

    std::string outFile = outDir + "testInvalidPlane.png";
    std::remove(outFile.c_str());

    // Expect an error from Slicing3D
    Slicing3D::slice3D(vol, "AB", 1, outFile);

    // Confirm no file was created
    int wTest, hTest, cTest;
    bool fileExists = stbi_info(outFile.c_str(), &wTest, &hTest, &cTest);
    assert(!fileExists && "No file should be created for invalid plane");

    std::cout << "[PASS] SLICING3D - Invalid Plane";

    // Insert "Error expected:" if we see "Error:"
    std::string logs = capture.getMergedString();
    std::istringstream iss(logs);
    std::string line;
    bool printedErrorExpected = false;
    while (std::getline(iss, line)) {
        // If we see "Error:" for the first time, insert "Error expected:"
        if (!printedErrorExpected && line.find("Error:") != std::string::npos) {
            std::cout << "\n       Error expected:";
            printedErrorExpected = true;
        }
        std::cout << "\n       " << line;
    }
    std::cout << std::endl;
}

// Test out-of-range coordinate
void Slicing3DTests::testOutOfRangeCoordinate() {
    CoutCerrRedirect capture;

    std::string outFile = outDir + "testOutOfRange.png";
    std::remove(outFile.c_str());

    // Expect an error
    Slicing3D::slice3D(vol, "XY", 10, outFile);

    // Confirm no file created
    int wTest, hTest, cTest;
    bool fileExists = stbi_info(outFile.c_str(), &wTest, &hTest, &cTest);
    assert(!fileExists && "No file should be created for out-of-range coordinate");

    std::cout << "[PASS] SLICING3D - Out of Range Coordinate";

    // Insert "Error expected:" if we see "Error:"
    std::string logs = capture.getMergedString();
    std::istringstream iss(logs);
    std::string line;
    bool printedErrorExpected = false;
    while (std::getline(iss, line)) {
        if (!printedErrorExpected && line.find("Error:") != std::string::npos) {
            std::cout << "\n       Error expected:";
            printedErrorExpected = true;
        }
        std::cout << "\n       " << line;
    }
    std::cout << std::endl;
}
