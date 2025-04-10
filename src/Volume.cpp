/*
 * @file Volume.cpp
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

 #include "Volume.h"
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cctype>
#include <vector>
#include <utility>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include "stb_image.h"

/**
 * @brief Extracts the slice number from a filename.
 * 
 * @param filename The filename to parse.
 * @param sliceNumber Reference to an integer where the extracted slice number will be stored.
 * @return True if a valid slice number was found, false otherwise.
 */
bool parseSliceNumber(const std::string &filename, int &sliceNumber)
{
    // Must end in ".png"
    if (filename.size() < 5) return false;
    if (filename.substr(filename.size() - 4) != ".png") {
        return false; // not .png
    }

    // remove ".png"
    std::string base = filename.substr(0, filename.size() - 4);

    // Move backwards to find last digit
    int end = static_cast<int>(base.size()) - 1;
    while (end >= 0 && !std::isdigit(base[end])) {
        end--;
    }
    if (end < 0) {
        return false; // no digits
    }

    // Move backwards through digits
    int start = end;
    while (start >= 0 && std::isdigit(base[start])) {
        start--;
    }
    start++; // now 'start' is the first digit

    // Substring of digits
    std::string digits = base.substr(start, end - start + 1);
    sliceNumber = std::stoi(digits);
    return true;
}

/**
 * @brief Splits a given path into directory and prefix components.
 * 
 * @param userPath The full path provided by the user.
 * @return A pair where the first element is the directory and the second is the filename prefix.
 */
static std::pair<std::string, std::string> splitDirectoryAndPrefix(const std::string &userPath)
{
    // We'll check if `userPath` itself is a directory.
    struct stat sb;
    if (stat(userPath.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR)) {
        // It's an actual directory; the prefix is empty
        return {userPath, ""};
    }

    // Otherwise, find the last slash/backslash
    // On Windows, paths can have `\`; on Unix, `/`.
    // We'll handle either.
    size_t pos = userPath.find_last_of("/\\");
    if (pos == std::string::npos) {
        // No slash at all => directory = ".", prefix = userPath
        return {".", userPath};
    }
    else {
        // Split
        std::string dirPart = userPath.substr(0, pos);
        std::string prefixPart = userPath.substr(pos + 1);
        if (dirPart.empty()) {
            dirPart = "."; // current directory if everything was just prefix
        }
        return {dirPart, prefixPart};
    }
}

/**
 * @brief Default constructor for Volume, initializing an empty volume.
 */
Volume::Volume()
  : width(0), height(0), depth(0), channels(1)
{
    // empty
}

/**
 * @brief Constructs a Volume with given dimensions and channels.
 * 
 * @param w Width of the volume.
 * @param h Height of the volume.
 * @param d Depth (number of slices) of the volume.
 * @param c Number of channels per voxel.
 */
Volume::Volume(int w, int h, int d, int c)
  : width(w), height(h), depth(d), channels(c)
{
    data.resize(static_cast<size_t>(w)*h*d*c, 0);
}

/**
 * @brief Loads a 3D volume from a sequence of 2D image slices.
 * 
 * @param folderPath The path to the folder containing the slice images.
 * @return True if the volume was successfully loaded, false otherwise.
 */
