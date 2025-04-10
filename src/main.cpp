/**
 * @file main.cpp
 * @brief Command line utility for processing 2D images or 3D volumes.
 *
 * Usage:
 *   For 2D image: ./Program -i <input_image> [filter options] <output_image>
 *   For 3D volume: ./Program -d <input_volume> [volume options] <output_image>
 *
 * Filter options for 2D image processing:
 *   Greyscale:      --greyscale or -g
 *   Brightness:     --brightness <value> or -b <value>
 *   Histogram:      --histogram <space> or -h <space> (e.g., HSV, HSL)
 *   Blur:           --blur <type> <size> [<stdev>] or -r <type> <size> [<stdev>]
 *                   (e.g., Gaussian 5 2.0, Box 7, Median 3)
 *   Edge Detection: --edge <type> or -e <type> (e.g., Sobel, Prewitt, Scharr, RobertsCross)
 *   Sharpening:     --sharpen or -p
 *   SaltPepper:     --saltpepper <amount> or -n <amount>
 *   Threshold:      --threshold <value> or -t <value> (e.g., 128 , 64 )
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

 #include "stb_image.h"
 #include "stb_image_write.h"
 
 #include <iostream>
 #include <string>
 #include <vector>
 #include <sys/stat.h>
 
 #include "Volume.h"
 #include "Projections3D.h"
 #include "CommandLine.h"
 #include "Slicing3D.h"
 #include "Filters2D.h"
 #include "Filters3D.h"
 #include "Image.h"
 
/**
 * @brief Helper function to check if a given path is a regular file (not a directory).
 * 
 * This function checks if the specified path points to a regular file, not a directory or other types
 * of file system objects.
 * 
 * @param path The path to check.
 * @return True if the path is a regular file, false otherwise.
 */
 static bool isRegularFile(const std::string &path) {
     struct stat sb;
     return (stat(path.c_str(), &sb) == 0 && (sb.st_mode & S_IFMT) == S_IFREG);
 }

/**
 * @brief Main entry point of the program.
 * 
 * This function processes command-line arguments to determine whether to run in image mode or volume mode.
 * Depending on the mode, it loads the necessary data, applies the specified operations, and saves the result.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit status code. Returns 0 on success, 1 on error.
 */
 int main(int argc, char* argv[])
{
    // Parse command line arguments into opts.
    CommandOptions opts = CommandLine::parseArgs(argc, argv);

    // ------------------- 2D image mode -------------------
    if (opts.isImage) {
        if (!isRegularFile(opts.inputPath)) {
            std::cerr << "ERROR: Input file not found: " << opts.inputPath << "\n";
            return 1;
        }

        // Load the 2D image
        Image img(opts.inputPath.c_str());
        Filters2D filter2d;

        // Process each operation specified on the command line
        for (auto &op : opts.operations) {
            const std::string &nm = op.name;
            const std::string &st = op.subtype;
            const auto &vals = op.floats;

            if (nm == "greyscale") {
                filter2d.apply_Greyscale(img);
            }
            else if (nm == "brightness") {
                float bVal = vals.empty() ? 0.f : vals[0];
                filter2d.apply_Brightness(img, static_cast<int>(bVal));
            }
            else if (nm == "histogram") {
                if (st == "HSV" || st == "HSL") {
                    filter2d.apply_Histogram_Equalisation(img, st);
                } else {
                    std::cerr << "[WARN] Invalid histogram space: " << st << " (defaulting to HSL)\n";
                    filter2d.apply_Histogram_Equalisation(img, "HSL");
                }
            }
            else if (nm == "blur") {
                float sz  = vals.size() > 0 ? vals[0] : 3.f;
                float dev = vals.size() > 1 ? vals[1] : 2.f;

                if (st == "Gaussian") {
                    filter2d.gaussianBlur(img, static_cast<int>(sz), dev);
                }
                else if (st == "Box") {
                    filter2d.boxBlur(img, static_cast<int>(sz));
                }
                else if (st == "Median") {
                    filter2d.medianBlur(img, static_cast<int>(sz));
                }
                else {
                    std::cerr << "[WARN] Unknown 2D blur type: " << st << "\n";
                }
            }
            else if (nm == "edge") {
                filter2d.apply_Greyscale(img);
                EdgeDetectorType edgeType = filter2d.GetEdgeDetectorType(st);
                filter2d.DetectEdges(img, edgeType);
            }
            else if (nm == "sharpen") {
                filter2d.Sharpen(img);
            }
            else if (nm == "saltpepper") {
                float amt = vals.empty() ? 5.f : vals[0];
                filter2d.apply_Salt_and_Pepper_Noise(img, amt);
            }
            else if (nm == "threshold") {
                float thr = vals.empty() ? 127.f : vals[0];
                if (st == "HSV" || st == "HSL") {
                    filter2d.Threshold(img, static_cast<int>(thr), st);
                } else {
                    std::cerr << "[WARN] Invalid threshold space: " << st << " (defaulting to HSL)\n";
                    filter2d.Threshold(img, static_cast<int>(thr), "HSL");
                }
            }
            else {
                std::cerr << "[WARN] Unimplemented 2D op: " << nm << "\n";
            }
        }

        // Save the final 2D result
        Image::WriteImage(img, opts.outputPath.c_str());
        return 0;
    }

    // ------------------- 3D volume mode -------------------
    else if (opts.isVolume) {
        // Load the volume
        Volume vol;
        vol.firstSlice = (opts.firstIndex < 1 ? 1 : opts.firstIndex);
        vol.lastSlice  = opts.lastIndex;
        vol.extension  = opts.volumeExt;

        if (!vol.loadVolumeFromSlices(opts.inputPath)) {
            std::cerr << "Failed to load volume from " << opts.inputPath << "\n";
            return 1;
        }

        Filters3D filters3d; // We'll use this for blur & slicing

        // Process each operation specified on the command line
        for (auto &op : opts.operations) {
            const std::string &nm = op.name;
            const std::string &st = op.subtype;
            const auto &vals = op.floats;

            if (nm == "blur") {
                float sz  = vals.size() > 0 ? vals[0] : 3.f;
                float dev = vals.size() > 1 ? vals[1] : 2.f;
                filters3d.apply3DBlur(vol, st, sz, dev);
            }
            /*else if (nm == "slice") {
                if (vals.empty()) {
                    std::cerr << "ERROR: slice has no param.\n";
                    continue;
                }
                float c = vals[0];
                filters3d.slice3D(vol, st, c, opts.outputPath);
                std::cout << "[Done] slice => " << opts.outputPath << "\n";
                return 0;
            }*/
            else if (nm == "slice") {
                if (vals.empty()) {
                    std::cerr << "ERROR: slice has no param.\n";
                    continue;
                }
                float c = vals[0];
            
                // Instead of filters3d.slice3D(...):
                // Call the dedicated Slicing3D method and pass c as an integer (if that is desired):
                Slicing3D::slice3D(vol, st, static_cast<int>(c), opts.outputPath);
            
                std::cout << "[Done] slice => " << opts.outputPath << "\n";
                return 0;
            }
            else if (nm == "projection") {
                Projections3D::applyProjection3D(vol, st, opts.outputPath,
                                                 opts.firstIndex, opts.lastIndex);
                std::cout << "[Done] projection => " << opts.outputPath << "\n";
                return 0;
            }
            else {
                std::cout << "[WARN] Unimplemented volume op: " << nm << "\n";
            }
        }
        return 0;
    }

    std::cerr << "ERROR: Neither image nor volume.\n";
    return 1;
}
