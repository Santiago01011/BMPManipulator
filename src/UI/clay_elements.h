#ifndef CLAY_ELEMENTS_H_INCLUDED
#define CLAY_ELEMENTS_H_INCLUDED

/**Buttons defines**/
#define BOX_PADD 20
#define BOX_H 40
#define BTN_H 30
#define BTN_W 75

typedef enum {
    ACTION_CUT,
    ACTION_ROTATE,
    ACTION_ROTATE_RIGHT,
    ACTION_ROTATE_LEFT,
    ACTION_CHANGE_RGB,
    ACTION_CHANGE_R,
    ACTION_CHANGE_G,
    ACTION_CHANGE_B,
    ACTION_SCALE,
    ACTION_GRAYSCALE,
    ACTION_OPEN_FILE,
    ACTION_OPEN_EDIT_MENU,
    ACTION_OPEN_FILE_MENU,
    ACTION_NONE
} ButtonAction;


/**
 * @brief Enumeration of possible animation states for the BMPM UI.
 *
 * This enumeration defines the possible states for animations that can be applied to the
 * image displayed in the central panel of the BMPM UI.
 *
 * \since BMPM 0.1.0
 */
typedef enum {
    ANIMATION_STATE_NONE,      // No animation active
    ANIMATION_STATE_ROTATING_RIGHT, // Rotating right animation
    ANIMATION_STATE_ROTATING_LEFT,  // Rotating left animation
    // Add more animation states here in the future (e.g., ANIMATION_STATE_ZOOMING, ANIMATION_STATE_FADING)
} AnimationState;


/**
 * @brief Structure to hold the calculated display dimensions (width and height) for an image.
 *
 * This structure is used by the `_calculateDisplayImgDimentions` function to
 * return the scaled width and height of the image to be displayed in the central panel.
 */
typedef struct {
    int width;
    int height;
} DisplayImgDimensions;

void HandleButtonClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
void RenderButton(Clay_String text, ButtonAction action);
void RenderDropdownMenuItem(Clay_String text);

/**|
 * @brief Generates the central panel render command of the BMPM UI, displaying the editing image.
 *
 * It performs the following steps:
 *
 * 1. **Creates Central Panel Container:** Uses the CLAY macro to define a container
 *    for the central panel. This container provides background color, layout properties
 *    (growable width/height, padding, child alignment), and rounded corners.
 * 2. **Checks for Image Availability:** Verifies if `originalImage` is loaded. If not (NULL),
 *    it prints a "No image to render" message to the console and returns early, preventing
 *    rendering of an empty panel.
 * 3. **Calculates Scaled Dimensions:** Calls the `_calculateDisplayImgDimentions` function
 *    to determine the appropriately scaled width and height for the image to fit within
 *    the central panel while maintaining aspect ratio.
 * 4. **Renders Image:** Uses the CLAY macro again to render the image itself within the
 *    central panel container. It sets the layout to a fixed size based on the
 *    calculated `scaledDimensions` and specifies the `editing_image` texture as the
 *    image data source, using `scaledDimensions` as the `sourceDimensions`.
 *
 * \note If no image is loaded (`originalImage` is NULL), this function will not render
 *       the image area and will print a message to the console.
 *
 * \since BMPM 0.1.0
 *
 * \see _calculateDisplayImgDimentions - Function called to calculate the scaled image dimensions.
 *
 * **Global Variables Used (extern, defined in main.c):**
 * - `originalImage`:   Pointer to the original loaded BMP image data.
 * - `editing_image`:   SDL_Texture representing the image being edited, ready for rendering.
 */
void RenderCentralPanel();
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
DisplayImgDimensions _calculateDisplayImgDimentions();

/**
 * @brief Renders the bottom panel that holds the buttons to edit the image.
 *
 * This function generates the render command for the bottom panel of the BMPM UI.
 * The panel contains buttons for various editing actions, such as cutting, rotating,
 * changing RGB values, scaling, and applying grayscale effects.
 *
 * The function performs the following steps:
 *
 * 1. **Create Bottom Panel Container:** Uses the CLAY macro to define a container for the
 *    bottom panel. This container provides background color, layout properties (fixed height,
 *    growable width, padding, child gap, child alignment), and rounded corners.
 * 2. **Render Buttons:** Calls the `RenderButton` function to render individual buttons for
 *    each editing action. The buttons are defined using the CLAY macro and include properties
 *    such as layout, background color, corner radius, border, and text configuration.
 *
 * \since BMPM 0.1.0
 *
 * \see RenderButton
 */
void RenderEditButtons();
Clay_RenderCommandArray CreateLayout();


#endif // CLAY_ELEMENTS_H_INCLUDED
