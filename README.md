# APImageFilters
Welcome to **APImageFilters** implemented by group Edmonds.

This project enables you to apply various filters to your images and scans. For 2D images, these are:
- Brightness
- Greyscale
- Histogram Equalisation
- Blur (Gaussian)
- Edge Detection (Sobel)
- Laplacian Sharpening
- Salt and Pepper Noise
- Threshold

For 3D scanes:
- Volume Slice (XZ)
- Volume Projection (MIP)

Enjoy!

## Installation instructions

First, clone the repository:

```bash
git clone https://github.com/ese-ada-lovelace-2024/advanced-programming-group-edmonds.git
cd advanced-programming-group-edmonds
```

### MacOS

Install cmake and a C++ compiler if you don't already have them:

```bash
brew install cmake
brew install gcc
```

On Window, you can download CMake from [here](https://cmake.org/download/).

Then, create a build directory and run cmake:

For Mac Users:
```bash
cmake -S . -B build
```

Finally, compile the project:

```bash
cmake --build build
```

### Windows

Download CMake from [here](https://cmake.org/download/).

First, in Windows Powershell, clone the repository:

Then, create a build directory and run cmake:

```pwsh
cmake -S . -B build -G "Visual Studio 17 2022"
```

Replace "Visual Studio 17 2022" with the appropriate version if you are using a different version of Visual Studio.

Open the generated solution file (.sln) in Visual Studio:

```pwsh
start build\APImageFilters.sln
```

Finally, compile the project in Visual Studio.

## Usage instructions

The program is used with command line options. Documentation about functionalities can be found in [CLI Documentation](CLIDocumentation.md), which serves as our documentation.

### Usage instructions for Devs

The functionalities are divided into 6 classes:
- `Image.h`: Reads and write an image file through its path, keep records of its data members such as height and width.
- `Filters2D.h`: Applies filters for images
- `Filters3D.h`: Applies filters for 3D scans
- `Projections3D.h`: Generated 2D images from 3D volumes by applying autographic projections.
- `Slicing3D.h`: Extracts 2D slices from 3D volumes and saves them as PNG images.
- `Volume.h`: Loads Volume from slices.

You can also make modifications to the command line interface: `CommandLine.cpp` & `CommandLine.h`.

After modification, you need to re-compile the project:

```bash
cmake --build build
```

Or for Windows, you can compile it directly in Visual Studio.

## Testing

The project also includes a `tests/` directory, including many unittests for the above classes. Tests for each class have their separate file, in the following format: `tests/*ClassName*Tests.h` and `tests/*ClassName*Tests.cpp`.

There are two ways to run the tests:

1. Using `ctest`:
```bash
cd build && ctest --output-on-failure $$ cd ..
```

2. Using `./TestRunner`:
```bash
cd build && ./TestRunner $$ cd ..
```

`ctest` runs all unit tests encapsulated into one: `RunCustomUnitTests`, as well as other tests written by the teaching team. If at least one fails, all custom unit tests will be printed showing which one passed and failed, and why. `./TestRunner` will test only the custom unit tetss and prints them all whatever their outcome.

Note that we need to go into our build directory to run the tests. The above command goes out then exit this directory so we do not get confused with the path we need to use for compiling. This way, we remain in the root directory.

For developers, after adding tests you can simply recompile the project using the same ways mentioned earlier.


## CT Scans
You can download some CT Scan datasets here if you'd like:
https://imperiallondon-my.sharepoint.com/:u:/g/personal/tmd02_ic_ac_uk/EafXMuNsbcNGnRpa8K62FjkBvIKvCswl1riz7hPDHpHdSQ