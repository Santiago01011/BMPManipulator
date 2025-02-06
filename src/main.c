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


#include "UI/ui.c"
#define BOX_PADD 20
#define BOX_H 40


const int FontIdBody16 = 0;
const Clay_Color ColorWhite = { 255, 255, 255, 255};
const Clay_Color FullBackgroundColor = (Clay_Color){39, 54, 61 ,255}; //rgb(39, 54, 61)
const Clay_Color BackgroundColor = (Clay_Color){47, 62, 70 ,255}; //rgb(47, 62, 70)
const Clay_Color ButtonColor = (Clay_Color){53, 79, 82, 255}; //rgb(53, 79, 82)
const Clay_Color ButtonHoverColor = (Clay_Color){82, 121, 111, 100}; // rgb(82, 121, 111)

typedef enum {
    ACTION_CUT,
    ACTION_ROTATE,
    ACTION_CHANGE_RGB,
    ACTION_SCALE,
    ACTION_GRAYSCALE,
    ACTION_NONE
} ButtonAction;

unsigned char* originalImage = NULL;
BMPMetadata metadata;
SDL_Surface *sample_image;
SDL_Window *window = NULL;

void HandleButtonClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        ButtonAction action = (ButtonAction)userData;

        switch (action) {
            case ACTION_CUT:
                printf("Cut function called!\n");
                // Call cut function here
                break;

            case ACTION_ROTATE:
                printf("Rotate function called!\n");
                // Call rotation function here
                break;

            case ACTION_CHANGE_RGB:
                printf("Change RGB function called!\n");
                // Call RGB adjustment function here
                break;

            case ACTION_SCALE:
                printf("Scale function called!\n");
                // Call scaling function here
                break;

            case ACTION_GRAYSCALE:
                printf("Applying grayscale...\n");
                // Call gray scaling function here
                grayScaleBMPM(&metadata, originalImage);
                break;

            default:
                printf("Unknown action\n");
                break;
        }
    }
}

void RenderButton(Clay_String text, ButtonAction action) {

    CLAY({
        .layout = {
            .padding = {BOX_PADD, BOX_PADD, 0, 0},
            .sizing = {.height = 25},
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = Clay_Hovered() ? ButtonHoverColor : ButtonColor
    }) {
        Clay_OnHover(HandleButtonClick, (intptr_t)action); // Attach event handler
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FontIdBody16,
            .fontSize = 12,
            .textColor = ColorWhite
        }));
    }
}

void RenderDropdownMenuItem(Clay_String text) {
    CLAY({.layout = { .padding = CLAY_PADDING_ALL(BOX_PADD)}}) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FontIdBody16,
            .fontSize = BOX_PADD,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void RenderHeader() {
    CLAY({
        .id = CLAY_ID("HeaderBar"),
        .layout = {
            .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(BOX_PADD),
            .childGap = BOX_PADD,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = BackgroundColor

    }) {
        RenderButton(CLAY_STRING("File"), ACTION_NONE);
        RenderButton(CLAY_STRING("Edit"), ACTION_NONE);
    }

}

void RenderCentralPanel() {
    int mainWwidth, mainWheight, scaled_width, scaled_height;
    SDL_GetWindowSize(window, &mainWwidth, &mainWheight);

    CLAY({ //new implementation of Clay
        .id = CLAY_ID("CentralPanel"),
        .backgroundColor = BackgroundColor,
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(0),
            .childGap = 0,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        }

    }) {
        if (sample_image) {
            int img_width = sample_image->w;
            int img_height = sample_image->h;

            // Get the available space in the parent container
            int available_width = mainWwidth - BOX_PADD;
            int available_height = mainWheight - (BOX_PADD * 2) - (BOX_H * 2);

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
            CLAY({
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(scaled_width),
                        .height = CLAY_SIZING_FIXED(scaled_height)
                    }
                },
                .image = {
                    .sourceDimensions = {scaled_width, scaled_height},
                    .imageData = sample_image,
                }
            }) {}
        } else {
            printf("No image to render\n");
        }
    }
}



void RenderEditButtons() {
    CLAY({
         .id = CLAY_ID("EditsButtons"),
         .backgroundColor = BackgroundColor,
         .layout = {
            .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(BOX_PADD),
            .childGap = BOX_PADD,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
         },
            .cornerRadius = {8.0f, 8.0f, 8.0f, 8.0f} //this don't work
    }) {
        RenderButton(CLAY_STRING("Cut"), ACTION_CUT);
        RenderButton(CLAY_STRING("Rotate"), ACTION_ROTATE);
        RenderButton(CLAY_STRING("Change RGB"), ACTION_CHANGE_RGB);
        RenderButton(CLAY_STRING("Scale"), ACTION_SCALE);
        RenderButton(CLAY_STRING("GrayScale"), ACTION_GRAYSCALE);
    }
}

static Clay_RenderCommandArray CreateLayout() {
    Clay_BeginLayout();
    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };
    CLAY({
         .id = CLAY_ID("OuterContainer"),
         .backgroundColor = FullBackgroundColor,
         .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = {5, 5, 5, 5},
            .childGap = 5
         }
    }) {
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

    fonts[FontIdBody16] = (SDL2_Font) {
        .fontId = FontIdBody16,
        .font = font,
    };
    if(loadImageBMPM("resources/miku.bmp", &metadata, &originalImage)){
        //printf("Image width: %d, image height: %d\n", metadata.width, metadata.height);
        int pitch = ((metadata.width * 3 + 3) / 4) * 4;
        printf("Pitch: %d\n", pitch);
        sample_image = SDL_CreateRGBSurfaceFrom(
            originalImage,
            metadata.width,
            metadata.height,
            metadata.bitsPerPixel,
            pitch,
            0xFF0000,  // Red mask
            0x00FF00,  // Green mask
            0x0000FF,  // Blue mask
            0           // Alpha mask
        );
        if(!sample_image){
            fprintf(stderr, "Error: could not create surface from image: %s\n", SDL_GetError());
            free(originalImage);
            return 1;
        }
    } else {
        system("pause");
    }
    //sample_image = IMG_Load("resources/test_org.bmp");

    SDL_Renderer *renderer = NULL;
    if (SDL_CreateWindowAndRenderer(600, 400, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0) {
        fprintf(stderr, "Error: could not create window and renderer: %s", SDL_GetError());
    }

    uint64_t totalMemorySize = Clay_MinMemorySize();
    void *memoryBuffer = malloc(totalMemorySize);
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, memoryBuffer);

    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)windowWidth, (float)windowHeight }, (Clay_ErrorHandler) { HandleClayErrors });

    Clay_SetMeasureTextFunction(SDL2_MeasureText, &fonts);

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

        // Add a small delay to prevent the loop from consuming 100% CPU - 16 -> almost 60 fps
        SDL_Delay(16);  //future test, try to use this to modify the consuming of the cpu
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
    if(originalImage) {
        free(originalImage);
    }
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

