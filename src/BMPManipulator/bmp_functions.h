#ifndef BMP_FUNCTIONS_H_INCLUDED
#define BMP_FUNCTIONS_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

// @todo (ZAPATASANTIAGO#7#02/06/25): Add support to more BMP types
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
    short angle;            // To display rotated images, not used for file values
} BMPMetadata;

typedef struct {
    BMPMetadata base;  // Keep standard fields (first 54 bytes)

    // Extra fields (present if headerSize >= 108)
    uint32_t redMask;
    uint32_t greenMask;
    uint32_t blueMask;
    uint32_t alphaMask;   // Only for 32-bit images with transparency
} BMPMetadataExtended;


int calculatePitch(BMPMetadata* metadata);
int loadImageBMPM(const char* filePath, BMPMetadata* metadata, unsigned char** originalImage);
int grayScaleBMPM(BMPMetadata* metadata, unsigned char* originalImage);
int rotateBMPM(BMPMetadata *metadata, unsigned char** originalImage, const char dir);



#endif // BMP_FUNCTIONS_H_INCLUDED
