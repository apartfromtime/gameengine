#pragma once
#include "constants.h"
#include "graphics.h"
#include "font.h"

class TextSDL
{
    // SDL Text properties
private:
    // System
    Graphics*   graphics;
    // View
    viewport_t  viewport3d;
    // Font
    LP_FONT     font;
    std::string fontName;
    COLOR_ARGB  fontColor;
    float       angle;
    int         height;

public:
    // Constructor
    TextSDL();

    // Destructor
    ~TextSDL();

    // Initialize font
    bool initialize(Graphics* pGraphics, int height, bool bold, bool italic,
        const std::string& fontName);

    // Print at x, y.
    // Return 0 on fail, height of text on success
    int print(const std::string& str, int x, int y);

    // Print inside rect using format.
    // Return 0 on fail, height of text on success
    int print(const std::string& str, rect_t& rect, unsigned int format);

    // Return rotation angle in degrees.
    float getDegrees();

    // Return rotation angle in radians.
    float getRadians();

    // Returns font color
    COLOR_ARGB getFontColor();

    // Set rotation angle in degrees.
    // 0 degrees is up. Angles progress clockwise.
    void setDegrees(float deg);

    // Set rotation angle in radians.
    // 0 radians is up. Angles progress clockwise.
    void setRadians(float rad);

    // Set the font color. Use SETCOLOR_ARGB macro or colors from graphicsNS::
    void setFontColor(COLOR_ARGB c);

    // Delete all reserved memory.
    void deleteAll();
};

