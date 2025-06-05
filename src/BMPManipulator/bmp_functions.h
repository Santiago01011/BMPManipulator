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
    uint8_t b, g, r, a;
} PixelBGRA;

typedef struct {
    BMPHeader metadata;
    PixelBGRA *pixels;
    short angle;
} ImageBMP;

int calculatePitch(BMPHeader* metadata);
ImageBMP *BMPM_loadImage(const char* filePath);
void BMPM_saveImage(const char *fileName, ImageBMP *image);
void BMPM_freeImage(ImageBMP *image);

#endif // BMP_FUNCTIONS_H_INCLUDED
