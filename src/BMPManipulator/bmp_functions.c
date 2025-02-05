#include "bmp_functions.h"

int loadImageBMPM(char* filePath, BMPMetadata* metadata, unsigned char** originalImage){
    FILE *imgFile = fopen(filePath, "rb");
    unsigned char header[HEADER_SIZE];
    if (!imgFile) {
        printf("Error opening file: %s\n", filePath);
        return 0;
    }

    fread(header, HEADER_SIZE, 1, imgFile);
    if ( header[0] != 'B' || header[1] != 'M') {
        printf("The file %s is not a supported file\nNOT BMP", filePath);
        fclose(imgFile);
        return 0;
    }

    // Copy the header of the file into our structure
    memcpy(metadata->signature, header, 2);
    metadata->fileSize = *(unsigned int*)(header + 2);
    metadata->reserved = *(unsigned int*)(header + 6);
    metadata->dataOffset = *(unsigned int*)(header + 10);
    metadata->headerSize = *(unsigned int*)(header + 14);
    metadata->width = *(int*)(header + 18);
    metadata->height = *(int*)(header + 22);
    metadata->planes = *(unsigned short*)(header + 26);
    metadata->bitsPerPixel = *(unsigned short*)(header + 28);
    metadata->compression = *(unsigned int*)(header + 30);
    metadata->imageSize = *(unsigned int*)(header + 34);
    metadata->xPixelsPerM = *(int*)(header + 38);
    metadata->yPixelsPerM = *(int*)(header + 42);
    metadata->colorsUsed = *(unsigned int*)(header + 46);
    metadata->importantColors = *(unsigned int*)(header + 50);

    if (metadata->bitsPerPixel != 24 || metadata->compression != 0){
        printf("The file %s is not a supported file\n", filePath);
        fclose(imgFile);
        return 0;
    }

    // Calculate row size and padding
    /**
    Little breakdown of what we´re doing here:
        ( meta->width * 3 ) calculates the total number of bytes required to store the pixel data for each row,
        assuming 3 bytes per pixel (24-bit color).
        ( +3 ) adds 3 to the result, which is equivalent to adding 3 bytes of padding to ensure that the division by 4
        rounds up to the nearest multiple of 4.
        ( /4 ) divides the result by 4, effectively rounding down to the nearest multiple of 4.
        ( *4 ) multiplies the result by 4, which gives the total row size with padding.
    */
    const int rowSize = ((metadata->width * 3 + 3) / 4) * 4;
    const int expectedSize = rowSize * metadata->height;

    //Allocate pixel buffer
    *originalImage = (unsigned char*)malloc(expectedSize);

    // Seek to the start of the image data
    fseek(imgFile, metadata->dataOffset, SEEK_SET);

    if(fread(*originalImage, expectedSize, 1, imgFile) != 1) {
        puts("Error reading pixel data");
        free(*originalImage);
        fclose(imgFile);
        return 0;
    }

    fclose(imgFile);

    return 1;
}

