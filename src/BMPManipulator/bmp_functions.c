#include "bmp_functions.h"


int calculatePitch(BMPHeader *metadata) {
    if (metadata->bitDepth == 32)
        return metadata->width * 4;
    int bytesPerPixel = metadata->bitDepth / 8;
    return ((metadata->width * bytesPerPixel + 3) / 4) * 4;
}

void _testHeader(BMPHeader *metadata) {
    printf("Signature: %c%c\n", metadata->signature[0], metadata->signature[1]);
    printf("File Size: %u bytes\n", metadata->fileSize);
    printf("Data Offset: %u bytes\n", metadata->dataOffset);
    printf("Header Size: %u bytes\n", metadata->headerSize);
    printf("Width: %u pixels\n", metadata->width);
    printf("Height: %u pixels\n", metadata->height);
    printf("Bit Depth: %u bits\n", metadata->bitDepth);
    printf("Compression: %u\n", metadata->compression);
}

ImageBMP *BMPM_loadImage(const char *filePath) {
    FILE *imgFile = fopen(filePath, "rb");
    ImageBMP *loadingImage = NULL;
    if (!imgFile) {
        printf("Error opening file: %s\n", filePath);
        return NULL;
    }
    loadingImage = (ImageBMP *)malloc(sizeof(ImageBMP));
    if (!loadingImage) {
        printf("Memory allocation failed for ImageBMP structure.\n");
        fclose(imgFile);
        return NULL;
    }
    loadingImage->angle = 0;

    fread(&loadingImage->metadata, sizeof(BMPHeader), 1, imgFile);
    _testHeader(&loadingImage->metadata);

    if (loadingImage->metadata.signature[0] != 'B' || loadingImage->metadata.signature[1] != 'M') {
        printf("Not a valid BMP file: %s\n", filePath);
        fclose(imgFile);
        free(loadingImage);
        return NULL;
    }

    if (loadingImage->metadata.bitDepth != 24 && loadingImage->metadata.bitDepth != 32) {
        printf("Unsupported BMP bit depth: %u\n", loadingImage->metadata.bitDepth);
        fclose(imgFile);
        free(loadingImage);
        return NULL;
    }

    int pitch = calculatePitch(&loadingImage->metadata);
    size_t pixelArraySize = pitch * loadingImage->metadata.height;

    loadingImage->pixels = (PixelBGRA *)malloc(pixelArraySize);
    if (!loadingImage->pixels) {
        fprintf(stderr, "Error: could not allocate pixel buffer\n");
        fclose(imgFile);
        free(loadingImage);
        return NULL;
    }

    fseek(imgFile, loadingImage->metadata.dataOffset, SEEK_SET);
    fread(loadingImage->pixels, pixelArraySize, 1, imgFile);

    fclose(imgFile);
    return loadingImage;
}

void BMPM_saveImage(const char *fileName, ImageBMP *image) 
{
    FILE *outputFile = fopen(fileName, "wb");
    if (!outputFile) {
        fprintf(stderr, "Error: could not open file for writing: %s\n", fileName);
        return;
    }
    if (!image || !image->pixels) {
        fprintf(stderr, "Error: invalid image data\n");
        fclose(outputFile);
        return;
    }

    fwrite(&image->metadata, sizeof(BMPHeader), 1, outputFile);

    size_t pixelArraySize = calculatePitch(&image->metadata) * image->metadata.height;
    fwrite(image->pixels, pixelArraySize, 1, outputFile);

    fclose(outputFile);
}

void BMPM_freeImage(ImageBMP *image) {
    if (image) {
        if (image->pixels) {
            free(image->pixels);
        }
        free(image);
    }
}





