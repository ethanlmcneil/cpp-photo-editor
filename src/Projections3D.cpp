/*
 * @file Projections.cpp
 *
 * Group Members:
 * - Jiaqi    (GitHub: esemsc-jc1424)
 * - Daicong  (GitHub: esemsc-c730ef50)
 * - Ida      (GitHub: esemsc-ifc24)
 * - Zhuyi    (GitHub: esemsc-zf1124)
 * - Dany     (GitHub: esemsc-dh324)
 * - Ethan    (GitHub: edsml-elm224)
 * - Keyun    (GitHub: esemsc-km824)
 */

#include "Projections3D.h"
#include <iostream>
#include <algorithm>

#include "stb_image_write.h"

/**
 * @brief Writes an 8-bit grayscale image to a PNG file.
 * 
 * @param filename The name of the output PNG file.
 * @param buffer Pointer to the grayscale image data.
 * @param width Width of the image.
 * @param height Height of the image.
 * @return True if the image was successfully written, false otherwise.
 */
bool Projections3D::writeGrayPNG(const std::string &filename,
    const unsigned char *buffer,
    int width,
    int height)
{
    // Will replicate 'buffer' (which is grayscale) into R,G,B
    // Create an std::vector to hold 3 * width * height
    std::vector<unsigned char> rgb(3 * width * height);

    for (int i = 0; i < width * height; ++i) {
        unsigned char g = buffer[i]; // grayscale intensity
        rgb[3*i + 0] = g; // R
        rgb[3*i + 1] = g; // G
        rgb[3*i + 2] = g; // B
    }

    // Now call stbi_write_png with 3 channels
    int stride_in_bytes = 3 * width;
    int success = stbi_write_png(filename.c_str(),
        width,
        height,
        /*channels=*/3,
        rgb.data(),
        stride_in_bytes);

    return (success != 0);
}

