/*
 * @file CommandLine.h
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

#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>
#include <vector>

/**
 * FilterOption: a single operation (2D or 3D).
 *   name: Represents the primary operation name (like "greyscale" for 2D or "slice" for 3D).
 *   subtype: Provides extra detail on the type of operation (like "Gaussian" for a blur, or "MIP" for a projection).
 *   floats: Holds any numeric parameters needed by the operation (e.g., kernel size, threshold value, etc.).
 */
struct FilterOption {
    std::string name;          ///< Main identifier of the operation
    std::string subtype;       ///< Additional qualifier or variety of the operation
    std::vector<float> floats; ///< Numeric parameters for the operation
};

/**
 * CommandOptions: Stores results after parsing the command line.
 *   - isImage / isVolume indicate the mode (-i for images, -d for volumes).
 *   - inputPath / outputPath are the paths for the input and output respectively.
 *   - firstIndex, lastIndex, volumeExt are used if it's a volume (to read slices).
 *   - operations holds all filters/operations in order.
 */
struct CommandOptions {
    bool isImage  = false;     ///< True if user selected an image mode (-i)
    bool isVolume = false;     ///< True if user selected a volume mode (-d)
    std::string inputPath;     ///< The input file or directory path
    std::string outputPath;    ///< The resulting output file name

    int firstIndex = -1;       ///< Starting index for volume slices
    int lastIndex  = -1;       ///< Ending index for volume slices (if needed)
    std::string volumeExt = "png"; ///< File extension for volume slices

    std::vector<FilterOption> operations; ///< Sequence of operations (filters or transforms)
};

/**
 * The CommandLine class: used to parse the command line according
 * to a specification that can handle both 2D image tasks and 3D volume tasks.
 * It converts raw arguments into a CommandOptions structure.
 */
class CommandLine {
public:
    /**
     * parseArgs: Takes in (argc, argv) and returns a CommandOptions struct
     * with the fully parsed information, including mode (image/volume),
     * input/output paths, any volume reading parameters, and the list of operations.
     */
    static CommandOptions parseArgs(int argc, char** argv);
};

#endif // COMMANDLINE_H
