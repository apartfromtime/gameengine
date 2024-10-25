#pragma once
#include "constants.h"
#include "graphics.h"
#include "input.h"
#include "textSDL.h"

namespace messageDialogNS
{
    const unsigned int W = 400;         // default width of dialog
    const unsigned int H = 100;         // default height
    const unsigned int X = W / 2;           // default location
    const unsigned int Y = H / 2;
    const unsigned int BORDER = 5;
    const unsigned int MARGIN = 5;              // text margin from border
    const char FONT[] = "arial.ttf";        // font
    const int FONT_HEIGHT = 18;         // font height
    const color_t FONT_COLOR = graphicsNS::WHITE;        // text color
    const color_t BORDER_COLOR = SETCOLOR_ARGB(192, 192, 192, 192); // border color
    const color_t BACK_COLOR = SETCOLOR_ARGB(255, 100, 100, 192);   // backdrop color
    const unsigned int BUTTON_WIDTH = (unsigned int)(FONT_HEIGHT * 4.5);
    const unsigned int BUTTON_HEIGHT = FONT_HEIGHT + 4;
    const int MAX_TYPE = 2;
    const int OK_CANCEL = 0;            // OK Cancel button type
    const int YES_NO = 1;               // Yes No button type
    static const char* BUTTON1_TEXT[MAX_TYPE] = { "OK", "YES" };
    static const char* BUTTON2_TEXT[MAX_TYPE] = { "CANCEL", "NO" };
    const unsigned char DIALOG_CLOSE_KEY = ENTER_KEY;    // Enter key
    const color_t BUTTON_COLOR = graphicsNS::GRAY;       // button background
    const color_t BUTTON_FONT_COLOR = graphicsNS::WHITE; // button text color
}

// Message Dialog
class MessageDialog
{
    // Dialog properties
protected:
    // System
    Graphics*   graphics;           // graphics system
    Input*      input;          // input system
    // View
    viewport_t  viewport3d;
    // Dialog
    TextSDL     font;           // SDL font
    vector2_t   offset;         // screen location
    vector2_t   extent;         // dialog width and height (height calculated in print())
    float       b, m;           // margin
    float       screenRatioX;
    float       screenRatioY;
    std::string text;           // dialog text
    rect_t      textRect;           // text rectangle
    rect_t      button1Rect;            // button rectangle
    rect_t      button2Rect;            // button2 rectangle
    color_t     fontColor;          // font color (a,r,g,b)
    color_t     borderColor;            // border color (a,r,g,b)
    color_t     backColor;          // background color (a,r,g,b)
    color_t     buttonColor;            // button color
    color_t     buttonFontColor;            // button font color
    VERTEX      dialogVerts[4];         // dialog vertex buffer
    VERTEX      borderVerts[4];         // border vertex buffer
    VERTEX      button1Verts[4];            // button vertex buffer
    VERTEX      button2Verts[4];            // button2 vertex buffer
    uint32_t    format;
    int         buttonClicked;          // which button was clicked (1 or 2)
    int         buttonType;         // 0 = OK/Cancel, 1 = Yes/No
    bool        initialized;            // true when initialized successfully
    bool        visible;            // true to display

public:
    // Constructor
    MessageDialog();

    // Destructor
    ~MessageDialog();

    // Initialize the MessageDialog.
    // Pre: *g points to Graphics object
    //      *in points to Input object
    //      hwnd = window handle
    bool initialize(Graphics* pGraphics, Input* pInput);

    // Prepare vertex buffers
    void prepareVerts();

    // Display the MessageDialog.
    const void draw();

    // Return button clicked
    // 0 = no button clicked
    // 1 is left button, 2 is right button
    int getButtonClicked();

    // Return visible.
    bool getVisible();

    // Set font color
    void setFontColor(color_t fc);

    // Set border color
    void setBorderColor(color_t bc);

    // Set background color
    void setBackColor(color_t bc);

    // Set button color
    void setButtonColor(color_t bc);

    // Set buitton font color
    void setButtonFontColor(color_t bfc);

    // Set visible;
    void setVisible(bool v);

    // Set button type 0 = OK/CANCEL, 1 = YES/NO
    void setButtonType(unsigned int t);

    // Display text str inside rectangle using format in MessageDialog
    void print(const std::string& str, rect_t& rect, unsigned int format);

    // Display text str at x, y in MessageDialog
    void print(const std::string& str, int x, int y);

    // Display text str in MessageDialog
    void print(const std::string& str);

    // Checks for Close event
    void update();
};

