/**
 * @file Filters3D.cpp
 * @brief Implementation of 3D image processing filters.
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
#include "Filters3D.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include "stb_image_write.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

/**
 * @brief Creates a directory if it does not exist.
 * @param dirName Name of the directory to create.
 */
void createDirectory(const std::string &dirName)
{
    struct stat info;
    if (stat(dirName.c_str(), &info) != 0)
    {
        mkdir(dirName.c_str(), 0777); // Create directory with full permissions
    }
}

/**
 * @brief Saves all slices of a volume as PNG images.
 * @param volume The 3D volume data.
 * @param folder The output folder where images will be saved.
 * @param prefix The prefix for saved image file names.
 */
void Filters3D::saveSlicesAsPNG(const Volume &volume, const std::string &folder, const std::string &prefix)
{
    if (volume.depth == 0 || volume.width == 0 || volume.height == 0)
    {
        std::cerr << "Error: Volume data is empty!" << std::endl;
        return;
    }

    int width = volume.width;
    int height = volume.height;
    int depth = volume.depth;

    // Create the output directory if it doesn't exist
    createDirectory(folder);

    // Save all slices
    for (int z = 0; z < depth; ++z)
    {
        std::vector<unsigned char> sliceData(width * height);

        // Copy slice data
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                sliceData[y * width + x] = volume.getVoxel(x, y, z);
            }
        }

        // Generate filename
        std::string filename = folder + "/" + prefix + "_slice_" + std::to_string(z) + ".png";

        // Save as PNG (1 channel = grayscale)
        if (stbi_write_png(filename.c_str(), width, height, 1, sliceData.data(), width))
        {
            std::cout << "Saved: " << filename << std::endl;
        }
        else
        {
            std::cerr << "Error saving PNG: " << filename << std::endl;
        }
    }
}

/**
 * @brief Generates a 1D Gaussian kernel for convolution.
 * @param kernelSize Size of the kernel.
 * @param sigma Standard deviation of the Gaussian function.
 * @return A vector representing the Gaussian kernel.
 */
std::vector<double> generateGaussianKernel(int kernelSize, double sigma)
{
    int radius = kernelSize / 2;
    std::vector<double> kernel(kernelSize);
    double sum = 0.0;

    for (int i = -radius; i <= radius; ++i)
    {
        double value = std::exp(-(i * i) / (2.0 * sigma * sigma));
        kernel[i + radius] = value;
        sum += value;
    }

    // Normalize the kernel
    for (double &val : kernel)
        val /= sum;

    return kernel;
}

/**
 * @brief Applies a 3D Gaussian blur to a volume using separable convolutions.
 * @param volume The 3D volume to process.
 * @param kernelSize Size of the Gaussian kernel.
 * @param sigma Standard deviation of the Gaussian function.
 */
void Filters3D::apply3DGaussianBlur(Volume &volume, int kernelSize, double sigma)
{
    if (kernelSize % 2 == 0)
        kernelSize += 1; // Ensure odd kernel size

    int width = volume.width;
    int height = volume.height;
    int depth = volume.depth;
    int radius = kernelSize / 2;

    std::vector<double> kernel = generateGaussianKernel(kernelSize, sigma);
    std::vector<unsigned char> tempData(volume.data.size());

    // Step 1: Apply 1D Gaussian Blur in X direction
    for (int z = 0; z < depth; ++z)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                double sum = 0.0, weightSum = 0.0;
                for (int dx = -radius; dx <= radius; ++dx)
                {
                    int nx = std::max(0, std::min(x + dx, width - 1));
                    double weight = kernel[dx + radius];
                    sum += volume.getVoxel(nx, y, z) * weight;
                    weightSum += weight;
                }
                tempData[x + width * (y + height * z)] = static_cast<unsigned char>(sum / weightSum);
            }
        }
    }

    // Step 2: Apply 1D Gaussian Blur in Y direction
    std::vector<unsigned char> tempData2(volume.data.size());
    for (int z = 0; z < depth; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                double sum = 0.0, weightSum = 0.0;
                for (int dy = -radius; dy <= radius; ++dy)
                {
                    int ny = std::max(0, std::min(y + dy, height - 1));
                    double weight = kernel[dy + radius];
                    sum += tempData[x + width * (ny + height * z)] * weight;
                    weightSum += weight;
                }
                tempData2[x + width * (y + height * z)] = static_cast<unsigned char>(sum / weightSum);
            }
        }
    }

    // Step 3: Apply 1D Gaussian Blur in Z direction
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int z = 0; z < depth; ++z)
            {
                double sum = 0.0, weightSum = 0.0;
                for (int dz = -radius; dz <= radius; ++dz)
                {
                    int nz = std::max(0, std::min(z + dz, depth - 1));
                    double weight = kernel[dz + radius];
                    sum += tempData2[x + width * (y + height * nz)] * weight;
                    weightSum += weight;
                }
                volume.data[x + width * (y + height * z)] = static_cast<unsigned char>(sum / weightSum);
            }
        }
    }
}

