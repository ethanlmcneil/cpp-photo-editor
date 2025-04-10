/*
 * @file Projections3D.h
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

#ifndef PROJECTIONS3D_H
#define PROJECTIONS3D_H

#include <string>
#include "Volume.h"

class Projections3D {
public:
    // Maximum Intensity Projection
    static void MIP(const Volume &vol, const std::string &outFilename);

    // Minimum Intensity Projection
    static void MinIP(const Volume &vol, const std::string &outFilename);

    // Average Intensity Projection
    static void AIP(const Volume &vol, const std::string &outFilename);

    // Optional partial-slab versions of MIP, MinIP, AIP.
    // zStart, zEnd define the subrange in [0..vol.depth-1].
    static void MIPSlab(const Volume &vol, int zStart, int zEnd, const std::string &outFilename);
    static void MinIPSlab(const Volume &vol, int zStart, int zEnd, const std::string &outFilename);
    static void AIPSlab(const Volume &vol, int zStart, int zEnd, const std::string &outFilename);
    static void AIPMedian(const Volume &vol, const std::string &outFilename);

    static void applyProjection3D(const Volume &vol, const std::string &projType,
                                  const std::string &outPath, int zStart, int zEnd);

private:
    // Helper to write out a 2D buffer as PNG
    static bool writeGrayPNG(const std::string &filename,
                             const unsigned char *buffer,
                             int width,
                             int height);
};

#endif // PROJECTIONS3D_H
