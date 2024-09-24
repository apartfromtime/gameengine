#include "textSDL.h"

//=============================================================================
// default constructor
//=============================================================================
TextSDL::TextSDL()
{
    // System
    graphics = NULL;
    // View
    viewport3d = Viewport();
    // Font
    font = NULL;
    fontColor = graphicsNS::WHITE;
    angle = 0;
    fontName = "";
    height = 0;
}

//=============================================================================
// destructor
//=============================================================================
TextSDL::~TextSDL()
{
}

//=============================================================================
// Create SDL Font
//=============================================================================
bool TextSDL::initialize(Graphics* pGraphics, int height, bool bold,
    bool italic, const std::string& fileName)
{
    if (pGraphics == NULL || fileName == "")
    {
        return false;
    }

    graphics = pGraphics;
    fontName = "fonts\\" + fileName;
    this->height = height;

    // create SDL font
    if (Create(graphics->get2DRenderer(), height, bold, italic,
        DEFAULT_QUALITY, fontName.c_str(), &font) == false)
    {
        return false;
    }

    viewport3d = graphics->get3DViewport();

    return true;
}

//=============================================================================
// Print text inside rectangle using SDL text format
//=============================================================================
int TextSDL::print(const std::string& str, rect_t& rect, unsigned int format)
{
    if (graphics == NULL)
    {
        return 0;
    }

    // Setup matrix to not rotate text
    const matrix4_t matrix = Transformation2DMatrix4(Vector2(),
        Vector2(1.0f, 1.0f), Vector2(), 0.0f, Vector2());
    graphics->getSprite()->SetTransform(&matrix);

    return font->DrawText(graphics->getSprite(), str.c_str(), (int)str.length(),
        &rect, format, fontColor);
}

//=============================================================================
// Print text at x, y.
//=============================================================================
int TextSDL::print(const std::string& str, int x, int y)
{
    if (graphics == NULL)
    {
        return 0;
    }

    rect_t rect;            // text rectangle
    vector2_t rCenter = Vector2((float)x, (float)y);
    unsigned int format = TOP | LEFT;

    // Setup matrix to rotate text by angle
    const matrix4_t matrix = Transformation2DMatrix4(Vector2(),
        Vector2(1.0f, 1.0f), rCenter, angle, Vector2());
    graphics->getSprite()->SetTransform(&matrix);

    viewport3d = graphics->get3DViewport();
    rect.min[0] = x;
    rect.min[1] = y;
    rect.max[0] = (viewport3d.w - viewport3d.x);
    rect.max[1] = (viewport3d.h - viewport3d.y);

    return font->DrawText(graphics->getSprite(), str.c_str(), (int)str.length(),
        &rect, format, fontColor);
}

//=============================================================================
// Return rotation angle in degrees.
//=============================================================================
float TextSDL::getDegrees()
{
    return RAD2DEG(angle);
}

//=============================================================================
// Return rotation angle in radians.
//=============================================================================
float TextSDL::getRadians()
{
    return angle;
}

//=============================================================================
// Returns font color
//=============================================================================
COLOR_ARGB TextSDL::getFontColor()
{
    return fontColor;
}

//=============================================================================
// Set rotation angle in degrees.
//=============================================================================
void TextSDL::setDegrees(float deg)
{
    angle = DEG2RAD(deg);
}

//=============================================================================
// Set rotation angle in radians.
//=============================================================================
void TextSDL::setRadians(float rad)
{
    angle = rad;
}

//=============================================================================
// Set the font color.
//=============================================================================
void TextSDL::setFontColor(COLOR_ARGB c)
{
    fontColor = c;
}

//=============================================================================
// Delete all reserved memory.
//=============================================================================
void TextSDL::deleteAll()
{
    safeDelete(font);
}