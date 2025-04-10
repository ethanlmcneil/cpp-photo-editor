#ifndef PROJECTIONS3DTESTS_H
#define PROJECTIONS3DTESTS_H

#include <string>
#include "Volume.h"

/**
 * A set of unit tests for the Projections3D class.
 * Each test creates a small synthetic Volume and calls the
 * respective projection methods (MIP, MinIP, AIP, and their slab versions).
 */
class Projections3DTests {
public:
    /**
     * Constructor: sets up a small synthetic volume for testing.
     */
    Projections3DTests();

    /**
     * Test the MIP method on the small volume.
     */
    void testMIP();

    /**
     * Test the MinIP method on the small volume.
     */
    void testMinIP();

    /**
     * Test the AIP method on the small volume.
     */
    void testAIP();

    /**
     * Test the MIPSlab method on the small volume (partial z-range).
     */
    void testMIPSlab();

    /**
     * Test the MinIPSlab method on the small volume (partial z-range).
     */
    void testMinIPSlab();

    /**
     * Test the AIPSlab method on the small volume (partial z-range).
     */
    void testAIPSlab();

    /**
     * Test the AIPMedian method on the small volume.
     */
    void testAIPMedian();

private:
    Volume vol;  ///< A small synthetic volume for testing.
    std::string outDir; ///< Directory or prefix for output test images.

    /**
     * A helper to check if a .png file is valid and has the given width/height.
     * Uses stbi_info to verify.
     * \param filename The path to the .png to check.
     * \param expectedW The expected width of the written image.
     * \param expectedH The expected height of the written image.
     */
    void verifyPNG(const std::string& filename, int expectedW, int expectedH);
};

#endif // PROJECTIONS3DTESTS_H
