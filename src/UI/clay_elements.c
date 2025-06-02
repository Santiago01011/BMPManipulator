#include "clay.h"
#include "clay_elements.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "../BMPManipulator/bmp_functions.h"
#include "../Utils/textures_utils.h"

/*BMPManipulator\src\UI\clay_elements.c*/

const Clay_Color ColorWhite = { 255, 255, 255, 255};
const Clay_Color FullBackgroundColor = (Clay_Color){39, 54, 61 ,255}; //rgb(39, 54, 61)
const Clay_Color BackgroundColor = (Clay_Color){47, 62, 70 ,255}; //rgb(47, 62, 70)
const Clay_Color ButtonColor = (Clay_Color){53, 79, 82, 255}; //rgb(53, 79, 82)
const Clay_Color ButtonHoverColor = (Clay_Color){82, 121, 111, 100}; // rgb(82, 121, 111)

bool isFileMenuFocused;


extern unsigned char* backUpImage;
extern ImageBMP originalImageBMP;
extern SDL_Texture *editing_image;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern short finalAngleAfterRotation;
extern AnimationState currentAnimationState;
extern const int FontIdBody16;

extern bool isFilePathInputFocused;
extern char filePathBuffer[256];

static DisplayImgDimensions _calculateDisplayImgDimentions();


void HandleButtonClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        ButtonAction action = (ButtonAction)userData;

        switch (action) {
            case ACTION_INPUT_FILEPATH_FOCUS:
                isFilePathInputFocused = true;
                filePathBuffer[0] = '\0';
                SDL_StartTextInput();
                printf("File Path Input Focused (input area click)!\n"); // Debugging
                break;

            case ACTION_OPEN_FILE:
                if(isFilePathInputFocused)
                    isFilePathInputFocused = false;
                else
                    isFilePathInputFocused = true;
                break;

            case ACTION_OPEN_FILE_MENU:
                if(isFileMenuFocused)
                    isFileMenuFocused = false;
                else
                    isFileMenuFocused = true;
                break;

            case ACTION_ROTATE_RIGHT:
                backUpImage = (unsigned char*)malloc(originalImageBMP.metadata.height * calculatePitch(&originalImageBMP.metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, (void*)originalImageBMP.pixels, originalImageBMP.metadata.height* calculatePitch(&originalImageBMP.metadata));
                //rotateBMPM(&originalImageBMP.metadata, &backUpImage, 'r');
                currentAnimationState = ANIMATION_STATE_ROTATING_RIGHT;

                finalAngleAfterRotation += 90;
                if(finalAngleAfterRotation >= 360)
                    finalAngleAfterRotation = 0;
                break;

            case ACTION_ROTATE_LEFT:
                backUpImage = (unsigned char*)malloc(originalImageBMP.metadata.height * calculatePitch(&originalImageBMP.metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, (void*)originalImageBMP.pixels, originalImageBMP.metadata.height* calculatePitch(&originalImageBMP.metadata));
                //rotateBMPM(&originalImageBMP.metadata, &backUpImage, 'l');
                currentAnimationState = ANIMATION_STATE_ROTATING_LEFT;

                finalAngleAfterRotation -= 90;
                if(finalAngleAfterRotation <= -360)
                    finalAngleAfterRotation = 0;
                break;

            case ACTION_CHANGE_R:
                backUpImage = (unsigned char*)malloc(originalImageBMP.metadata.height * calculatePitch(&originalImageBMP.metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, (void*)originalImageBMP.pixels, originalImageBMP.metadata.height* calculatePitch(&originalImageBMP.metadata));
                //changeRGBvalues(&originalImageBMP.metadata, backUpImage, 1.25, 0.75, 0.75);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&originalImageBMP.metadata));
                free(backUpImage);
                break;

             case ACTION_CHANGE_G:
                backUpImage = (unsigned char*)malloc(originalImageBMP.metadata.height * calculatePitch(&originalImageBMP.metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, (void*)originalImageBMP.pixels, originalImageBMP.metadata.height* calculatePitch(&originalImageBMP.metadata));
                //changeRGBvalues(&originalImageBMP.metadata, backUpImage, 0.75, 1.25, 0.75);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&originalImageBMP.metadata));
                free(backUpImage);
                break;

             case ACTION_CHANGE_B:
                backUpImage = (unsigned char*)malloc(originalImageBMP.metadata.height * calculatePitch(&originalImageBMP.metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change green value");
                    return;
                }
                memcpy(backUpImage, (void*)originalImageBMP.pixels, originalImageBMP.metadata.height* calculatePitch(&originalImageBMP.metadata));
                //changeRGBvalues(&originalImageBMP.metadata, backUpImage, 0.75, 0.75, 1.25);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&originalImageBMP.metadata));
                free(backUpImage);
                break;

            case ACTION_SCALE:
                printf("Scale function called!\n");
                // Call scaling function here
                break;

            case ACTION_GRAYSCALE:
                backUpImage = (unsigned char*)malloc(originalImageBMP.metadata.height * calculatePitch(&originalImageBMP.metadata));
                if(!backUpImage){
                    puts("Error allocating memory to gray-scale");
                    return;
                }
                memcpy(backUpImage, (void*)originalImageBMP.pixels, originalImageBMP.metadata.height* calculatePitch(&originalImageBMP.metadata));
                //grayScaleBMPM(&originalImageBMP.metadata, backUpImage);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&originalImageBMP.metadata));
                free(backUpImage);
                break;

            default:
                printf("Unknown action\n");
                break;
        }
    }
}

/*
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna
aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint
occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
*/

void RenderCentralPanel() {
    DisplayImgDimensions scaledDimentions;

    /*Central panel container*/
    CLAY({ .id = CLAY_ID("CentralPanel"), .backgroundColor = BackgroundColor, .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(0), .childGap = 0, .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER } }, .cornerRadius = CLAY_CORNER_RADIUS(5) })
    { /*Here we set the layout for display the current image*/

        /*Call to a function for handle the size of the image to display it inside of the container*/
        scaledDimentions = _calculateDisplayImgDimentions();
        /*Image*/
        CLAY({ .layout = {.childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }, .sizing = { .width = CLAY_SIZING_FIXED(scaledDimentions.width), .height = CLAY_SIZING_FIXED(scaledDimentions.height) } }, .image = { .imageData = editing_image } }) { if(!(void*)originalImageBMP.pixels){ CLAY_TEXT(CLAY_STRING("NO IMAGE TO DISPLAY"),CLAY_TEXT_CONFIG({ .fontId = 0, .fontSize =  42, .textColor = {255,255,255,255} })); };}
    }
}

