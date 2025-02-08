#include "bmp_functions.h"


/*
    // Header check zone
    printf("Image header:\n");
    printf("%s\n", metadata->signature);
    printf("File size: %d\n", metadata->fileSize);
    printf("Reserved: %d\n", metadata->reserved);
    printf("Data offset: %d\n", metadata->dataOffset);
    printf("Header size: %d\n", metadata->headerSize);
    printf("Width: %d\n", metadata->width);
    printf("Height: %d\n", metadata->height);
    printf("Planes: %d\n", metadata->planes);
    printf("Bits per pixel: %d\n", metadata->bitsPerPixel);
    printf("Compression: %d\n", metadata->compression);
    printf("Image size: %d\n", metadata->imageSize);
    printf("X pixels per meter: %d\n", metadata->xPixelsPerM);
    printf("Y pixels per meter: %d\n", metadata->yPixelsPerM);
    printf("Colors used: %d\n", metadata->colorsUsed);
    printf("Important colors: %d\n", metadata->importantColors);
    // Header check ends
    if (metadata->bitsPerPixel != 24 || metadata->compression != 0){
        printf("The file %s is not a supported file (compression)\n", filePath);
        fclose(imgFile);
        return 0;
    }

    // Calculate row size and padding

    Little breakdown of what we�re doing here:
        ( meta->width * 3 ) calculates the total number of bytes required to store the pixel data for each row,
        assuming 3 bytes per pixel (24-bit color).
        ( +3 ) adds 3 to the result, which is equivalent to adding 3 bytes of padding to ensure that the division by 4
        rounds up to the nearest multiple of 4.
        ( /4 ) divides the result by 4, effectively rounding down to the nearest multiple of 4.
        ( *4 ) multiplies the result by 4, which gives the total row size with padding.
*/

int calculatePitch(BMPMetadata* metadata) {
    int bytesPerPixel = metadata->bitsPerPixel / 8;
    return ((metadata->width * bytesPerPixel + 3) / 4) * 4;
}


int loadImageBMPM(const char* filePath, BMPMetadata* metadata, unsigned char** originalImage) {
    FILE *imgFile = fopen(filePath, "rb");
    if (!imgFile) {
        printf("Error opening file: %s\n", filePath);
        return 0;
    }

    unsigned char header[HEADER_SIZE];
    fread(header, HEADER_SIZE, 1, imgFile);

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a BMP file: %s\n", filePath);
        fclose(imgFile);
        return 0;
    }

    // Read only the first 40 bytes (BITMAPINFOHEADER)
    memcpy(metadata->signature, header, 2);
    metadata->fileSize = *(uint32_t*)(header + 2);
    metadata->reserved = *(uint32_t*)(header + 6);
    metadata->dataOffset = *(uint32_t*)(header + 10);
    metadata->headerSize = *(uint32_t*)(header + 14);
    metadata->width = *(int32_t*)(header + 18);
    metadata->height = *(int32_t*)(header + 22);
    metadata->planes = *(uint16_t*)(header + 26);
    metadata->bitsPerPixel = *(uint16_t*)(header + 28);
    metadata->compression = *(uint32_t*)(header + 30);
    metadata->imageSize = *(uint32_t*)(header + 34);
    metadata->xPixelsPerM = *(int32_t*)(header + 38);
    metadata->yPixelsPerM = *(int32_t*)(header + 42);
    metadata->colorsUsed = *(uint32_t*)(header + 46);
    metadata->importantColors = *(uint32_t*)(header + 50);

    // **Handle only supported cases**
    if ((metadata->bitsPerPixel == 24 && metadata->compression == 0) ||
        (metadata->bitsPerPixel == 32 && metadata->compression == 3)) {

        // If the header is larger than 40 bytes, skip extra data
        if (metadata->headerSize > 40) {
            fseek(imgFile, metadata->headerSize - 40, SEEK_CUR);
        }

        // Handle 32-bit images with compression 3 (bit masks)
        uint32_t redMask = 0, greenMask = 0, blueMask = 0, alphaMask = 0;
        if (metadata->bitsPerPixel == 32 && metadata->compression == 3) {
            fread(&redMask, sizeof(uint32_t), 1, imgFile);
            fread(&greenMask, sizeof(uint32_t), 1, imgFile);
            fread(&blueMask, sizeof(uint32_t), 1, imgFile);
            fread(&alphaMask, sizeof(uint32_t), 1, imgFile);
            printf("Bit masks - R: 0x%X, G: 0x%X, B: 0x%X, A: 0x%X\n", redMask, greenMask, blueMask, alphaMask);
        }

        // Calculate row size and allocate memory
        const int rowSize = calculatePitch(metadata);
        const int expectedSize = rowSize * abs(metadata->height);


        *originalImage = (unsigned char*)malloc(expectedSize);
        if(!(*originalImage)){
            printf("memory not alocated");
            return 0;
        }
        fseek(imgFile, metadata->dataOffset, SEEK_SET);

        if (fread(*originalImage, expectedSize, 1, imgFile) != 1) {
            puts("Error reading pixel data");
            free(*originalImage);
            fclose(imgFile);
            return 0;
        }

        fclose(imgFile);
        return 1;
    }

    printf("Unsupported BMP format: %s\n", filePath);
    fclose(imgFile);
    return 0;
}


