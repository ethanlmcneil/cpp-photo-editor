/**
 * @file Slicing3D.h
 * @brief Header file for the Slicing3D class, providing functionality to extract 2D slices from 3D volumes
 * @author Your Name
 *
 * This class provides methods for extracting 2D slices from 3D volumes along principal planes (xy, xz, yz).
 * Group Members:
 * - Jiaqi    (GitHub: esemsc-jc1424)
 * - Daicong  (GitHub: esemsc-c730ef50)
 * - Ida      (GitHub: esemsc-ifc24)
 * - Zhuyi    (GitHub: esemsc-zf1124)
 * - Dany     (GitHub: esemsc-dh324)
 * - Ethan    (GitHub: edsml-elm224)
 * - Keyun    (GitHub: esemsc-km824)
 */

#ifndef SLICING3D_H
#define SLICING3D_H

#include "Volume.h"
#include <string>

class Slicing3D {
public:
    /**
     * @brief Extract a slice from a 3D volume along a given plane
     *
     * @param vol The 3D volume to slice
     * @param plane The plane to slice along ("xy", "xz", "yz")
     * @param coordinate The coordinate at which to extract the slice
     * @param outputPath The path to save the resulting 2D image
     */
    static void slice3D(const Volume& vol, const std::string& plane, int coordinate, const std::string& outputPath);

private:
    /**
     * @brief Extract a slice in the xy plane at a given z-coordinate
     *
     * @param vol The 3D volume to slice
     * @param z The z-coordinate at which to extract the slice
     * @param outputPath The path to save the resulting image
     */
    static void sliceXY(const Volume& vol, int z, const std::string& outputPath);

    /**
     * @brief Extract a slice in the xz plane at a given y-coordinate
     *
     * @param vol The 3D volume to slice
     * @param y The y-coordinate at which to extract the slice
     * @param outputPath The path to save the resulting image
     */
    static void sliceXZ(const Volume& vol, int y, const std::string& outputPath);

    /**
     * @brief Extract a slice in the yz plane at a given x-coordinate
     *
     * @param vol The 3D volume to slice
     * @param x The x-coordinate at which to extract the slice
     * @param outputPath The path to save the resulting image
     */
    static void sliceYZ(const Volume& vol, int x, const std::string& outputPath);
};

#endif // SLICING3D_H