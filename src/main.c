#define CLAY_IMPLEMENTATION
#include "UI/clay.h"
#include "UI/clay_renderer_SDL2.c"

#include "BMPManipulator/funciones_estudiante.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#define BOX_PADD 10
#define BOX_H 40


const int FONT_ID_BODY_16 = 0;
const Clay_Color COLOR_WHITE = { 255, 255, 255, 255};
SDL_Surface *sample_image;
SDL_Window *window = NULL;

void RenderHeaderButton(Clay_String text) {
    CLAY(
        CLAY_LAYOUT({ .padding = { 12, 12, 2, 2 }}),
        CLAY_RECTANGLE({
            .color = { 140, 140, 140, 255 },
            .cornerRadius = 5
        })
    ) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void RenderHeader() {
    CLAY(
        CLAY_ID("HeaderBar"),
        CLAY_RECTANGLE({ .color = { 100, 41, 51, 255 } }), // Dark background
        CLAY_LAYOUT({
            .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) },
            .padding = { BOX_PADD },
            .childGap = BOX_PADD,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        })
    ) {
        RenderHeaderButton(CLAY_STRING("File"));
        RenderHeaderButton(CLAY_STRING("Edit"));
    }
}

void RenderCentralPanel() {
    int mainWwidth, mainWheight, scaled_width, scaled_height;
    SDL_GetWindowSize(window, &mainWwidth, &mainWheight);
    CLAY(
        CLAY_ID("CentralPanel"),
        CLAY_RECTANGLE({ .color = { 0, 0, 0, 0 } }),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { BOX_PADD },
            .childGap = BOX_PADD,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        })
    ) {
        if (sample_image) {
            int img_width = sample_image->w;
            int img_height = sample_image->h;

            // Get the available space in the parent container
            int available_width = mainWwidth - (BOX_PADD * 2);
            int available_height = mainWheight - (BOX_PADD * 4) - (BOX_H * 2);

            // Calculate the scaling factor to fit the image within the available space
            float scale_width = (float)available_width / (float)img_width;
            float scale_height = (float)available_height / (float)img_height;
            float scale = (scale_width < scale_height) ? scale_width : scale_height;

            // Calculate the scaled dimensions
            if(img_height > available_height || img_width > available_width){
                scaled_width = (int)(img_width * scale);
                scaled_height = (int)(img_height * scale);
            } else {
                scaled_width = img_width;
                scaled_height = img_height;
            }
            // Render the scaled image
            CLAY(
                CLAY_LAYOUT({
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(scaled_width),
                        .height = CLAY_SIZING_FIXED(scaled_height)
                    }
                }),
                CLAY_IMAGE({ sample_image, scaled_width, scaled_height })
            ) {}
        } else {
            printf("No image to render\n");
        }
    }
}



void RenderEditButtons() {
    CLAY(
        CLAY_ID("EditButtons"),
        CLAY_RECTANGLE({ .color = { 100, 41, 51, 255 } }),
        CLAY_LAYOUT({
            .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) },
            .padding = { BOX_PADD },
            .childGap = BOX_PADD,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        })
    ) {
        RenderHeaderButton(CLAY_STRING("Cut"));
        RenderHeaderButton(CLAY_STRING("Rotate"));
        RenderHeaderButton(CLAY_STRING("Change RGB"));
        RenderHeaderButton(CLAY_STRING("Scale"));
        RenderHeaderButton(CLAY_STRING("B&W"));
    }
}

static Clay_RenderCommandArray CreateLayout() {
    Clay_BeginLayout();
    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_RECTANGLE({ .color = { 43, 41, 51, 255 } }), // Dark background
        CLAY_LAYOUT({
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(10),
            .childGap = 10
        })
    ) {
        // Header
        RenderHeader();

        // Image Display
        RenderCentralPanel();

        // Edit Buttons
        RenderEditButtons();
    }

    return Clay_EndLayout();
}

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
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) < 0) {
        fprintf(stderr, "Error: could not initialize IMG: %s\n", IMG_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("resources/Roboto-Regular.ttf", 16);
    if (!font) {
        fprintf(stderr, "Error: could not load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL2_Font fonts[1] = {};

    fonts[FONT_ID_BODY_16] = (SDL2_Font) {
        .fontId = FONT_ID_BODY_16,
        .font = font,
    };

    sample_image = IMG_Load("resources/test_org.bmp");
    if (!sample_image) {
        fprintf(stderr, "Error: could not load image: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Renderer *renderer = NULL;
    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0) {
        fprintf(stderr, "Error: could not create window and renderer: %s", SDL_GetError());
    }

    uint64_t totalMemorySize = Clay_MinMemorySize();
    void *memoryBuffer = malloc(totalMemorySize);
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, memoryBuffer);

    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)windowWidth, (float)windowHeight }, (Clay_ErrorHandler) { HandleClayErrors });

    Clay_SetMeasureTextFunction(SDL2_MeasureText, (uintptr_t)&fonts);

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    while (true) {
        Clay_Vector2 scrollDelta = {};
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: { goto quit; }
                case SDL_MOUSEWHEEL: {
                    scrollDelta.x = event.wheel.x;
                    scrollDelta.y = event.wheel.y;
                    break;
                }
                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        int newWidth = event.window.data1;
                        int newHeight = event.window.data2;
                        Clay_SetLayoutDimensions((Clay_Dimensions) { (float)newWidth, (float)newHeight });
                    }
                    break;
                }
            }
        }
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        int mouseX = 0;
        int mouseY = 0;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        Clay_Vector2 mousePosition = { (float)mouseX, (float)mouseY };
        Clay_SetPointerState(mousePosition, mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));

        Clay_UpdateScrollContainers(
            true,
            (Clay_Vector2) { scrollDelta.x, scrollDelta.y },
            deltaTime
        );

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        Clay_SetLayoutDimensions((Clay_Dimensions) { (float)windowWidth, (float)windowHeight });

        Clay_RenderCommandArray renderCommands = CreateLayout();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        Clay_SDL2_Render(renderer, renderCommands, fonts);

        SDL_RenderPresent(renderer);

        // Add a small delay to prevent the loop from consuming 100% CPU
        SDL_Delay(16); // ~60 FPS
    }

quit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (sample_image) {
        SDL_FreeSurface(sample_image);
    }
    if (memoryBuffer) {
        free(memoryBuffer);
    }
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
