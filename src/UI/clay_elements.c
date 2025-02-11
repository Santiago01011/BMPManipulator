#include "clay.h"          // Clay UI header
#include "clay_elements.h" // Include its own header
//#include "clay_renderer_SDL2.c" // not used here
#include <SDL.h>         // SDL (you might need SDL_rect.h etc. if used in UI functions)
#include <stdio.h>         // for printf, etc.
#include "../BMPManipulator/bmp_functions.h" // For BMPMetadata and rotateBMPM if used in UI actions
#include "../Utils/textures_utils.h"

#include <stdlib.h>

const Clay_Color ColorWhite = { 255, 255, 255, 255};
const Clay_Color FullBackgroundColor = (Clay_Color){39, 54, 61 ,255}; //rgb(39, 54, 61)
const Clay_Color BackgroundColor = (Clay_Color){47, 62, 70 ,255}; //rgb(47, 62, 70)
const Clay_Color ButtonColor = (Clay_Color){53, 79, 82, 255}; //rgb(53, 79, 82)
const Clay_Color ButtonHoverColor = (Clay_Color){82, 121, 111, 100}; // rgb(82, 121, 111)

extern unsigned char* originalImage;
extern unsigned char* backUpImage;
extern SDL_Texture *editing_image;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern BMPMetadata metadata;
extern short finalAngleAfterRotation;
extern AnimationState currentAnimationState;
extern const int FontIdBody16;

void HandleButtonClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData) {
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        ButtonAction action = (ButtonAction)userData;

        switch (action) {
            case ACTION_CUT:
                printf("Cut function called!\n");
                // Call cut function here
                break;

            case ACTION_ROTATE_RIGHT:
                backUpImage = (unsigned char*)malloc(metadata.height * calculatePitch(&metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, originalImage, metadata.height* calculatePitch(&metadata));
                rotateBMPM(&metadata, &backUpImage, 'r');
                currentAnimationState = ANIMATION_STATE_ROTATING_RIGHT;

                finalAngleAfterRotation += 90;
                if(finalAngleAfterRotation >= 360)
                    finalAngleAfterRotation = 0;
                break;
            case ACTION_ROTATE_LEFT:
                backUpImage = (unsigned char*)malloc(metadata.height * calculatePitch(&metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, originalImage, metadata.height* calculatePitch(&metadata));
                rotateBMPM(&metadata, &backUpImage, 'l');
                currentAnimationState = ANIMATION_STATE_ROTATING_LEFT;

                finalAngleAfterRotation -= 90;
                if(finalAngleAfterRotation <= -360)
                    finalAngleAfterRotation = 0;
                break;

            case ACTION_CHANGE_R:
                backUpImage = (unsigned char*)malloc(metadata.height * calculatePitch(&metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, originalImage, metadata.height* calculatePitch(&metadata));
                changeRGBvalues(&metadata, backUpImage, 1.25, 0.75, 0.75);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&metadata));
                free(backUpImage);
                break;

             case ACTION_CHANGE_G:
                backUpImage = (unsigned char*)malloc(metadata.height * calculatePitch(&metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change red value");
                    return;
                }
                memcpy(backUpImage, originalImage, metadata.height* calculatePitch(&metadata));
                changeRGBvalues(&metadata, backUpImage, 0.75, 1.25, 0.75);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&metadata));
                free(backUpImage);
                break;

             case ACTION_CHANGE_B:
                backUpImage = (unsigned char*)malloc(metadata.height * calculatePitch(&metadata));
                if(!backUpImage){
                    puts("Error allocating memory to change green value");
                    return;
                }
                memcpy(backUpImage, originalImage, metadata.height* calculatePitch(&metadata));
                changeRGBvalues(&metadata, backUpImage, 0.75, 0.75, 1.25);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&metadata));
                free(backUpImage);
                break;

            case ACTION_SCALE:
                printf("Scale function called!\n");
                // Call scaling function here
                break;

            case ACTION_GRAYSCALE:
                backUpImage = (unsigned char*)malloc(metadata.height * calculatePitch(&metadata));
                if(!backUpImage){
                    puts("Error allocating memory to gray-scale");
                    return;
                }
                memcpy(backUpImage, originalImage, metadata.height* calculatePitch(&metadata));
                grayScaleBMPM(&metadata, backUpImage);
                SDL_UpdateTexture(editing_image, NULL, backUpImage, calculatePitch(&metadata));
                free(backUpImage);
                break;

            case ACTION_OPEN_FILE:
                printf("Opening file...\n");

                char imgPath[256];
                printf("Enter the path of the image: ");
                scanf("%255s", imgPath);

                if (chargeTexture(imgPath)) {
                    printf("Image loaded successfully.\n");
                } else {
                    chargeTexture("resources/noImage.bmp");
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

/**
 * @brief Renders a dropdown menu item with the specified text.
 *
 * This function creates a dropdown menu item using the CLAY macro. The item includes properties such as layout, background color,
 * border, and text configuration.
 *
 * Steps performed by this function:
 *
 * 1. **Define Layout and Styling:** Uses the CLAY macro to define the layout, sizing, child alignment, padding, background color,
 * and border of the dropdown menu item. The background color changes based on whether the item is hovered.
 * 2. **Render Text:** Uses the CLAY_TEXT macro to render the provided text within the dropdown menu item. The text configuration
 * includes font ID, font size, and text color.
 *
 * \param text The text to be displayed in the dropdown menu item.
 *
 * \since BMPM 0.1.0
 */
void RenderDropdownMenuItem(Clay_String text) {
    CLAY({.layout = {.sizing = {.width = BTN_W, .height = BTN_H}, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT}, .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}}, .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor, .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}})
    {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
    }
}

