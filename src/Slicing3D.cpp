/**
 * @file Slicing3D.cpp
 * @brief Implementation of the Slicing3D class for extracting 2D slices from 3D volumes
 * Group Members:
 * - Jiaqi    (GitHub: esemsc-jc1424)
 * - Daicong  (GitHub: esemsc-c730ef50)
 * - Ida      (GitHub: esemsc-ifc24)
 * - Zhuyi    (GitHub: esemsc-zf1124)
 * - Dany     (GitHub: esemsc-dh324)
 * - Ethan    (GitHub: edsml-elm224)
 * - Keyun    (GitHub: esemsc-km824)
 */

#include "Slicing3D.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "stb_image_write.h"

/**
 * @brief Extracts a 2D slice from a 3D volume along the specified plane.
 * 
 * @param vol The input 3D volume.
 * @param plane The slicing plane ("XY", "XZ", or "YZ").
 * @param coordinate The slice index along the chosen plane.
 * @param outputPath The output file path for the extracted slice.
 */
void Slicing3D::slice3D(const Volume& vol, const std::string& plane, int coordinate, const std::string& outputPath) {
    // Check if the volume has valid dimensions
    if (vol.width <= 0 || vol.height <= 0 || vol.depth <= 0) {
        std::cerr << "Error: Invalid volume dimensions for slicing\n";
        return;
    }

    // Call the appropriate slicing method based on the specified plane
    try {
        std::string upperPlane = plane;
        // Convert plane to uppercase for case-insensitive comparison
        std::transform(upperPlane.begin(), upperPlane.end(), upperPlane.begin(), ::toupper);

        if (upperPlane == "XY") {
            // Check if the z-coordinate is valid
            if (coordinate < 0 || coordinate >= vol.depth) {
                std::cerr << "Error: Z-coordinate " << coordinate << " out of range (0-" << (vol.depth - 1) << ")\n";
                return;
            }
            sliceXY(vol, coordinate, outputPath);
        }
        else if (upperPlane == "XZ") {
            // Check if the y-coordinate is valid
            if (coordinate < 0 || coordinate >= vol.height) {
                std::cerr << "Error: Y-coordinate " << coordinate << " out of range (0-" << (vol.height - 1) << ")\n";
                return;
            }
            sliceXZ(vol, coordinate, outputPath);
        }
        else if (upperPlane == "YZ") {
            // Check if the x-coordinate is valid
            if (coordinate < 0 || coordinate >= vol.width) {
                std::cerr << "Error: X-coordinate " << coordinate << " out of range (0-" << (vol.width - 1) << ")\n";
                return;
            }
            sliceYZ(vol, coordinate, outputPath);
        }
        else {
            std::cerr << "Error: Unknown plane type " << plane << ". Expected XY, XZ, or YZ\n";
            return;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error during slice extraction: " << e.what() << "\n";
        return;
    }
}

/**
 * @brief Extracts a slice from the XY plane at a given Z-coordinate.
 * 
 * @param vol The input 3D volume.
 * @param z The Z-coordinate of the slice.
 * @param outputPath The output file path for the extracted slice.
 */
void Slicing3D::sliceXY(const Volume& vol, int z, const std::string& outputPath) {
    // Create a new 2D image with the same width and height as the volume
    int outWidth = vol.width;
    int outHeight = vol.height;

    // Create a buffer for the 2D slice
    std::vector<unsigned char> sliceData(outWidth * outHeight, 0);

    // Copy the slice data from the volume to the image
    for (int y = 0; y < outHeight; ++y) {
        for (int x = 0; x < outWidth; ++x) {
            // Get the voxel value at (x, y, z)
            unsigned char voxel = vol.getVoxel(x, y, z);

            // Set the corresponding pixel in the output image
            sliceData[y * outWidth + x] = voxel;
        }
    }

    // Write the slice to a PNG file
    int success = stbi_write_png(outputPath.c_str(), outWidth, outHeight, 1,
        sliceData.data(), outWidth);

    if (!success) {
        std::cerr << "Failed to write XY slice to " << outputPath << "\n";
    }
    else {
        std::cout << "[Slicing3D] XY slice at Z=" << z << " saved to " << outputPath << "\n";
    }
}

/**
 * @brief Extracts a slice from the XZ plane at a given Y-coordinate.
 * 
 * @param vol The input 3D volume.
 * @param y The Y-coordinate of the slice.
 * @param outputPath The output file path for the extracted slice.
 */
void Slicing3D::sliceXZ(const Volume& vol, int y, const std::string& outputPath) {
    // Create a new 2D image with width = volume width and height = volume depth
    int outWidth = vol.width;
    int outHeight = vol.depth;

    // Create a buffer for the 2D slice
    std::vector<unsigned char> sliceData(outWidth * outHeight, 0);

    // Copy the slice data from the volume to the image
    for (int z = 0; z < outHeight; ++z) {
        for (int x = 0; x < outWidth; ++x) {
            // Get the voxel value at (x, y, z)
            unsigned char voxel = vol.getVoxel(x, y, z);

            // Set the corresponding pixel in the output image
            // Note: z becomes the y-coordinate in the resulting image
            sliceData[z * outWidth + x] = voxel;
        }
    }

    // Write the slice to a PNG file
    int success = stbi_write_png(outputPath.c_str(), outWidth, outHeight, 1,
        sliceData.data(), outWidth);

    if (!success) {
        std::cerr << "Failed to write XZ slice to " << outputPath << "\n";
    }
    else {
        std::cout << "[Slicing3D] XZ slice at Y=" << y << " saved to " << outputPath << "\n";
    }
}

/**
 * @brief Extracts a slice from the YZ plane at a given X-coordinate.
 * 
 * @param vol The input 3D volume.
 * @param x The X-coordinate of the slice.
 * @param outputPath The output file path for the extracted slice.
 */
void Slicing3D::sliceYZ(const Volume& vol, int x, const std::string& outputPath) {
    // Create a new 2D image with width = volume height and height = volume depth
    int outWidth = vol.height;
    int outHeight = vol.depth;

    // Create a buffer for the 2D slice
    std::vector<unsigned char> sliceData(outWidth * outHeight, 0);

    // Copy the slice data from the volume to the image
    for (int z = 0; z < outHeight; ++z) {
        for (int y = 0; y < outWidth; ++y) {
            // Get the voxel value at (x, y, z)
            unsigned char voxel = vol.getVoxel(x, y, z);

            // Set the corresponding pixel in the output image
            // Note: y becomes the x-coordinate and z becomes the y-coordinate in the resulting image
            sliceData[z * outWidth + y] = voxel;
        }
    }

    // Write the slice to a PNG file
    int success = stbi_write_png(outputPath.c_str(), outWidth, outHeight, 1,
        sliceData.data(), outWidth);

    if (!success) {
        std::cerr << "Failed to write YZ slice to " << outputPath << "\n";
    }
    else {
        std::cout << "[Slicing3D] YZ slice at X=" << x << " saved to " << outputPath << "\n";
    }
}