bool Volume::loadVolumeFromSlices(const std::string &folderPath)
{
    data.clear();
    width = height = depth = 0;
    channels = 1;

    // 1) Split into (actualDir, prefix)
    auto parts = splitDirectoryAndPrefix(folderPath);
    const std::string &actualDir = parts.first;   // "Scans/TestVolume"
    const std::string &prefix    = parts.second;  // "vol"

    // 2) Attempt to open that directory
    DIR* dir = opendir(actualDir.c_str());
    if (!dir) {
        std::cerr << "Cannot open directory: " << actualDir << std::endl;
        return false;
    }

    std::vector< std::pair<std::string,int> > filesWithIndex;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string fname = entry->d_name;
        if (fname=="." || fname=="..") {
            continue;
        }

        // Check if it starts with 'prefix' (if prefix is not empty)
        if (!prefix.empty()) {
            // If the filename doesn't begin with prefix, skip
            if (fname.rfind(prefix, 0) != 0) {
                // e.g. "vol" => "vol000.png" should pass,
                // but "someOtherFile.png" fails
                continue;
            }
        }

        // Build full path: actualDir + "/" + fname
        std::string fullPath = actualDir;
        if (!fullPath.empty() && fullPath.back() != '/' && fullPath.back() != '\\') {
            fullPath += "/";
        }
        fullPath += fname;

        // Check if it's a regular file
        struct stat sb;
        if (stat(fullPath.c_str(), &sb)==0 && S_ISREG(sb.st_mode)) {
            // parse trailing digits from fname
            int sliceNum = -1;
            if (parseSliceNumber(fname, sliceNum)) {
                // check if sliceNum is in range
                if (sliceNum >= firstSlice && (lastSlice < 0 || sliceNum <= lastSlice)) {
                    filesWithIndex.push_back({ fullPath, sliceNum });
                }
            }
        }
    }
    closedir(dir);

    if (filesWithIndex.empty()) {
        std::cerr << "No slices found in " << folderPath << std::endl;
        return false;
    }

    // Sort by sliceNumber ascending
    std::sort(filesWithIndex.begin(), filesWithIndex.end(),
        [](const std::pair<std::string,int> &a, const std::pair<std::string,int> &b){
            return a.second < b.second;
        }
    );

    // 'depth' = number of slices
    depth = static_cast<int>(filesWithIndex.size());

    // Load first slice to get width/height
    {
        int w, h, c;
        unsigned char* tempData = stbi_load(filesWithIndex[0].first.c_str(), &w, &h, &c, 1);
        if (!tempData) {
            std::cerr << "Failed to load first slice: "
                      << filesWithIndex[0].first << std::endl;
            return false;
        }
        width = w;
        height = h;
        stbi_image_free(tempData);
    }

    // Allocate
    data.resize(static_cast<size_t>(width)*height*depth, 0);

    // Load each slice
    for (int z = 0; z < depth; ++z) {
        const std::string &filepath = filesWithIndex[z].first;

        int w, h, c;
        unsigned char* sliceData = stbi_load(filepath.c_str(), &w, &h, &c, 1);
        if (!sliceData) {
            std::cerr << "Failed to load slice: " << filepath << std::endl;
            return false;
        }
        if (w != width || h != height) {
            std::cerr << "Slice dimension mismatch at " << filepath << std::endl;
            stbi_image_free(sliceData);
            return false;
        }

        // Copy
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                size_t idx = col
                           + static_cast<size_t>(width) *
                             (row + static_cast<size_t>(height)* z);
                data[idx] = sliceData[row*width + col];
            }
        }
        stbi_image_free(sliceData);
    }

    std::cout << "Loaded " << depth 
              << " slices from " << folderPath << std::endl
              << "Volume dimension: "
              << width << " x " << height << " x " << depth
              << std::endl;

    return true;
}

/**
 * @brief Retrieves the voxel value at the specified coordinates.
 * 
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @param z The z-coordinate.
 * @param c The channel index (default is 0 for grayscale images).
 * @return The voxel value as an unsigned char.
 * @throws std::out_of_range If the coordinates are out of bounds.
 */
unsigned char Volume::getVoxel(int x, int y, int z, int c) const
{
    if (x < 0 || x >= width ||
        y < 0 || y >= height ||
        z < 0 || z >= depth  ||
        c < 0 || c >= channels)
    {
        throw std::out_of_range("getVoxel: index out of range");
    }
    size_t idx = c
               + channels * ( static_cast<size_t>(x)
               + static_cast<size_t>(width) *
                 ( static_cast<size_t>(y)
                 + static_cast<size_t>(height)*z ) );
    return data[idx];
}

/**
 * @brief Sets the voxel value at the specified coordinates.
 * 
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @param z The z-coordinate.
 * @param value The new voxel value.
 * @param c The channel index (default is 0 for grayscale images).
 * @throws std::out_of_range If the coordinates are out of bounds.
 */
void Volume::setVoxel(int x, int y, int z, unsigned char value, int c)
{
    if (x < 0 || x >= width ||
        y < 0 || y >= height ||
        z < 0 || z >= depth ||
        c < 0 || c >= channels)
    {
        throw std::out_of_range("setVoxel: index out of range");
    }
    size_t idx = c
               + channels * ( static_cast<size_t>(x)
               + static_cast<size_t>(width) *
                 ( static_cast<size_t>(y)
                 + static_cast<size_t>(height)*z ) );
    data[idx] = value;
}
