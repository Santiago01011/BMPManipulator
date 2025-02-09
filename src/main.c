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

/**Buttons defines**/
#define BOX_PADD 20
#define BOX_H 40
#define BTN_H 30
#define BTN_W 75



const int FontIdBody16 = 0;
const Clay_Color ColorWhite = { 255, 255, 255, 255};
const Clay_Color FullBackgroundColor = (Clay_Color){39, 54, 61 ,255}; //rgb(39, 54, 61)
const Clay_Color BackgroundColor = (Clay_Color){47, 62, 70 ,255}; //rgb(47, 62, 70)
const Clay_Color ButtonColor = (Clay_Color){53, 79, 82, 255}; //rgb(53, 79, 82)
const Clay_Color ButtonHoverColor = (Clay_Color){82, 121, 111, 100}; // rgb(82, 121, 111)

typedef enum {
    ACTION_CUT,
    ACTION_ROTATE,
    ACTION_ROTATE_RIGHT,
    ACTION_ROTATE_LEFT,
    ACTION_CHANGE_RGB,
    ACTION_SCALE,
    ACTION_GRAYSCALE,
    ACTION_OPEN_FILE,
    ACTION_OPEN_EDIT_MENU,
    ACTION_OPEN_FILE_MENU,
    ACTION_NONE
} ButtonAction;

typedef enum {
    ANIMATION_STATE_NONE,      // No animation active
    ANIMATION_STATE_ROTATING_RIGHT, // Rotating right animation
    ANIMATION_STATE_ROTATING_LEFT,  // Rotating left animation
    // Add more animation states here in the future (e.g., ANIMATION_STATE_ZOOMING, ANIMATION_STATE_FADING)
} AnimationState;

unsigned char* originalImage = NULL;
unsigned char* backUpImage = NULL;
SDL_Texture *editing_image = NULL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
BMPMetadata metadata;
short finalAngleAfterRotation;
AnimationState currentAnimationState = ANIMATION_STATE_NONE;



int chargeTexture(const char *imgPath);

void HandleButtonClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        ButtonAction action = (ButtonAction)userData;

        switch (action) {
            case ACTION_CUT:
                printf("Cut function called!\n");
                // Call cut function here
                break;

            case ACTION_ROTATE_RIGHT:
                printf("Rotate function called!\n");
                // Call rotation function here
                rotateBMPM(&metadata, &originalImage, 'r');
                currentAnimationState = ANIMATION_STATE_ROTATING_RIGHT;

                finalAngleAfterRotation += 90;
                if(finalAngleAfterRotation >= 360)
                    finalAngleAfterRotation = 0;
                break;
            case ACTION_ROTATE_LEFT:
                printf("Rotate function called!\n");
                // Call rotation function here
                rotateBMPM(&metadata, &originalImage, 'l');
                currentAnimationState = ANIMATION_STATE_ROTATING_LEFT;

                finalAngleAfterRotation -= 90;
                if(finalAngleAfterRotation <= -360)
                    finalAngleAfterRotation = 0;
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

            case ACTION_OPEN_FILE:
                printf("Opening file...\n");

                char imgPath[256];
                printf("Enter the path of the image: ");
                scanf("%255s", imgPath);

                if (chargeTexture(imgPath)) {
                    printf("Image loaded successfully.\n");
                } else {
                    printf("Failed to load image.\n");
                }

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
            .sizing = {.height = BTN_H},
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER}
        },
        .backgroundColor = Clay_Hovered() ? ButtonHoverColor : ButtonColor,
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
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
    CLAY({.layout = {.sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
    {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
    }
}

void RenderCentralPanel() {
    int mainWwidth = 0, mainWheight = 0, img_width = 0, img_height = 0;
    SDL_GetWindowSize(window, &mainWwidth, &mainWheight);
    int scaled_width = 0, scaled_height = 0;

    // Central panel container
    CLAY({
        .id = CLAY_ID("CentralPanel"),
        .backgroundColor = BackgroundColor,
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(0),
            .childGap = 0,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        if (!originalImage) {
            printf("No image to render\n");
            return;
        } //mod
            img_width = metadata.width;
            img_height = metadata.height;


            // printf("Image dimensions: %dx%d\n", img_width, img_height);

            // Calculations for available space; adjust BOX_PADD and BOX_H as defined in your project
            int available_width = mainWwidth - BOX_PADD;
            int available_height = mainWheight - (BOX_PADD * 2) - (BOX_H * 2);
            float scale_width = (float)available_width / (float)img_width;
            float scale_height = (float)available_height / (float)img_height;
            float scale = (scale_width < scale_height) ? scale_width : scale_height;

            if (img_height > available_height || img_width > available_width) {
                scaled_width = (int)(img_width * scale);
                scaled_height = (int)(img_height * scale);
            } else {
                scaled_width = img_width;
                scaled_height = img_height;
            }
            // printf("Scaled dimensions: %dx%d\n", scaled_width, scaled_height);
            CLAY({
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(scaled_width),
                        .height = CLAY_SIZING_FIXED(scaled_height)
                    }
                },
                .image = {
                    .sourceDimensions = {scaled_width, scaled_height},
                    // Pass the texture directly
                    .imageData = editing_image
                }
            }) {};
    }
}


void RenderEditButtons() {
    // Renders the Bottom panel that holds the buttons to edit the image
    CLAY({ .id = CLAY_ID("EditsButtons"), .backgroundColor = BackgroundColor, .layout = { .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(BOX_PADD), .childGap = BOX_PADD, .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER } }, .cornerRadius = CLAY_CORNER_RADIUS(8) })
    {
        RenderButton(CLAY_STRING("Cut"), ACTION_CUT);

        Clay_ElementDeclaration btnConfig =
        {
            .id = CLAY_ID("rotate_btn"),
            .layout = {
                .padding = {0, 0, 0, 0},
                .sizing = {.height = BTN_H, .width = BTN_W},
                .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
                .childGap = 20
            }
        };

        if (Clay_PointerOver(CLAY_ID("rotate_btn"))) {
            btnConfig.backgroundColor = ButtonHoverColor;
            btnConfig.cornerRadius = (Clay_CornerRadius){ 0, 0, 0, 0 };
            btnConfig.border = (Clay_BorderElementConfig){
                .width = {1, 1, 0, 1},
                .color = ColorWhite
            };
        } else {
            btnConfig.backgroundColor = ButtonColor;
            btnConfig.cornerRadius = (Clay_CornerRadius){ 6, 6, 6, 6 };
            btnConfig.border = (Clay_BorderElementConfig){
                .width = {1, 1, 1, 1},
                .color = FullBackgroundColor
            };
        }

        CLAY(btnConfig) {
            // Button content goes here

            // Button text and effects
            Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE_MENU); CLAY_TEXT(CLAY_STRING("Rotate"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));
            bool rotMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rotate_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rot_menu")));
            if (rotMenuVisible  /*|| rotMenuClicked*/)
            {
                /* Rotating Up Slide */
                CLAY({
                    .id = CLAY_ID("rot_menu"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM      //The layout direction should apply to the floating element
                    },
                    .floating = { .attachTo = CLAY_ATTACH_TO_PARENT,
                        .attachPoints = {
                            .element = CLAY_ATTACH_POINT_RIGHT_BOTTOM,
                            .parent = CLAY_ATTACH_POINT_RIGHT_TOP
                        }
                    }
                }){
                    CLAY({  //The buttons should be siblings under the floating element.
                        .id = CLAY_ID("rot_r"),
                        .layout = {
                            .sizing = {.width = BTN_W, .height = BTN_H},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT},
                            .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}
                        },
                        .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor,
                        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
                    }){
                        Clay_OnHover(HandleButtonClick, ACTION_ROTATE_RIGHT); CLAY_TEXT(CLAY_STRING("Right"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }

                    CLAY({
                        .id = CLAY_ID("rot_l"),
                        .layout = {
                            .sizing = {.width = BTN_W, .height = BTN_H},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT},
                            .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}
                        },
                        .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor,
                        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
                    }){
                        Clay_OnHover(HandleButtonClick, ACTION_ROTATE_LEFT); CLAY_TEXT(CLAY_STRING("Left"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }
                }
            }
        }
    }
}

