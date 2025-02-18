#pragma once
#include <string>
#include <deque>
#include "constants.h"
#include "gameError.h"
#include "graphics.h"
#include "input.h"
#include "textSDL.h"

namespace consoleNS
{
    const unsigned int W = 500;
    const unsigned int H = 400;
    const unsigned int X = 5;
    const unsigned int Y = 5;
    const unsigned int BORDER = 0;
    const unsigned int MARGIN = 4;          // text margin from console edge
    const char FONT[] = "cour.ttf";
    const int FONT_HEIGHT = 12;
    const color_t FONT_COLOR = graphicsNS::WHITE;
    const color_t BACK_COLOR = SETCOLOR_ARGB(192, 128, 128, 128);            // backdrop color
    const int MAX_LINES = 256;          // maximun number of lines in text buffer
}

class Console
{
    // Console properties
private:
    Graphics*   graphics;
    Input*      input;
    TextSDL     font;
    float       x, y;           // console location (dynamic)
    float       w, h;           // width and height
    float       b, m;           // border and margin
    int         rowH;           // row height
    int         rows;           // number of rows of text that will fit on console
    std::string textIn;         // console command
    std::string inputStr;           // console text input
    std::deque<std::string> text;           // console text
    rect_t      textRect;           // text rectangle
    color_t     fontColor;          // font color (a,r,g,b)
    color_t     backColor;          // background color (a,r,g,b)
    vector4_t   vtx[4];         // buffer to hold vertex data
    int         scrollAmount;           // number of lines to scroll the display up
    bool        initialized;            // true when initialized successfully
    bool        visible;            // true to display

public:
    // Constructor
    Console();

    // Destructor
    ~Console();

    // Initialize the Console
    bool initialize(Graphics* pGraphics, Input* pInput);

    // Display the Console.
    void draw();

    // update the console
    void update(float frameTime);

    // Show/Hide the Console.
    void showHide();

    // Return visible.
    bool getVisible() const;

    // Set visible = true;
    void show();

    // Set visible = false;
    void hide();

    // print variable format string
    void print(const char* fmt, ...);

    // Add text str to Console display.
    // Only the first line of text in str will be displayed.
    void print(const std::string& str);

    // Save the char just entered in textIn string
    void keyIn(unsigned int key);

    // Return Console command
    std::string getCommand();

    // Return Console Input text
    std::string getInput();

    // Clear Input text
    void clearInput();
};

