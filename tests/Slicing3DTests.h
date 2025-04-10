#ifndef SLICING3D_TESTS_H
#define SLICING3D_TESTS_H

#include <string>
#include "Volume.h"

/**
 * @file Slicing3DTests.h
 * @brief A set of unit tests for the Slicing3D class
 * @author Your Name
 * 
 * Each test creates a small synthetic Volume and calls the
 * respective slice methods to extract 2D slices along different planes.
 */
class Slicing3DTests {
public:
    /**
     * Constructor: sets up a small synthetic volume for testing.
     */
    Slicing3DTests();

    /**
     * Test the sliceXY method on the small volume.
     * This extracts a 2D slice in the XY plane at a given Z coordinate.
     */
    void testSliceXY();

    /**
     * Test the sliceXZ method on the small volume.
     * This extracts a 2D slice in the XZ plane at a given Y coordinate.
     */
    void testSliceXZ();

    /**
     * Test the sliceYZ method on the small volume.
     * This extracts a 2D slice in the YZ plane at a given X coordinate.
     */
    void testSliceYZ();

    /**
     * Test slice3D function with invalid plane parameter.
     */
    void testInvalidPlane();

    /**
     * Test slice3D function with out-of-range coordinate.
     */
    void testOutOfRangeCoordinate();

private:
    Volume vol;           ///< A small synthetic volume for testing.
    std::string outDir;   ///< Directory for output test images.

    /**
     * A helper to check if a .png file is valid and has the given width/height.
     * Uses stbi_info to verify.
     * 
     * @param filename The path to the .png to check.
     * @param expectedW The expected width of the written image.
     * @param expectedH The expected height of the written image.
     */
    void verifyPNG(const std::string& filename, int expectedW, int expectedH);
};

#endif // SLICING3D_TESTS_H
