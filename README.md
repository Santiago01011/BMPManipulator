# BMPM (BMP Manipulator)

## Project Description

BMPM (BMP Manipulator) is a simple, cross-platform BMP image editor created as a project for a university programming course.  The core focus of BMPM is to demonstrate the principles of reading, loading into memory, and performing basic image manipulations on BMP (Bitmap) image files, including both the file header and pixel data.

BMPM utilizes the SDL2 library for efficient, cross-platform rendering of the loaded image data (represented as a bidimensional array in memory). The user interface is built with the Clay UI library, chosen for its lightweight nature and high performance. Take a look at the [Clay UI library](https://github.com/nicbarker/clay?tab=readme-ov-file#clay) for more information.

**Current Features Include:**

*   Loading and displaying BMP images (using custom BMP loading functions).
*   Image rotation in 90-degree increments (right and left).
*   Animated image rotation for smooth visual feedback.
*   Basic Clay UI for image display and rotation controls.
*   High-performance UI rendering with Clay.

## Getting Started

These instructions will help you get a copy of the project up and running on your local machine for development and testing purposes.  The project is designed to be built and run on multiple platforms, including Windows, macOS, and Linux.

### Installation

To get started with BMPM, you will need to follow these steps:

1.  **Clone the Repository:**  Clone the BMPM repository to your local machine using Git

2.  **Install SDL2 Library:**  Download and install the SDL2 and SDL2\_ttf libraries on your system.  Refer to the [SDL2 GitHub Project](https://github.com/libsdl-org/SDL/releases) for installation instructions.

3.  **Compile the Project:** Compile the BMPM project using a C compiler (e.g., GCC, Clang, MSVC). Ensure you link against the SDL2 and SDL2\_ttf libraries, and place the required DLLs in the same directory as the executable.

4.  **Run the Executable:**  Execute the compiled BMPM program to test the image loading and manipulation features.

### Usage

Upon running BMPM, you will be presented with a simple user interface that allows you to load a BMP image file and perform basic image manipulations. The interface includes buttons for simple editing operations, such as rotating the image left or right.

### Prerequisites

*   **SDL2 Library:** You need both the SDL2 runtime library and development headers. Installation instructions vary by operating system. Refer to the [SDL2 website](https://www.libsdl.org/download-2.0.php).
*   **SDL2\_ttf Library:**  Likely required by the Clay UI library for font rendering.  Install both runtime and development headers. See [SDL2\_ttf documentation](https://www.libsdl.org/projects/SDL_ttf/).
*   **C Compiler:** You will need a C compiler toolchain, such as GCC (for Linux/macOS/MinGW on Windows), Clang, or Microsoft Visual C++ (MSVC) Build Tools.