static Clay_RenderCommandArray CreateLayout() {

    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    // Main container
    CLAY({ .id = CLAY_ID("OuterContainer"), .backgroundColor = FullBackgroundColor, .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = layoutExpand, .padding = {5, 5, 5, 5}, .childGap = 5} })
    {
        // Header Bar Layout
        CLAY({ .id = CLAY_ID("HeaderBar"), .layout = { .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(BOX_PADD), .childGap = BOX_PADD, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER} }, .backgroundColor = BackgroundColor, .cornerRadius = CLAY_CORNER_RADIUS(5) })
        { // Child header butons
            /* File button */
            CLAY({.id = CLAY_ID("file_btn"), .layout = { .padding = {BOX_PADD, BOX_PADD, 0, 0}, .sizing = {.height = BTN_H, .width = BTN_W}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}}, .backgroundColor = Clay_Hovered() ? ButtonHoverColor : ButtonColor, .cornerRadius = CLAY_CORNER_RADIUS(6), .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
            {
                // Button text and efects
                Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE_MENU); CLAY_TEXT(CLAY_STRING("File"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));
                bool fileMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("file_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("file_menu")));
                if (fileMenuVisible  /*|| fileMenuClicked*/)
                {
                    /* File Dropdown Menu */
                    CLAY({ .id = CLAY_ID("file_menu"), .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .attachPoints = { .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM },} })
                    {
                        CLAY({ .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.height = CLAY_SIZING_GROW(0), .width = CLAY_SIZING_FIXED(BTN_W)} } })
                        {
                            // Render dropdown items here
                            CLAY({ .id = CLAY_ID("open_file_btn"), .layout = {.sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
                            {
                                Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE); CLAY_TEXT(CLAY_STRING("Open"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                            }
                            CLAY({ .id = CLAY_ID("close_file_btn"), .layout = {.sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
                            {
                                Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE); CLAY_TEXT(CLAY_STRING("Close"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                            }

                        }
                    }
                }
            }

        /* Edit button */CLAY({.id = CLAY_ID("edit_btn"), .layout = { .padding = {BOX_PADD, BOX_PADD, 0, 0}, .sizing = {.height = 25}, .childAlignment = {.y = CLAY_ALIGN_Y_CENTER} }, .backgroundColor = Clay_Hovered() ? ButtonHoverColor : ButtonColor, .cornerRadius = CLAY_CORNER_RADIUS(6), .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
        { Clay_OnHover(HandleButtonClick, ACTION_OPEN_EDIT_MENU); CLAY_TEXT(CLAY_STRING("Edit"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));}
        /* Drop Down Edit Menu */
        }

        // Image Display Panel
        RenderCentralPanel();

        // Edit Buttons
        RenderEditButtons();
    }

    return Clay_EndLayout();
}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int chargeTexture(const char *imgPath) {
    currentAnimationState = ANIMATION_STATE_NONE;
    finalAngleAfterRotation = 0;
    if (loadImageBMPM(imgPath, &metadata, &originalImage)) {
        editing_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, metadata.width, metadata.height);
        if (!editing_image) {
            fprintf(stderr, "Error: could not create texture from image: %s\n", SDL_GetError());
            free(originalImage);
            return 0;
        }
        if (SDL_UpdateTexture(editing_image, NULL, originalImage, calculatePitch(&metadata)) != 0) {
            fprintf(stderr, "Error: could not update texture: %s\n", SDL_GetError());
            free(originalImage);
            SDL_DestroyTexture(editing_image);
            return 0;
        }
        SDL_SetTextureBlendMode(editing_image, SDL_BLENDMODE_BLEND);
        return 1;
    } else {
        fprintf(stderr, "Error: could not load image: %s\n", SDL_GetError());
        return 0;
    }
}

int chargeTexture_test() {
    metadata.width = 64;
    metadata.height = 64;

    // No need for originalImage buffer anymore, create texture directly

    editing_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, metadata.width, metadata.height); // Use TARGET for direct manipulation
    if (!editing_image) {
        fprintf(stderr, "Error in chargeTexture_test: SDL_CreateTexture failed: %s\n", SDL_GetError());
        return 0;
    }

    SDL_SetRenderTarget(renderer, editing_image); // Set texture as render target
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_RenderClear(renderer); // Fill texture with red
    SDL_SetRenderTarget(renderer, NULL); // Reset render target to window

    SDL_SetTextureBlendMode(editing_image, SDL_BLENDMODE_BLEND);
    printf("Test texture loaded successfully: %dx%d\n", metadata.width, metadata.height);
    return 1;
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

    // Load an image
    if (!chargeTexture("resources/test_org.bmp")) {
        fprintf(stderr, "Failed to load image.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

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

        /*this switch is for animation control*/
        switch(currentAnimationState){
            case ANIMATION_STATE_ROTATING_RIGHT:
                if(finalAngleAfterRotation > 0){
                    metadata.angle += 5;
                    finalAngleAfterRotation -=5;
                    if(metadata.angle >= 360)
                        metadata.angle -= 360;
                } else {
                    currentAnimationState = ANIMATION_STATE_NONE;
                }
                break;
            case ANIMATION_STATE_ROTATING_LEFT:
                if(finalAngleAfterRotation < 0){
                    metadata.angle -= 5;
                    finalAngleAfterRotation += 5;
                    if(metadata.angle <= -360)
                        metadata.angle += 360;
                } else {
                    currentAnimationState = ANIMATION_STATE_NONE;
                }
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

