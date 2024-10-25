#pragma once
#include "constants.h"
#include "messageDialog.h"

namespace inputDialogNS
{
    const color_t TEXT_BACK_COLOR = graphicsNS::WHITE;   // input text background
    const color_t TEXT_COLOR = graphicsNS::BLACK;        // input text color
}

// Input Dialog, inherits from Message Dialog
class InputDialog : public MessageDialog
{
    // Dialog properties
private:
    // Dialog
    std::string textIn;         // input text
    VERTEX  inTextVerts[4];            // text area vertex buffer
    color_t textBackColor;          // text area background color
    color_t textFontColor;          // text area font color
    rect_t  inTextRect;
    rect_t  tempRect;

public:
    // Constructor
    InputDialog();

    // Destructor
    ~InputDialog();

    // Initialize the MessageDialog.
    // Pre: pGraphics points to Graphics object
    //      pInput points to Input object
    bool initialize(Graphics* pGraphics, Input* pInput);

    // Prepare vertex buffers
    void prepareVerts();

    // Display the InputDialog.
    const void draw();

    // Save the char just entered in textIn string
    void keyIn(unsigned int key);

    // Return input text.
    std::string getText();

    // Set input text font color
    void setTextFontColor(color_t fc);

    // Set input text background color
    void setTextBackColor(color_t bc);

    // Display text str inside rectangle using format in MessageDialog
    void print(const std::string& str, rect_t& rect, unsigned int format);

    // Display text str at x, y in InputDialog
    void print(const std::string& str, int x, int y);

    // Display text str in InputDialog
    void print(const std::string& str);

    // Checks for Close event
    void update();
};

