#ifndef BMP_FUNCTIONS_H_INCLUDED
#define BMP_FUNCTIONS_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define HEADER_SIZE 54

typedef struct {
    // File Header (14 bytes)
    char signature[2];      // 'BM'
    uint32_t fileSize;      // Total file size
    uint32_t reserved;      // Unused
    uint32_t dataOffset;    // Pixel data offset

    // Info Header (40 bytes)
    uint32_t headerSize;    // Info header size (40)
    int32_t width;          // Image width in pixels
    int32_t height;         // Image height in pixels
    uint16_t planes;        // Must be 1
    uint16_t bitsPerPixel;  // Color depth (1, 4, 8, 24, 32)
    uint32_t compression;   // Compression method
    uint32_t imageSize;     // Raw bitmap data size (including padding)
    int32_t xPixelsPerM;    // Horizontal resolution
    int32_t yPixelsPerM;    // Vertical resolution
    uint32_t colorsUsed;    // Number of colors in palette
    uint32_t importantColors;
} BMPMetadata;


int loadImageBMPM(char* filePath, BMPMetadata* metadata, unsigned char** originalImage);
int grayScaleBMPM(BMPMetadata* metadata, unsigned char* originalImage);


#endif // BMP_FUNCTIONS_H_INCLUDED
