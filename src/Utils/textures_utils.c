#include "textures_utils.h"

extern SDL_Texture *editing_image;
extern SDL_Renderer *renderer;
extern ImageBMP *originalImageBMP;

int chargeTexture(const char *imgPath) {
    SDL_Texture *newTexture = NULL;
    ImageBMP *newImageBMP = BMPM_loadImage(imgPath);
    if (!newImageBMP || !newImageBMP->pixels) {
        fprintf(stderr, "Error: could not load image: %s\n", imgPath);
        if (newImageBMP)
            BMPM_freeImage(newImageBMP);
        return 0;
    }

    switch(newImageBMP->metadata.bitDepth) {
        case 24:
            newTexture = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, newImageBMP->metadata.width, newImageBMP->metadata.height
            );
            break;
        case 32:
            newTexture = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, newImageBMP->metadata.width, newImageBMP->metadata.height
            );
            break;
        default:
            fprintf(stderr, "Unsupported bit depth for texture: %u\n", newImageBMP->metadata.bitDepth);
            BMPM_freeImage(newImageBMP);
            return 0;
    }

    if (!newTexture) {
        fprintf(stderr, "Error: could not create texture from image: %s\n", SDL_GetError());
        BMPM_freeImage(newImageBMP);
        return 0;
    }

    if (SDL_UpdateTexture(newTexture, NULL, newImageBMP->pixels, calculatePitch(&newImageBMP->metadata)) != 0) {
        fprintf(stderr, "Error: could not update texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(newTexture);
        BMPM_freeImage(newImageBMP);
        return 0;
    }

    if (SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND) != 0) {
        fprintf(stderr, "Error: could not set texture blend mode: %s\n", SDL_GetError());
        SDL_DestroyTexture(newTexture);
        BMPM_freeImage(newImageBMP);
        return 0;
    }

    if (originalImageBMP)
        BMPM_freeImage(originalImageBMP);
    originalImageBMP = newImageBMP;

    if (editing_image)
        SDL_DestroyTexture(editing_image);
    editing_image = newTexture;

    return 1;
}


