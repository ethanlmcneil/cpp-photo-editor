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

#ifndef IMAGE_H
#define IMAGE_H

class Image {
    public:
        Image(const char* filepath);
        Image(unsigned char* input, int w, int h, int c);
        ~Image();
        
        static void WriteImage(const Image& img, const char* filepath);
        
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        int getChannels() const { return channels; }
        const unsigned char* getData() const { return data; }
        void setData(const unsigned char* newData); 
        void setChannels(int newChannels); 

    private:
        unsigned char* data;
        int width;
        int height;
        int channels;
};

#endif // IMAGE_H