/**
 * @brief Applies a 3D median blur to a volume.
 * @param volume The 3D volume to process.
 * @param kernelSize Size of the median filter kernel.
 */
void Filters3D::apply3DMedianBlur(Volume &volume, int kernelSize)
{
    if (kernelSize % 2 == 0)
        kernelSize += 1; // Ensure kernel size is odd
    int radius = kernelSize / 2;

    int width = volume.width;
    int height = volume.height;
    int depth = volume.depth;

    std::vector<unsigned char> newData(volume.data.size(), 0);
    int histogram[256]; // Histogram array for counting sort (0-255 values)

    // Iterate over every voxel in the volume
    for (int z = 0; z < depth; ++z)
    {
        for (int y = 0; y < height; ++y)
        {
            memset(histogram, 0, sizeof(histogram)); // Reset histogram for the new row

            // Initialize histogram for the first column
            for (int dz = -radius; dz <= radius; ++dz)
            {
                for (int dy = -radius; dy <= radius; ++dy)
                {
                    for (int dx = -radius; dx <= radius; ++dx)
                    {
                        int nz = std::max(0, std::min(z + dz, depth - 1));
                        int ny = std::max(0, std::min(y + dy, height - 1));
                        int nx = std::max(0, std::min(dx, width - 1));
                        histogram[volume.getVoxel(nx, ny, nz)]++;
                    }
                }
            }

            // Compute the first median
            int count = 0, medianValue = 0;
            int medianPos = (kernelSize * kernelSize * kernelSize) / 2;
            for (int i = 0; i < 256; ++i)
            {
                count += histogram[i];
                if (count > medianPos)
                {
                    medianValue = i;
                    break;
                }
            }
            newData[z * width * height + y * width] = medianValue;

            // Move the window horizontally across the row
            for (int x = 1; x < width; ++x)
            {
                // Remove the outgoing column
                for (int dz = -radius; dz <= radius; ++dz)
                {
                    for (int dy = -radius; dy <= radius; ++dy)
                    {
                        int nz = std::max(0, std::min(z + dz, depth - 1));
                        int ny = std::max(0, std::min(y + dy, height - 1));
                        int oldNx = std::max(0, x - radius - 1);
                        histogram[volume.getVoxel(oldNx, ny, nz)]--;
                    }
                }

                // Add the new column
                for (int dz = -radius; dz <= radius; ++dz)
                {
                    for (int dy = -radius; dy <= radius; ++dy)
                    {
                        int nz = std::max(0, std::min(z + dz, depth - 1));
                        int ny = std::max(0, std::min(y + dy, height - 1));
                        int newNx = std::min(width - 1, x + radius);
                        histogram[volume.getVoxel(newNx, ny, nz)]++;
                    }
                }

                // Compute new median
                count = 0, medianValue = 0;
                for (int i = 0; i < 256; ++i)
                {
                    count += histogram[i];
                    if (count > medianPos)
                    {
                        medianValue = i;
                        break;
                    }
                }

                // Store new median
                newData[z * width * height + y * width + x] = medianValue;
            }
        }
    }

    volume.data = newData;
}

/**
 * @brief Applies a selected 3D blur filter to a volume.
 * @param volume The 3D volume to process.
 * @param blurType Type of blur to apply ("Gaussian" or "Median").
 * @param kernelSize Size of the filter kernel.
 * @param sigma Standard deviation (only for Gaussian blur).
 */
void Filters3D::apply3DBlur(Volume &volume,
                            const std::string &blurType,
                            float kernelSize,
                            float sigma /*=2.0f*/)
{
    if (blurType == "Gaussian")
    {
        // cast kernelSize to int, pass sigma
        apply3DGaussianBlur(volume, (int)kernelSize, sigma);
    }
    else if (blurType == "Median")
    {
        // cast kernelSize to int
        apply3DMedianBlur(volume, (int)kernelSize);
    }
    else
    {
        // You could implement "Box" or any other 3D blur type here
        std::cerr << "[WARN] Unknown 3D blur type: " << blurType << "\n";
    }
}