/**
 * @brief Computes the Maximum Intensity Projection (MIP) of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::MIP(const Volume &vol, const std::string &outFilename)
{
    // now taking: vol.channels == 1 !!! (grayscale)
    // If have multi-channel data, need to e.g. look at volume.data[ x + w*( y + h*z)*channels ].

    int w = vol.width;
    int h = vol.height;
    int d = vol.depth;
    if (w <= 0 || h <= 0 || d <= 0) {
        std::cerr << "Volume dimensions are zero; cannot do MIP!\n";
        return;
    }    

    // will produce a 2D image of size w*h, each pixel an 8-bit intensity
    // so allocate w*h bytes in a buffer
    std::vector<unsigned char> output(w * h, 0); // zero-init

    // MIP, so: for each (x,y), look across z in [0..d-1], find maximum
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char maxVal = 0;
            for (int z = 0; z < d; ++z) {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                unsigned char val = vol.data[idx];
                if (val > maxVal) {
                    maxVal = val;
                }
            }
            output[x + w * y] = maxVal;
        }
    }

    // Now write out the resulting 2D buffer as a PNG
    if (!writeGrayPNG(outFilename, output.data(), w, h)) {
        std::cerr << "Failed to write MIP to " << outFilename << std::endl;
    }
}

/**
 * @brief Computes the Minimum Intensity Projection (MinIP) of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::MinIP(const Volume &vol, const std::string &outFilename)
{
    int w = vol.width;
    int h = vol.height;
    int d = vol.depth;

    std::vector<unsigned char> output(w * h, 255); // start with all 255

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char minVal = 255;
            for (int z = 0; z < d; ++z) {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                unsigned char val = vol.data[idx];
                if (val < minVal) {
                    minVal = val;
                }
            }
            output[x + w * y] = minVal;
        }
    }

    if (!writeGrayPNG(outFilename, output.data(), w, h)) {
        std::cerr << "Failed to write MinIP to " << outFilename << std::endl;
    }
}

/**
 * @brief Computes the Average Intensity Projection (AIP) of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::AIP(const Volume &vol, const std::string &outFilename)
{
    int w = vol.width;
    int h = vol.height;
    int d = vol.depth;

    std::vector<unsigned int> accum(w * h, 0); // 32-bit accumulators
    std::vector<unsigned char> output(w * h, 0);

    // sum up all slices
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned int sumVal = 0;
            for (int z = 0; z < d; ++z) {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                sumVal += (unsigned int)(vol.data[idx]);
            }
            accum[x + w * y] = sumVal;
        }
    }

    // now do the average
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned int sumVal = accum[x + w * y];
            // integer division
            unsigned char mean = (unsigned char)(sumVal / d);
            output[x + w * y] = mean;
        }
    }

    if (!writeGrayPNG(outFilename, output.data(), w, h)) {
        std::cerr << "Failed to write AIP to " << outFilename << std::endl;
    }
}

/**
 * @brief Computes a slab-based Maximum Intensity Projection (MIP) for a subregion of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param zStart The starting slice index for the slab.
 * @param zEnd The ending slice index for the slab.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::MIPSlab(const Volume &vol, int zStart, int zEnd, const std::string &outFilename)
{
    // 1) clamp or validate zStart, zEnd
    int startZ = std::max(0, std::min(zStart, vol.depth - 1));
    int endZ   = std::max(0, std::min(zEnd,   vol.depth - 1));
    if (startZ > endZ) {
        // If user gave reversed or invalid range, either swap or do nothing.
        // Let's just swap for convenience:
        std::swap(startZ, endZ);
    }

    int w = vol.width;
    int h = vol.height;
    // allocate output 2D buffer
    std::vector<unsigned char> output(w * h, 0);

    // for each (x,y), find max in [zStart..zEnd]
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char maxVal = 0;
            for (int z = startZ; z <= endZ; ++z) {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                unsigned char val = vol.data[idx];
                if (val > maxVal) {
                    maxVal = val;
                }
            }
            output[x + w * y] = maxVal;
        }
    }

    // save result
    if (!writeGrayPNG(outFilename, output.data(), w, h)) {
        std::cerr << "MIPSlab failed to write PNG: " << outFilename << std::endl;
    }
}

/**
 * @brief Computes a slab-based Minimum Intensity Projection (MinIP) for a subregion of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param zStart The starting slice index for the slab.
 * @param zEnd The ending slice index for the slab.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::MinIPSlab(const Volume &vol, int zStart, int zEnd, const std::string &outFilename)
{
    int startZ = std::max(0, std::min(zStart, vol.depth - 1));
    int endZ   = std::max(0, std::min(zEnd,   vol.depth - 1));
    if (startZ > endZ) {
        std::swap(startZ, endZ);
    }

    int w = vol.width;
    int h = vol.height;
    std::vector<unsigned char> output(w * h, 255); // start with 255

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char minVal = 255;
            for (int z = startZ; z <= endZ; ++z) {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                unsigned char val = vol.data[idx];
                if (val < minVal) {
                    minVal = val;
                }
            }
            output[x + w * y] = minVal;
        }
    }

    if (!writeGrayPNG(outFilename, output.data(), w, h)) {
        std::cerr << "MinIPSlab failed to write PNG: " << outFilename << std::endl;
    }
}

/**
 * @brief Computes a slab-based Average Intensity Projection (AIP) for a subregion of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param zStart The starting slice index for the slab.
 * @param zEnd The ending slice index for the slab.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::AIPSlab(const Volume &vol, int zStart, int zEnd, const std::string &outFilename)
{
    int startZ = std::max(0, std::min(zStart, vol.depth - 1));
    int endZ   = std::max(0, std::min(zEnd,   vol.depth - 1));
    if (startZ > endZ) {
        std::swap(startZ, endZ);
    }

    int w = vol.width;
    int h = vol.height;
    int d = (endZ - startZ + 1);

    // accumulators
    std::vector<unsigned int> accum(w * h, 0);
    std::vector<unsigned char> output(w * h, 0);

    // sum partial slab
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned int sumVal = 0;
            for (int z = startZ; z <= endZ; ++z) {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                sumVal += (unsigned int)(vol.data[idx]);
            }
            accum[x + w * y] = sumVal;
        }
    }

    // average
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned int sumVal = accum[x + w * y];
            unsigned char mean = (unsigned char)(sumVal / d);
            output[x + w * y] = mean;
        }
    }

    if (!writeGrayPNG(outFilename, output.data(), w, h)) {
        std::cerr << "AIPSlab failed to write PNG: " << outFilename << std::endl;
    }
}

/**
 * @brief Computes the median intensity projection of a 3D volume.
 * 
 * @param vol The input 3D volume.
 * @param outFilename The output file name for the generated projection.
 */