/**
 * @brief Generates the central panel render command of the BMPM UI, displaying the editing image.
 *
 * This function performs the following steps:
 *
 * 1. **Creates Central Panel Container:** Uses the CLAY macro to define a container for the central panel. This container provides
 * background color, layout properties (growable width/height, padding, child alignment), and rounded corners.
 * 2. **Checks for Image Availability:** Verifies if `originalImage` is loaded. If not (NULL), it prints a "No image to render" message
 * to the console and returns early, preventing rendering of an empty panel.
 * 3. **Calculates Scaled Dimensions:** Calls the `_calculateDisplayImgDimentions` function to determine the appropriately scaled width
 * and height for the image to fit within the central panel while maintaining aspect ratio.
 * 4. **Renders Image:** Uses the CLAY macro again to render the image itself within the central panel container. It sets the layout to
 * a fixed size based on the calculated `scaledDimensions` and specifies the `editing_image` texture as the image data source, using
 * `scaledDimensions` as the `sourceDimensions`.
 *
 * \since BMPM 0.1.0
 */
void RenderCentralPanel() {
    DisplayImgDimensions scaledDimentions;

    /*Central panel container*/
    CLAY({ .id = CLAY_ID("CentralPanel"), .backgroundColor = BackgroundColor, .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(0), .childGap = 0, .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER } }, .cornerRadius = CLAY_CORNER_RADIUS(5) })
    { /*Here we set the layout for display the current image*/

        /*Call to a function for handle the size of the image to display it inside of the container*/
        scaledDimentions = _calculateDisplayImgDimentions();
        /*Image*/
        CLAY({ .layout = {.childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }, .sizing = { .width = CLAY_SIZING_FIXED(scaledDimentions.width), .height = CLAY_SIZING_FIXED(scaledDimentions.height) } }, .image = { .sourceDimensions = {scaledDimentions.width, scaledDimentions.height}, .imageData = editing_image } }) { if(!originalImage){ CLAY_TEXT(CLAY_STRING("NO IMAGE TO DISPLAY"),CLAY_TEXT_CONFIG({ .fontId = 0, .fontSize =  42, .textColor = {255,255,255,255} })); };}
    }
}