int grayScaleBMPM(BMPMetadata * metadata, unsigned char* originalImage){
    for(int i = 0; i < metadata->height * metadata ->width; i++){
        unsigned char *pixel = &originalImage[i * (metadata->bitsPerPixel / 8)];

        // Compute grayscale value
        //unsigned char gray = (pixel[0] + pixel[1] + pixel[2]) / 3;

        // Advanced grayscale formula
        unsigned char gray = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);

        // Overwrite pixel with grayscale values
        pixel[0] = gray; // Blue
        pixel[1] = gray; // Green
        pixel[2] = gray; // Red
    }
    return 1;
}

/*int rotateRightBMPM(BMPMetadata *metadata, unsigned char** originalImage) {
    int oldWidth = metadata->width;
    int oldHeight = metadata->height;
    int bytesPerPixel = metadata->bitsPerPixel / 8;

    // Swap width and height
    metadata->width = oldHeight;
    metadata->height = oldWidth;

    // Calculate new image size
    int newRowSize = calculatePitch(metadata);
    int expectedSize = newRowSize * abs(metadata->height);

    printf("Rotate function called!\n");
    printf("Rotating: Old Size: %dx%d, New Size: %dx%d, BytesPerPixel: %d\n",
           oldWidth, oldHeight, metadata->width, metadata->height, bytesPerPixel);
    printf("Expected Image Size: %d\n", expectedSize);

    if (*originalImage == NULL) {
        puts("Error: originalImage is NULL!");
        return 1;
    }

    unsigned char *tempImage = (unsigned char*)malloc(expectedSize);
    if (!tempImage) {
        puts("Error allocating memory for rotateRightBMP");
        return 1;
    }

    memset(tempImage, 0, expectedSize);  // Ensure all memory is initialized

    for (int y = 0; y < oldHeight; y++) {
        for (int x = 0; x < oldWidth; x++) {
            // Source pixel in original image
            int srcIndex = (y * oldWidth + x) * bytesPerPixel;

            // Destination pixel in rotated image
            int destIndex = ((x * oldHeight) + (oldHeight - y - 1)) * bytesPerPixel;

            if (srcIndex >= expectedSize || destIndex >= expectedSize) {
                printf("ERROR: Out-of-bounds access! srcIndex: %d, destIndex: %d\n", srcIndex, destIndex);
                free(tempImage);
                return 1;
            }

            memcpy(&tempImage[destIndex], &(*originalImage)[srcIndex], bytesPerPixel);
        }
    }

    // Free old image and assign new one
    free(*originalImage);
    *originalImage = tempImage;

    return 0;
}*/