void RenderEditButtons() {
    // Renders the Bottom panel that holds the buttons to edit the image
    CLAY({ .id = CLAY_ID("EditsButtons"), .backgroundColor = BackgroundColor, .layout = { .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(BOX_PADD), .childGap = BOX_PADD, .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER } }, .cornerRadius = CLAY_CORNER_RADIUS(8) })
    {
        Clay_ElementDeclaration rotate_btn = { .id = CLAY_ID("rotate_btn"), .layout = { .padding = {0, 0, 0, 0}, .sizing = {.height = BTN_H, .width = BTN_W}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}, .childGap = 20 } };
        if (Clay_PointerOver(CLAY_ID("rotate_btn"))) {
            rotate_btn.backgroundColor = ButtonHoverColor;
            rotate_btn.cornerRadius = (Clay_CornerRadius){ 0, 0, 0, 0 };
            rotate_btn.border = (Clay_BorderElementConfig){
                .width = {1, 1, 0, 1},
                .color = ColorWhite
            };
        } else {
            rotate_btn.backgroundColor = ButtonColor;
            rotate_btn.cornerRadius = (Clay_CornerRadius){ 6, 6, 6, 6 };
            rotate_btn.border = (Clay_BorderElementConfig){
                .width = {1, 1, 1, 1},
                .color = FullBackgroundColor
            };
        }
        CLAY(rotate_btn) {
            // Button content goes here
            // Button text and effects
            Clay_OnHover(HandleButtonClick, ACTION_TOGGLE_EDIT_MENU); CLAY_TEXT(CLAY_STRING("Rotate"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));
            bool rotMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rotate_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rot_menu")));
            if (rotMenuVisible)
            {
                CLAY({ .id = CLAY_ID("rot_menu"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM }, .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .attachPoints = { .element = CLAY_ATTACH_POINT_RIGHT_BOTTOM, .parent = CLAY_ATTACH_POINT_RIGHT_TOP } } })
                {
                    CLAY({ .id = CLAY_ID("rot_r"), .layout = { .sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0} }, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
                    {
                        Clay_OnHover(HandleButtonClick, ACTION_ROTATE_RIGHT); CLAY_TEXT(CLAY_STRING("Right"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }

                    CLAY({ .id = CLAY_ID("rot_l"), .layout = { .sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0} }, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
                    {
                        Clay_OnHover(HandleButtonClick, ACTION_ROTATE_LEFT); CLAY_TEXT(CLAY_STRING("Left"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }
                }
            }
        }
        Clay_ElementDeclaration RGB_btn = { .id = CLAY_ID("RGB_btn"), .layout = { .padding = {0, 0, 0, 0}, .sizing = {.height = BTN_H, .width = BTN_W}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}, .childGap = 20 } };
        if (Clay_PointerOver(CLAY_ID("RGB_btn")) || Clay_PointerOver(CLAY_ID("rgb_menu"))) {
            RGB_btn.backgroundColor = ButtonHoverColor;
            RGB_btn.cornerRadius = (Clay_CornerRadius){ 0, 0, 0, 0 };
            RGB_btn.border = (Clay_BorderElementConfig){
                .width = {1, 1, 0, 1},
                .color = ColorWhite
            };
        } else {
            RGB_btn.backgroundColor = ButtonColor;
            RGB_btn.cornerRadius = (Clay_CornerRadius){ 6, 6, 6, 6 };
            RGB_btn.border = (Clay_BorderElementConfig){
                .width = {1, 1, 1, 1},
                .color = FullBackgroundColor
            };
        }
        CLAY(RGB_btn) {
            // Button content goes here
            // Button text and effects
            Clay_OnHover(HandleButtonClick, ACTION_TOGGLE_EDIT_MENU); CLAY_TEXT(CLAY_STRING("RGB"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));
            bool rgbMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("RGB_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rgb_menu")));
            if (rgbMenuVisible)
            {
                CLAY({ .id = CLAY_ID("rgb_menu"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM }, .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .attachPoints = { .element = CLAY_ATTACH_POINT_RIGHT_BOTTOM, .parent = CLAY_ATTACH_POINT_RIGHT_TOP } } })
                {
                    CLAY({ .id = CLAY_ID("rgb-r"), .layout = { .sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0} }, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
                    {
                        Clay_OnHover(HandleButtonClick, ACTION_CHANGE_R); CLAY_TEXT(CLAY_STRING("Red"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }

                    CLAY({ .id = CLAY_ID("rgb-g"), .layout = { .sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0} }, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
                    {
                        Clay_OnHover(HandleButtonClick, ACTION_CHANGE_G); CLAY_TEXT(CLAY_STRING("Green"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }
                    CLAY({ .id = CLAY_ID("rgb-b"), .layout = { .sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0} }, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
                    {
                        Clay_OnHover(HandleButtonClick, ACTION_CHANGE_B); CLAY_TEXT(CLAY_STRING("Blue"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }
                    CLAY({ .id = CLAY_ID("rgb-grey"), .layout = { .sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0} }, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite} })
                    {
                        Clay_OnHover(HandleButtonClick, ACTION_GRAYSCALE); CLAY_TEXT(CLAY_STRING("Grey"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }

                }
            }
        }

    }
}

Clay_RenderCommandArray CreateLayout() {

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
                bool fileMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("file_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("file_menu"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("file_input_text_area")));
                if (fileMenuVisible  || isFileMenuFocused)
                {
                    /* File Dropdown Menu */
                    CLAY({ .id = CLAY_ID("file_menu"), .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .attachPoints = { .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM }} })
                    {
                        CLAY({ .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.height = CLAY_SIZING_GROW(0), .width = CLAY_SIZING_FIXED(BTN_W)} } })
                        {
                            // Render dropdown items here
                            CLAY({ .id = CLAY_ID("open_file_btn"), .layout = {.sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
                            {
                                // Button text and efects
                                Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE); CLAY_TEXT(CLAY_STRING("Open"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                                if(isFilePathInputFocused)
                                {
                                    /* File input text area */
                                    CLAY({ .id = CLAY_ID("input_menu"), .floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .attachPoints = { .parent = CLAY_ATTACH_POINT_RIGHT_TOP}} })
                                    {
                                        CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = {.height = CLAY_SIZING_GROW(0), .width = CLAY_SIZING_FIXED(BTN_W)} } })
                                        {
                                            /*This will display the actual buffer of the input text area*/
                                            CLAY({ .id = CLAY_ID("file_input_text_area"), .layout = {.sizing = {.width = BTN_W * 4, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  FullBackgroundColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
                                            {
                                                Clay_OnHover(HandleButtonClick, ACTION_INPUT_FILEPATH_FOCUS);
                                                Clay_String filePathSlice = { .chars = filePathBuffer, .length = strlen(filePathBuffer) };
                                                CLAY_TEXT( filePathSlice, CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }) );
                                            }
                                        }
                                    }
                                }
                            }
                            CLAY({ .id = CLAY_ID("close_file_btn"), .layout = {.sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
                            {
                                Clay_OnHover(HandleButtonClick, ACTION_NONE); CLAY_TEXT(CLAY_STRING("Close"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
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


/**
 * @brief Calculates scaled image dimensions for display within the central panel.
 *
 * This function determines the scale, if needed, to display the image in the available space.
 *
 * The calculation takes into account:
 *  - The current dimensions of the main window.
 *  - Predefined padding and header/button box heights that define the available
 *    space within the central panel.
 *  - Image rotation: If the image is rotated vertically, the available
 *    width and height are swapped during scaling to reflect the new image dimentions.
 *
 * \returns DisplayImgDimensions A structure containing the calculated scaled width and height.
 *
 * \since BMPM 0.1.0
 *
 * \sa RenderCentralPanel
 */
static DisplayImgDimensions _calculateDisplayImgDimentions(){
    DisplayImgDimensions auxDim;
    int mainWwidth = 0, mainWheight = 0, img_width = 0, img_height = 0,
    _aux, available_width, available_height;
    SDL_GetWindowSize(window, &mainWwidth, &mainWheight);
    float scale_width = 0, scale_height = 0, scale = 0;

    img_width = originalImageBMP.metadata.width;
    img_height = originalImageBMP.metadata.height;

    // Calculations for available space
    available_width = mainWwidth - BOX_PADD;
    available_height = mainWheight - (BOX_PADD * 2) - (BOX_H * 2);
    // if (originalImageBMP.metadata.angle % 180 != 0){ //swap the available space if image is rotated vertically, to adjust layout
    //     _aux = available_height;
    //     available_height = available_width;
    //     available_width = _aux;
    // }
    scale_width = (float)available_width / (float)img_width;
    scale_height = (float)available_height / (float)img_height;
    scale = (scale_width < scale_height) ? scale_width : scale_height;

    if (img_height > available_height || img_width > available_width) {
        auxDim.width = (int)(img_width * scale);
        auxDim.height = (int)(img_height * scale);
    } else {
        auxDim.width = img_width;
        auxDim.height = img_height;
    }
    return auxDim;
}


