#include "bmp_functions.h"


int calculatePitch(BMPHeader* metadata) {
    int bytesPerPixel = metadata->bitDepth / 8;
    return ((metadata->width * bytesPerPixel + 3) / 4) * 4;
}

void _testHeader(BMPHeader* metadata) {
    printf("Signature: %c%c\n", metadata->signature[0], metadata->signature[1]);
    printf("File Size: %u bytes\n", metadata->fileSize);
    printf("Data Offset: %u bytes\n", metadata->dataOffset);
    printf("Header Size: %u bytes\n", metadata->headerSize);
    printf("Width: %u pixels\n", metadata->width);
    printf("Height: %u pixels\n", metadata->height);
    printf("Bit Depth: %u bits\n", metadata->bitDepth);
    printf("Compression: %u\n", metadata->compression);
}

ImageBMP loadImageBMPM(const char* filePath) {
    FILE *imgFile = fopen(filePath, "rb");
    ImageBMP loadingImage;
    loadingImage.pixels = NULL;
    if (!imgFile) {
        printf("Error opening file: %s\n", filePath);
        return;
    }

    fread(&loadingImage.metadata, sizeof(BMPHeader), 1, imgFile);
    _testHeader(&loadingImage.metadata);

    if (loadingImage.metadata.signature[0] != 'B' || loadingImage.metadata.signature[1] != 'M') {
        printf("Not a valid BMP file: %s\n", filePath);
        fclose(imgFile);
        return;
    }

    if (loadingImage.metadata.bitDepth != 24 && loadingImage.metadata.bitDepth != 32) {
        printf("Unsupported BMP bit depth: %u\n", loadingImage.metadata.bitDepth);
        fclose(imgFile);
        return;
    }

    if (loadingImage.metadata.compression != 0) {
        printf("Unsupported BMP compression: %u\n", loadingImage.metadata.compression);
        fclose(imgFile);
        return loadingImage;
    }

    loadingImage.pixels = (PixelRGB *)malloc(loadingImage.metadata.width * loadingImage.metadata.height * sizeof(PixelRGB));
    if (!loadingImage.pixels) {
        printf("Memory allocation failed for pixels.\n");
        fclose(imgFile);
        return;
    }
    int paddedRowSize = (int)(4 * ceil((float)(loadingImage.metadata.width * (loadingImage.metadata.bitDepth / 8)) / 4.0f));
    fseek(imgFile, loadingImage.metadata.dataOffset, SEEK_SET);
    for (int i = 0; i < loadingImage.metadata.height; i++) {
        fread(&loadingImage.pixels[i * loadingImage.metadata.width], sizeof(PixelRGB), loadingImage.metadata.width, imgFile);
        fseek(imgFile, paddedRowSize - (loadingImage.metadata.width * sizeof(PixelRGB)), SEEK_CUR);
    }
    fclose(imgFile);
    return loadingImage;
}

void WriteImageBMPM(const char *fileName, PixelRGB *pixels, BMPHeader imgHeader)
{
    int i;
    FILE *outputFile = fopen(fileName, "wb");
    int paddedRowSize = (int)(4 * ceil((float)(imgHeader.width) / 4.0f)) * (imgHeader.bitDepth / 8);
    fwrite(&imgHeader, sizeof(BMPHeader), 1, outputFile);
    fseek(outputFile, imgHeader.dataOffset, SEEK_SET);

    char *paddingBytes = (char *)calloc(paddedRowSize, 1);
    for (i = 0; i < imgHeader.height; i++)
    {
        memcpy(paddingBytes, &pixels[/*pixelOffset*/ (i * imgHeader.width)], imgHeader.height * sizeof(PixelRGB));
        fwrite(paddingBytes, 1, paddedRowSize, outputFile);
    }
    free(paddingBytes);
    fclose(outputFile);
}





