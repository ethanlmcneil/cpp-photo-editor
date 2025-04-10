/*
 * @file Volume.h
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

#ifndef VOLUME_H
#define VOLUME_H

#include <vector>
#include <string>

class Volume {
public:
    int width;
    int height;
    int depth;
    int channels;  // e.g. 1 for grayscale, 3 for RGB, etc

    // Raw storage: a single std::vector to hold all voxel data
    // Size will be width * height * depth * channels
    std::vector<unsigned char> data;

    int firstSlice = 1;
    int lastSlice  = -1;        // -1 can indicate "not set => load all"
    std::string extension = "png"; // default extension for slices

    // Constructors
    Volume();
    Volume(int w, int h, int d, int c = 1);

    // (A) Loading volume data (from a folder of 2D slices, for example)
    //     This is just a stub; the actual logic can be done by the person in charge of I/O
    bool loadVolumeFromSlices(const std::string& folderPath);

    // Basic accessors/mutators for voxel data
    unsigned char getVoxel(int x, int y, int z, int c = 0) const;
    void setVoxel(int x, int y, int z, unsigned char value, int c = 0);
};

#endif // VOLUME_H
