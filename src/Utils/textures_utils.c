#include "textures_utils.h"

extern SDL_Texture *editing_image;
extern SDL_Renderer *renderer;
extern ImageBMP *originalImageBMP;

int chargeTexture(const char *imgPath) {
    if (originalImageBMP) {
        if (originalImageBMP->pixels) free(originalImageBMP->pixels);
        free(originalImageBMP);
        originalImageBMP = NULL;
    }
    originalImageBMP = BMPM_loadImage(imgPath);
    if (!originalImageBMP || !originalImageBMP->pixels) {
        fprintf(stderr, "Error: could not load image: %s\n", imgPath);
        return 0;
    }
    switch(originalImageBMP->metadata.bitDepth){
        case 24:
            editing_image = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, originalImageBMP->metadata.width, originalImageBMP->metadata.height
            );
            break;
        case 32:
            editing_image = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, originalImageBMP->metadata.width, originalImageBMP->metadata.height
            );
            break;
        default:
            fprintf(stderr, "Unsupported bit depth for texture: %u\n", originalImageBMP->metadata.bitDepth);
            return 0;
    }
    if (!editing_image) {
        fprintf(stderr, "Error: could not create texture from image: %s\n", SDL_GetError());
        free(originalImageBMP->pixels);
        free(originalImageBMP);
        originalImageBMP = NULL;
        return 0;
    }
    if (SDL_UpdateTexture(editing_image, NULL, originalImageBMP->pixels, calculatePitch(&originalImageBMP->metadata)) != 0) {
        fprintf(stderr, "Error: could not update texture: %s\n", SDL_GetError());
        free(originalImageBMP->pixels);
        free(originalImageBMP);
        originalImageBMP = NULL;
        SDL_DestroyTexture(editing_image);
        return 0;
    }
    if (SDL_SetTextureBlendMode(editing_image, SDL_BLENDMODE_BLEND) != 0) {
        fprintf(stderr, "Error: could not set texture blend mode: %s\n", SDL_GetError());
        free(originalImageBMP->pixels);
        free(originalImageBMP);
        originalImageBMP = NULL;
        SDL_DestroyTexture(editing_image);
        return 0;
    }
    return 1;
}