int rotateBMPM(BMPMetadata *metadata, unsigned char** originalImage, const char dir) {
    int srcIndex, destIndex, x, y;
    int bytesPerPixel = metadata->bitsPerPixel / 8;

    // Save previous row size before swap
    int32_t oldRowSize = calculatePitch(metadata);

    // Swap width and height to rotate
    int32_t temp = metadata->width;
    metadata->width = metadata->height;
    metadata->height = temp;

    int32_t newRowSize = calculatePitch(metadata);



    // Calculate new image size
    size_t expectedSize = newRowSize * abs(metadata->height);

    if (*originalImage == NULL) {
        puts("Error: originalImage is NULL!");
        return 0;
    }

    // Allocate memory for the rotated image
    unsigned char *tempImage = (unsigned char*)malloc(expectedSize);
    if (!tempImage) {
        puts("Error allocating memory for rotateRightBMP");
        return 0;
    }


    if(dir == 'r' || dir == 'l'){
        // Rotate the image
        for (y = 0; y < metadata->width; y++) {
            for (x = 0; x < metadata->height; x++) {
                // Source pixel in original image
                srcIndex = y * oldRowSize + x * bytesPerPixel;
                if(dir == 'l') destIndex = x * newRowSize + (metadata->width - y - 1) * bytesPerPixel;
                if(dir == 'r') destIndex = (metadata->height - x - 1) * newRowSize + y * bytesPerPixel;
                // Copy pixel data
                memcpy(&tempImage[destIndex], &(*originalImage)[srcIndex], bytesPerPixel);
            }
        }
    } else {
        printf("dir argument is not a valid value");
        return 0;
    }

    // Free old image and assign new one
    free(*originalImage);
    *originalImage = tempImage;

    return 1;
}

/*practice matrix
                      0  1  2  3  4  5  6  7  8
 here we finish---> 0[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    1[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    5[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    6[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    7[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    8[ ][ ][ ][ ][ ][ ][ ][ ][ ]<--- here we start ---> for each pixel [B][G][R]
*/


/* rotate left  (x, y) => (x2 = h - y1 - 1, y2 = x1) = (x2, y2) -> 90° left
        (8,8) -> (0,8) => (x = 9 - 8 - 1 , y = x) = (0,8)
        (7,7) -> (7, 1) => (x = 9 - 7 - 1, y = x) = (1, 7)
        (6,3) => (x2 = 9 - 3 - 1, y2 = 6) = (5, 6)

                      0  1  2  3  4  5  6  7  8
 here we finish---> 0[ ][ ][ ][ ][ ][ ][ ][ ][l]
                    1[ ][ ][ ][ ][ ][ ][ ][l][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    5[ ][ ][ ][ ][ ][ ][l][ ][ ]
                    6[ ][ ][ ][a][ ][ ][ ][ ][ ]
                    7[ ][ ][ ][ ][ ][ ][ ][a][ ]
                    8[ ][ ][ ][ ][ ][ ][ ][ ][a]<--- here we start ---> for each pixel [B][G][R]
*/
/* rotate right (x, y) => (x2 = y1, y2 = h - x1 - 1) = (x2, y2) -> 90° right
        (x1 = 8 , y1 = 8) => (x2 = 8, y2 = 9 - 8 - 1) = (8, 0)
        (x1 = 7 , y1 = 7) => (x2 = 7, y2 = 9 - 7 - 1) = (7, 1)
        (x1 = 6 , y1 = 3) => (x2 = 3, y2 = 9 - 6 - 1) = (3, 2)
        (x1 = 4 , y1 = 5) => (x2 = 5, y2 = 9 - 4 - 1) = (5, 4)
                      0  1  2  3  4  5  6  7  8
 here we finish---> 0[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    1[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    2[ ][ ][ ][ ][ ][ ][ ][ ][ ]
                    3[ ][ ][r][ ][ ][ ][ ][ ][ ]
                    4[ ][ ][ ][ ][ ][a][ ][ ][ ]
                    5[ ][ ][ ][ ][r][ ][ ][ ][ ]
                    6[ ][ ][ ][a][ ][ ][ ][ ][ ]
                    7[ ][r][ ][ ][ ][ ][ ][a][ ]
                    8[r][ ][ ][ ][ ][ ][ ][ ][a]<--- here we start ---> for each pixel [B][G][R]
*/

/* diagonal practice

                    (x,x) -> main diagonal
                    (x,x) -> main diagonal

                       0  1  2  3  4  5  6  7  8
 here we finish---> 0 [a][ ][ ][ ][ ][ ][ ][ ][b]
                    1 [ ][a][ ][ ][ ][ ][ ][b][ ]
                    2 [ ][ ][a][ ][ ][ ][b][ ][ ]
                    3 [ ][ ][ ][a][ ][b][ ][ ][ ]
                    4 [ ][ ][ ][ ][a][ ][ ][ ][ ]
                    5 [ ][ ][ ][b][ ][a][ ][ ][ ]
                    6 [ ][ ][b][ ][ ][ ][a][ ][ ]
                    7 [ ][b][ ][ ][ ][ ][ ][a][ ]
                    8 [b][ ][ ][ ][ ][ ][ ][ ][a]<--- here we start ---> for each pixel [B][G][R]
*/






