# CLI Usage Instructions for APImageFilters

Welcome to APImageFilters, your go-to tool for processing images and volumes with a variety of filters and transformations. This guide will walk you through how to use the program effectively.

## Getting Started

To use APImageFilters, you need to provide:
1. An input image or data volume.
2. One or more processing options (e.g., filters, transformations).
3. An output filename (where the processed image will be saved).

After compiling your project, go into your build directory:

```bash
cd build
```

Basic Command Structure:

```bash
./APImageFilters -i <input_image> [options] <output_image>
./APImageFilters -d <data_volume> [options] <output_image>
```

- `-i <input_image>` → Specifies an image file for processing.
- `-d <data_volume>` → Specifies a volume dataset for processing.
- `<output_image>` → The final processed image file (must be .png or .jpg).
- `[options]` → One or more processing options (filters, transformations).

Example:

```bash
./APImageFilters -i photo.png -g -b 50 output.png
```

This applies greyscale and increases brightness by 50 to `photo.png`, saving the result as `output.png`.


## Image Processing Options

You can apply various filters and transformations to images. These options can be used individually or combined.

### **Basic Options**
| Feature           | Short Flag     | Long Flag         | Example Usage |
|------------------|--------------|----------------|---------------------------|
| Greyscale        | `-g`         | `--greyscale`  | `./APImageFilters -i input.png -g output.png` |
| Brightness       | `-b <value>` | `--brightness <value>` | `./APImageFilters -i input.png -b 100 output.png` |
| Histogram Equalisation | `-h <type>` | `--histogram <type>` | `./APImageFilters -i input.png -h HSV output.png` |

### **Blurring**
| Blur Type       | Short Flag | Long Flag | Example Usage |
|----------------|------------|------------|--------------------------------|
| Gaussian Blur  | `-r Gaussian <size> <stdev>` | `--blur Gaussian <size> <stdev>` | `./APImageFilters -i input.png -r Gaussian 5 2.0 output.png` |
| Box Blur       | `-r Box <size>` | `--blur Box <size>` | `./APImageFilters -i input.png -r Box 7 output.png` |
| Median Blur    | `-r Median <size>` | `--blur Median <size>` | `./APImageFilters -i input.png -r Median 3 output.png` |

### **Edge Detection**
| Edge Type       | Short Flag | Long Flag | Example Usage |
|----------------|------------|------------|--------------------------------|
| Sobel         | `-e Sobel`  | `--edge Sobel` | `./APImageFilters -i input.png -e Sobel output.png` |
| Prewitt       | `-e Prewitt` | `--edge Prewitt` | `./APImageFilters -i input.png -e Prewitt output.png` |
| Scharr        | `-e Scharr` | `--edge Scharr` | `./APImageFilters -i input.png -e Scharr output.png` |
| Roberts Cross | `-e RobertsCross` | `--edge RobertsCross` | `./APImageFilters -i input.png -e RobertsCross output.png` |

### **Other Filters**
| Feature            | Short Flag      | Long Flag          | Example Usage |
|-------------------|----------------|-------------------|---------------------------|
| Laplacian Sharpening | `-p` | `--sharpen` | `./APImageFilters -i input.png --sharpen output.png` |
| Salt & Pepper Noise | `-n <amount>` | `--saltpepper <amount>` | `./APImageFilters -i input.png -n 5 output.png` |
| Thresholding       | `-t <value> <type>` | `--threshold <value> <type>` | `./APImageFilters -i input.png -t 128 HSV output.png` |

---

## Volume Processing Options

### **Volume Input & Output**
| Feature         | Short Flag | Long Flag | Example Usage |
|----------------|------------|------------|--------------------------------|
| Input Volume   | `-d <data_volume>` | None | `./APImageFilters -d volume_folder output.png` |
| Output Image   | None       | None      | Must be `.png` or `.jpg` |

### **Reading Slices from Volume**
| Feature       | Short Flag | Long Flag | Example Usage |
|--------------|------------|------------|--------------------------------|
| First Index  | `-f <index>` | `--first <index>` | `./APImageFilters -d volume -f 1 output.png` |
| Last Index   | `-l <index>` | `--last <index>` | `./APImageFilters -d volume -l 10 output.png` |
| File Extension | `-x <ext>` | `--extension <ext>` | `./APImageFilters -d volume -x jpg output.png` |

**If `--first` and `--last` are not specified, all volume images are read.**

### **Blurring a Volume**
| Feature       | Short Flag | Long Flag | Example Usage |
|--------------|------------|------------|--------------------------------|
| Blur on Volume | `-r <type> <size> [<stdev>]` | `--blur <type> <size> [<stdev>]` | `./APImageFilters -d volume -r Gaussian 3 2.0 output.png` |

### **Slicing & Projection**
| Feature       | Short Flag | Long Flag | Example Usage |
|--------------|------------|------------|--------------------------------|
| Slice a Volume | `-s <plane> <constant>` | `--slice <plane> <constant>` | `./APImageFilters -d volume -s XZ 16 output.png` |
| Projection   | `-p <type>` | `--projection <type>` | `./APImageFilters -d volume -p MIP output.png` |

**Available projections:**  
- `MIP` (Maximum Intensity Projection)  
- `MinIP` (Minimum Intensity Projection)  
- `meanAIP` (Mean Average Intensity Projection)  
- `medianAIP` (Median Average Intensity Projection)  

---

## **Example Commands**
- Convert an image to greyscale:
  ```bash
  ./APImageFilters -i input.png -g output.png
  ```
- Apply brightness and Gaussian blur:
  ```bash
  ./APImageFilters -i input.png -b 50 -r Gaussian 5 2.0 output.png
  ```
- Detect edges using Sobel:
  ```bash
  ./APImageFilters -i input.png --edge Sobel output.png
  ```
- Extract a **slice** from a volume:
  ```bash
  ./APImageFilters -d volume -s XZ 16 output.png
  ```
- Apply a **Maximum Intensity Projection**:
  ```bash
  ./APImageFilters -d volume -p MIP output.png
  ```