void Projections3D::AIPMedian(const Volume &vol, const std::string &outFilename)
{
    int w = vol.width;
    int h = vol.height;
    int d = vol.depth;

    if (w <= 0 || h <= 0 || d <= 0) {
        std::cerr << "Volume dimensions are zero; cannot do AIPMedian!\n";
        return;
    }

    // Output buffer for the resulting 2D image
    std::vector<unsigned char> output(w * h, 0);

    // For each (x,y), gather all z-values, sort them, and pick the median
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            // Collect intensity values along z
            std::vector<unsigned char> vals(d);
            for (int z = 0; z < d; ++z)
            {
                size_t idx = (size_t)x + (size_t)w * ((size_t)y + (size_t)h * z);
                vals[z] = vol.data[idx]; // grayscale intensity
            }
            // Sort the intensities
            std::sort(vals.begin(), vals.end());

            // Compute median
            unsigned char medianVal = 0;
            if (d % 2 == 1) {
                // Odd number of slices
                medianVal = vals[d / 2];
            } else {
                // Even number of slices; average the two middle values
                unsigned char v1 = vals[(d / 2) - 1];
                unsigned char v2 = vals[d / 2];
                // safe integer average
                medianVal = static_cast<unsigned char>((static_cast<int>(v1) + v2) / 2);
            }

            // Store in output image
            output[x + w * y] = medianVal;
        }
    }

    // Finally, write out the resulting 2D image as a PNG
    if (!writeGrayPNG(outFilename, output.data(), w, h))
    {
        std::cerr << "Failed to write AIPMedian to " << outFilename << std::endl;
    }
}

/**
 * @brief Applies the specified 3D projection method to a volume.
 * 
 * @param vol The input 3D volume.
 * @param projType The type of projection to apply ("MIP", "MinIP", "AIP", or "AIPMedian").
 * @param outPath The output file path for the projection result.
 * @param zStart Optional starting slice index for slab-based projections (default: full volume).
 * @param zEnd Optional ending slice index for slab-based projections (default: full volume).
 */
void Projections3D::applyProjection3D(const Volume &vol, const std::string &projType,
                                      const std::string &outPath, int zStart, int zEnd)
{
    if (projType == "MIP") {
        if (zStart > 0 || zEnd >= 0) {
            int zs = std::max(zStart, 0);
            int ze = (zEnd < 0) ? (vol.depth - 1) : std::min(zEnd, vol.depth - 1);
            std::cout << "[3D Projection] MIP (slab " << zs << ".." << ze << ") => " << outPath << "\n";
            Projections3D::MIPSlab(vol, zs, ze, outPath);
        } else {
            std::cout << "[3D Projection] MIP (full) => " << outPath << "\n";
            Projections3D::MIP(vol, outPath);
        }
    }
    else if (projType == "MinIP") {
        if (zStart > 0 || zEnd >= 0) {
            int zs = std::max(zStart, 0);
            int ze = (zEnd < 0) ? (vol.depth - 1) : std::min(zEnd, vol.depth - 1);
            std::cout << "[3D Projection] MinIP (slab) => " << outPath << "\n";
            Projections3D::MinIPSlab(vol, zs, ze, outPath);
        } else {
            std::cout << "[3D Projection] MinIP (full) => " << outPath << "\n";
            Projections3D::MinIP(vol, outPath);
        }
    }
    else if (projType == "AIP") {
        if (zStart > 0 || zEnd >= 0) {
            int zs = std::max(zStart, 0);
            int ze = (zEnd < 0) ? (vol.depth - 1) : std::min(zEnd, vol.depth - 1);
            std::cout << "[3D Projection] AIP (slab) => " << outPath << "\n";
            Projections3D::AIPSlab(vol, zs, ze, outPath);
        } else {
            std::cout << "[3D Projection] AIP (full) => " << outPath << "\n";
            Projections3D::AIP(vol, outPath);
        }
    }
    else if (projType == "AIPMedian") {
        std::cout << "[3D Projection] AIPMedian => " << outPath << "\n";
        Projections3D::AIPMedian(vol, outPath);
    }
    else {
        std::cout << "[3D Projection] " << projType << " not yet implemented => " << outPath << "\n";
    }
}
