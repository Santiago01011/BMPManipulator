#define CLAY_IMPLEMENTATION
#include "UI/clay.h"
#include "UI/clay_renderer_SDL2.c"

#include "BMPManipulator/bmp_functions.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#include "UI/clay_elements.h"
#include "Utils/textures_utils.h"



const int FontIdBody16 = 0;

unsigned char* originalImage = NULL;
unsigned char* backUpImage = NULL;
SDL_Texture *editing_image = NULL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
BMPMetadata metadata;
short finalAngleAfterRotation;
AnimationState currentAnimationState = ANIMATION_STATE_NONE;


void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error: could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() < 0) {
        fprintf(stderr, "Error: could not initialize TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) < 0) {
        fprintf(stderr, "Error: could not initialize IMG: %s\n", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    TTF_Font *font = TTF_OpenFont("resources/Roboto-Regular.ttf", 16);
    if (!font) {
        fprintf(stderr, "Error: could not load font: %s\n", TTF_GetError());
        return 1;
    }
    SDL2_Font fonts[1] = {0};
    fonts[FontIdBody16] = (SDL2_Font){ .fontId = FontIdBody16, .font = font };

    if (SDL_CreateWindowAndRenderer(600, 400, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0) {
        fprintf(stderr, "Error: could not create window and renderer: %s\n", SDL_GetError());
        return 1;
    }

    // Reserve memory for Clay
    uint64_t totalMemorySize = Clay_MinMemorySize();
    void *memoryBuffer = malloc(totalMemorySize);

    // Load an default image
    //if (!chargeTexture("resources/noImage.bmp")) {
        //goto quit;
    //}
    chargeTexture("resources/test_666.bmp");

    // Initialize Clay with current window dimensions
    int windowWidth = 0, windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, memoryBuffer);
    Clay_Initialize(clayMemory, (Clay_Dimensions){ (float)windowWidth, (float)windowHeight }, (Clay_ErrorHandler){ HandleClayErrors });
    Clay_SetMeasureTextFunction(SDL2_MeasureText, &fonts);

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = NOW;
    double deltaTime = 0;

    while (true) {
        Clay_Vector2 scrollDelta = {0};
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                goto quit;
            if (event.type == SDL_MOUSEWHEEL) {
                scrollDelta.x = event.wheel.x;
                scrollDelta.y = event.wheel.y;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                Clay_SetLayoutDimensions((Clay_Dimensions){ (float)event.window.data1, (float)event.window.data2 });
            }
        }
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        int mouseX = 0, mouseY = 0;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        Clay_SetPointerState((Clay_Vector2){ (float)mouseX, (float)mouseY }, mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

        Clay_UpdateScrollContainers(true, (Clay_Vector2){ scrollDelta.x, scrollDelta.y }, deltaTime);

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        Clay_SetLayoutDimensions((Clay_Dimensions){ (float)windowWidth, (float)windowHeight });

        // Build layout
        Clay_RenderCommandArray renderCommands = CreateLayout();

        /*this is for animation control*/
        switch(currentAnimationState){
            case ANIMATION_STATE_ROTATING_RIGHT:
                if(finalAngleAfterRotation > 0){
                    metadata.angle += 5;
                    finalAngleAfterRotation -=5;
                    if(metadata.angle >= 360)
                        metadata.angle -= 0;
                } else {
                    currentAnimationState = ANIMATION_STATE_NONE;
                }
                break;
            case ANIMATION_STATE_ROTATING_LEFT:
                if(finalAngleAfterRotation < 0){
                    metadata.angle -= 5;
                    finalAngleAfterRotation += 5;
                    if(metadata.angle <= -360)
                        metadata.angle += 0;
                } else {
                    currentAnimationState = ANIMATION_STATE_NONE;
                }
                break;
            default:
                break;
        }

        SDL_SetRenderDrawColor(renderer, 8, 8, 8, 8);
        SDL_RenderClear(renderer);
        Clay_SDL2_Render(renderer, renderCommands, fonts);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
quit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (originalImage)
        free(originalImage);
    if (memoryBuffer)
        free(memoryBuffer);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

