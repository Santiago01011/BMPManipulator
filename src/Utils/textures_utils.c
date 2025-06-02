#include "textures_utils.h"

extern SDL_Texture *editing_image;
extern SDL_Renderer *renderer;
extern ImageBMP originalImageBMP;

int chargeTexture(const char *imgPath) {
    originalImageBMP = loadImageBMPM(imgPath);
    if (!originalImageBMP.pixels) {
        fprintf(stderr, "Error: could not load image: %s\n", imgPath);
        return 0;
    }
    printf("Renderer: %p\n", (void*)renderer);
    printf("Image size: %ux%u, bitDepth: %u\n", originalImageBMP.metadata.width, originalImageBMP.metadata.height, originalImageBMP.metadata.bitDepth);
    switch(originalImageBMP.metadata.bitDepth){ // Handle all supported cases with SDL_PIXELFORMAT
        case 24:
            editing_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, originalImageBMP.metadata.width, originalImageBMP.metadata.height);
            break;
        case 32:
            editing_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, originalImageBMP.metadata.width, originalImageBMP.metadata.height);
            break;
    }
    if (!editing_image) {
        puts("test");
        fprintf(stderr, "Error: could not create texture from image: %s\n", SDL_GetError());
        free(originalImageBMP.pixels);
        return 0;
    }
    if (SDL_UpdateTexture(editing_image, NULL, (void*)originalImageBMP.pixels, calculatePitch(&originalImageBMP.metadata)) != 0) {
        fprintf(stderr, "Error: could not update texture: %s\n", SDL_GetError());
        free(originalImageBMP.pixels);
        SDL_DestroyTexture(editing_image);
        return 0;
    }
    if (SDL_SetTextureBlendMode(editing_image, SDL_BLENDMODE_BLEND) != 0) {
        fprintf(stderr, "Error: could not set texture blend mode: %s\n", SDL_GetError());
        free(originalImageBMP.pixels);
        SDL_DestroyTexture(editing_image);
        return 0;
    }
    return 1;
}


