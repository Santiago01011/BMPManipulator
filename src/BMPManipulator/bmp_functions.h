#ifndef BMP_FUNCTIONS_H_INCLUDED
#define BMP_FUNCTIONS_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// @todo (ZAPATASANTIAGO#7#02/06/25): Add support to more BMP types
#define HEADER_SIZE 54

typedef struct __attribute__((packed)) {
    char signature[2];
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t headerSize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitDepth;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t xResolution;
    uint32_t yResolution;
    uint32_t colorTableSize;
    uint32_t colorCount;
}BMPHeader;

typedef struct {
    BMPHeader base;  // Keep standard fields (first 54 bytes)

    // Extra fields (present if headerSize >= 108)
    uint32_t redMask;
    uint32_t greenMask;
    uint32_t blueMask;
    uint32_t alphaMask;   // Only for 32-bit images with transparency
} BMPHeaderExtended;

typedef struct {
    uint8_t b, g, r;
}PixelRGB;

typedef struct {
    BMPHeader metadata;
    PixelRGB *pixels;
    short angle;
}ImageBMP;




int calculatePitch(BMPHeader* metadata);
ImageBMP loadImageBMPM(const char* filePath);
void WriteImageBMPM(const char *fileName, PixelRGB *pixels, BMPHeader imgHeader);

#endif // BMP_FUNCTIONS_H_INCLUDED
