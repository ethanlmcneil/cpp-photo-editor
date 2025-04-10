/*
 * @file Filters3D.h
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

#ifndef FILTERS3D_H
#define FILTERS3D_H

#include "Volume.h"

class Filters3D
{
public:
    // Existing specialized blur methods:
    void apply3DGaussianBlur(Volume &volume, int kernelSize = 3, double sigma = 2.0);
    void apply3DMedianBlur(Volume &volume, int kernelSize = 3);

    // A "master" 3D blur dispatcher, to be called from main:
    void apply3DBlur(Volume &volume,
                     const std::string &blurType, // e.g. "Gaussian", "Median", "Box", etc.
                     float kernelSize,            // e.g. 3
                     float sigma = 2.0f);         // optional stdev if "Gaussian"
                     
    // Utility: Save volume slices
    void saveSlicesAsPNG(const Volume &volume,
                         const std::string &folder,
                         const std::string &prefix);
};

#endif // FILTERS3D_H
