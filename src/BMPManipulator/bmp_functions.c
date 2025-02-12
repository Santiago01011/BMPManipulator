#include "bmp_functions.h"


/*

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


int loadImageBMPM(const char* filePath, BMPMetadata* metadata, unsigned char** pixels) {
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
    metadata->angle = 0;

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
        }

        // Calculate row size and allocate memory
        const int rowSize = calculatePitch(metadata);
        const int expectedSize = rowSize * abs(metadata->height);


        *pixels = (unsigned char*)malloc(expectedSize);
        if(!(*pixels)){
            printf("memory not alocated");
            return 0;
        }
        fseek(imgFile, metadata->dataOffset, SEEK_SET);

        if (fread(*pixels, expectedSize, 1, imgFile) != 1) {
            puts("Error reading pixel data");
            free(*pixels);
            fclose(imgFile);
            return 0;
        }
//        // **BGR to RGB byte swap for 24-bit BMP**
//        if (metadata->bitsPerPixel == 24) {
//            unsigned char *pixels = *pixels;
//            for (int i = 0; i < metadata->width * metadata->height; ++i) {
//                unsigned char temp = pixels[i * 3];       // Blue
//                pixels[i * 3] = pixels[i * 3 + 2];    // Blue becomes Red
//                pixels[i * 3 + 2] = temp;               // Red becomes Blue
//            }
//        }

        fclose(imgFile);
        return 1;
    }

    printf("Unsupported BMP format: %s\n", filePath);
    fclose(imgFile);
    return 0;
}

int saveImageBMPM(const char* dirPath, BMPMetadata *metadata, unsigned char* pixels) {
    FILE *bmpFile = NULL;

    if (dirPath == NULL || metadata == NULL || pixels == NULL) {
        fprintf(stderr, "Error: Invalid input parameters to saveImageBMPM.\n");
        return 0; // Indicate failure due to invalid input
    }

    bmpFile = fopen(dirPath, "wb");
    if (bmpFile == NULL) {
        perror("Error opening file for writing");
        return 0; // Indicate file open error
    }

    if (metadata->bitsPerPixel != 24) { // Still supporting only 24-bit for now
        fprintf(stderr, "Error: saveImageBMPM currently only supports 24-bit BMP.\n");
        fclose(bmpFile);
        return 0;
    }

    // --- Calculate imageSize and fileSize (based on metadata->width, height, bitsPerPixel) ---
    int bytesPerPixel = metadata->bitsPerPixel / 8;
    int rowPadding = (4 - (metadata->width * bytesPerPixel) % 4) % 4;
    metadata->imageSize = (metadata->width * bytesPerPixel + rowPadding) * metadata->height;
    metadata->fileSize = metadata->dataOffset + metadata->imageSize;


    // --- Write BMP File Header (14 bytes) from the metadata struct ---
    fwrite(metadata->signature, sizeof(char), 2, bmpFile);      // Signature "BM"
    fwrite(&metadata->fileSize, sizeof(uint32_t), 1, bmpFile);    // FileSize
    fwrite(&metadata->reserved, sizeof(uint32_t), 1, bmpFile);    // Reserved
    fwrite(&metadata->dataOffset, sizeof(uint32_t), 1, bmpFile);  // DataOffset

    // --- Write DIB Header (40 bytes) from the metadata struct ---
    fwrite(&metadata->headerSize, sizeof(uint32_t), 1, bmpFile);   // HeaderSize
    fwrite(&metadata->width, sizeof(int32_t), 1, bmpFile);      // Width
    fwrite(&metadata->height, sizeof(int32_t), 1, bmpFile);     // Height
    fwrite(&metadata->planes, sizeof(uint16_t), 1, bmpFile);     // Planes
    fwrite(&metadata->bitsPerPixel, sizeof(uint16_t), 1, bmpFile); // BitsPerPixel
    fwrite(&metadata->compression, sizeof(uint32_t), 1, bmpFile);  // Compression
    fwrite(&metadata->imageSize, sizeof(uint32_t), 1, bmpFile);   // ImageSize
    fwrite(&metadata->xPixelsPerM, sizeof(int32_t), 1, bmpFile);   // xPixelsPerM
    fwrite(&metadata->yPixelsPerM, sizeof(int32_t), 1, bmpFile);   // yPixelsPerM
    fwrite(&metadata->colorsUsed, sizeof(uint32_t), 1, bmpFile);  // colorsUsed
    fwrite(&metadata->importantColors, sizeof(uint32_t), 1, bmpFile);// importantColors

    // --- Pixel Data (same as before) ---
    unsigned char padding[3] = {0, 0, 0};
    bytesPerPixel = metadata->bitsPerPixel / 8;
    rowPadding = (4 - (metadata->width * bytesPerPixel) % 4) % 4;

    for (int y = metadata->height; y > 0 ; --y) {
        unsigned char *rowStart = pixels + (metadata->height - 1 - y) * metadata->width * bytesPerPixel;
        fwrite(rowStart, bytesPerPixel * metadata->width, 1, bmpFile);
        fwrite(padding, rowPadding, 1, bmpFile);
    }

    fclose(bmpFile);
    printf("BMP image saved successfully to: %s\n", dirPath);
    return 1;
}

int grayScaleBMPM(BMPMetadata * metadata, unsigned char* pixels){
    for(int i = 0; i < metadata->height * metadata ->width; i++){
        unsigned char *pixel = &pixels[i * (metadata->bitsPerPixel / 8)];

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

int changeRGBvalues(BMPMetadata * metadata, unsigned char* pixels, float r, float g, float b){
    for(int i = 0; i < metadata->height * metadata->width; i++){
        unsigned char *pixel = &pixels[i * (metadata->bitsPerPixel / 8)];
        pixel[0] = (pixel[0] * b) < 255 ? pixel[0] * b : 255; //Blue
        pixel[1] = (pixel[1] * g) < 255 ? pixel[1] * g : 255; //Green
        pixel[2] = (pixel[2] * r) < 255 ? pixel[2] * r : 255; //Red
    }
    return 1;
}

int rotateBMPM(BMPMetadata *metadata, unsigned char** pixels, const char dir) {
    int srcIndex, destIndex, x, y;
    int bytesPerPixel = metadata->bitsPerPixel / 8;

    // Save previous row size before swap
    int32_t oldRowSize = calculatePitch(metadata);

    // Swap width and height to rotate
    int32_t old_w = metadata->width;
    int32_t old_h = metadata->height;

    int32_t newRowSize = calculatePitch(metadata);



    // Calculate new image size
    size_t expectedSize = newRowSize * abs(old_w);

    if (*pixels == NULL) {
        puts("Error: pixels is NULL!");
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
        for (y = 0; y < old_h; y++) {
            for (x = 0; x < old_w; x++) {
                // Source pixel in original image
                srcIndex = y * oldRowSize + x * bytesPerPixel;
                if(dir == 'l') destIndex = x * newRowSize + (old_h - y - 1) * bytesPerPixel;
                if(dir == 'r') destIndex = (old_w - x - 1) * newRowSize + y * bytesPerPixel;
                // Copy pixel data
                memcpy(&tempImage[destIndex], &(*pixels)[srcIndex], bytesPerPixel);
            }
        }
    } else {
        printf("dir argument is not a valid value");
        return 0;
    }

    // Free old image and assign new one
    free(*pixels);
    *pixels = tempImage;
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