void RenderEditButtons() {
    // Renders the Bottom panel that holds the buttons to edit the image
    CLAY({ .id = CLAY_ID("EditsButtons"), .backgroundColor = BackgroundColor, .layout = { .sizing = { .height = CLAY_SIZING_FIXED(BOX_H), .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(BOX_PADD), .childGap = BOX_PADD, .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER } }, .cornerRadius = CLAY_CORNER_RADIUS(8) })
    {
        RenderButton(CLAY_STRING("Cut"), ACTION_CUT);

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
            Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE_MENU); CLAY_TEXT(CLAY_STRING("Rotate"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));
            bool rotMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rotate_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rot_menu")));
            if (rotMenuVisible  /*|| rotMenuClicked*/)
            {
                CLAY({
                    .id = CLAY_ID("rot_menu"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .floating = { .attachTo = CLAY_ATTACH_TO_PARENT,
                        .attachPoints = {
                            .element = CLAY_ATTACH_POINT_RIGHT_BOTTOM,
                            .parent = CLAY_ATTACH_POINT_RIGHT_TOP
                        }
                    }
                }){
                    CLAY({
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
        Clay_ElementDeclaration RGB_btn = { .id = CLAY_ID("RGB_btn"), .layout = { .padding = {0, 0, 0, 0}, .sizing = {.height = BTN_H, .width = BTN_W}, .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}, .childGap = 20 } };
        if (Clay_PointerOver(CLAY_ID("RGB_btn"))) {
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
            Clay_OnHover(HandleButtonClick, ACTION_OPEN_FILE_MENU); CLAY_TEXT(CLAY_STRING("RGB"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite}));
            bool rgbMenuVisible = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("RGB_btn"))) || Clay_PointerOver(Clay_GetElementId(CLAY_STRING("rgb_menu")));
            if (rgbMenuVisible  /*|| rotMenuClicked*/)
            {
                CLAY({
                    .id = CLAY_ID("rgb_menu"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .floating = { .attachTo = CLAY_ATTACH_TO_PARENT,
                        .attachPoints = {
                            .element = CLAY_ATTACH_POINT_RIGHT_BOTTOM,
                            .parent = CLAY_ATTACH_POINT_RIGHT_TOP
                        }
                    }
                }){
                    CLAY({
                        .id = CLAY_ID("rgb-r"),
                        .layout = {
                            .sizing = {.width = BTN_W, .height = BTN_H},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT},
                            .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}
                        },
                        .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor,
                        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
                    }){
                        Clay_OnHover(HandleButtonClick, ACTION_CHANGE_R); CLAY_TEXT(CLAY_STRING("Red"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }

                    CLAY({
                        .id = CLAY_ID("rgb-g"),
                        .layout = {
                            .sizing = {.width = BTN_W, .height = BTN_H},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT},
                            .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}
                        },
                        .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor,
                        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
                    }){
                        Clay_OnHover(HandleButtonClick, ACTION_CHANGE_G); CLAY_TEXT(CLAY_STRING("Green"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }
                    CLAY({
                        .id = CLAY_ID("rgb-b"),
                        .layout = {
                            .sizing = {.width = BTN_W, .height = BTN_H},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT},
                            .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}
                        },
                        .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor,
                        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
                    }){
                        Clay_OnHover(HandleButtonClick, ACTION_CHANGE_B); CLAY_TEXT(CLAY_STRING("Blue"), CLAY_TEXT_CONFIG({ .fontId = FontIdBody16, .fontSize = 12, .textColor = ColorWhite }));
                    }
                    CLAY({
                        .id = CLAY_ID("rgb-grey"),
                        .layout = {
                            .sizing = {.width = BTN_W, .height = BTN_H},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x =  CLAY_ALIGN_X_LEFT},
                            .padding = {BOX_PADD / 2, BOX_PADD, 0, 0}
                        },
                        .backgroundColor =  Clay_Hovered() ? ButtonHoverColor : ButtonColor,
                        .border = {.width = {1,1,1,1}, .color = !Clay_Hovered() ? FullBackgroundColor : ColorWhite}
                    }){
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

DisplayImgDimensions _calculateDisplayImgDimentions(){
    DisplayImgDimensions auxDim;
    int mainWwidth = 0, mainWheight = 0, img_width = 0, img_height = 0,
    _aux, available_width, available_height;
    SDL_GetWindowSize(window, &mainWwidth, &mainWheight);
    float scale_width = 0, scale_height = 0, scale = 0;

    img_width = metadata.width;
    img_height = metadata.height;

    // Calculations for available space
    available_width = mainWwidth - BOX_PADD;
    available_height = mainWheight - (BOX_PADD * 2) - (BOX_H * 2);
    if (metadata.angle % 180 != 0){ //swap the available space if image is rotated vertically, to adjust layout
        _aux = available_height;
        available_height = available_width;
        available_width = _aux;
    }
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


