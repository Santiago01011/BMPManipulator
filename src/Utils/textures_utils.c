#include "textures_utils.h"

extern BMPMetadata metadata;
extern unsigned char* originalImage;
extern SDL_Texture *editing_image;
extern SDL_Renderer *renderer;

int chargeTexture(const char *imgPath) {
//    currentAnimationState = ANIMATION_STATE_NONE;
//    finalAngleAfterRotation = 0;
    if (!loadImageBMPM(imgPath, &metadata, &originalImage)) {

        fprintf(stderr, "Error: could not load image: %s\n", SDL_GetError());
        return 0;
    }

    switch(metadata.bitsPerPixel){ // Handle all supported cases with SDL_PIXELFORMAT
        case 24:
            editing_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, metadata.width, metadata.height);
            break;
        case 32:
            editing_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, metadata.width, metadata.height);
            break;

    }
    if (!editing_image) {
        //fprintf(stderr, "Error: could not create texture from image: %s\n", SDL_GetError());
        free(originalImage);
        return 0;
    }
    if (SDL_UpdateTexture(editing_image, NULL, originalImage, calculatePitch(&metadata)) != 0) {
        //fprintf(stderr, "Error: could not update texture: %s\n", SDL_GetError());
        free(originalImage);
        SDL_DestroyTexture(editing_image);
        return 0;
    }
    SDL_SetTextureBlendMode(editing_image, SDL_BLENDMODE_BLEND);
    return 1;
}


