#include "inputDialog.h"

//=============================================================================
// Constructor
//=============================================================================
InputDialog::InputDialog()
{
    textIn = "";
    SDL_memset(&inTextVerts, 0, 4 * sizeof(vector4_t));
    textBackColor = inputDialogNS::TEXT_BACK_COLOR;
    textFontColor = inputDialogNS::TEXT_COLOR;
    inTextRect = Rectangle();
    tempRect = Rectangle();
}

//=============================================================================
// Destructor
//=============================================================================
InputDialog::~InputDialog()
{
}

//=============================================================================
// Initialize the InputDialog
//=============================================================================
bool InputDialog::initialize(Graphics* pGraphics, Input* pInput)
{
    return MessageDialog::initialize(pGraphics, pInput);
}

//=============================================================================
// Prepare the vertex buffers for drawing dialog background and button
//=============================================================================
void InputDialog::prepareVerts()
{
    MessageDialog::prepareVerts();

    // inText top left
    inTextVerts[0].x = offset.x + b * 2;
    inTextVerts[0].y = offset.y + extent.y - b - m - messageDialogNS::BUTTON_HEIGHT * 2.5f;

    // inText top right
    inTextVerts[1].x = offset.x + extent.x - b * 2;
    inTextVerts[1].y = inTextVerts[0].y;

    // inText bottom right
    inTextVerts[2].x = inTextVerts[1].x;
    inTextVerts[2].y = inTextVerts[0].y + messageDialogNS::BUTTON_HEIGHT;

    // inText bottom left
    inTextVerts[3].x = inTextVerts[0].x;
    inTextVerts[3].y = inTextVerts[2].y;

    // set inTextRect
    inTextRect.min.x = inTextVerts[0].x;
    inTextRect.max.x = inTextVerts[2].x;
    inTextRect.min.y = inTextVerts[0].y;
    inTextRect.max.y = inTextVerts[2].y;
}

//=============================================================================
// Draw the InputDialog
//=============================================================================
const void InputDialog::draw()
{
    if (!visible || graphics == NULL || !initialized)
    {
        return;
    }

    graphics->drawQuad(borderVerts[0], borderVerts[1], borderVerts[2], borderVerts[3],
        borderColor);           // border
    graphics->drawQuad(dialogVerts[0], dialogVerts[1], dialogVerts[2], dialogVerts[3],
        backColor);         // backdrop
    graphics->drawQuad(button1Verts[0], button1Verts[1], button1Verts[2],
        button1Verts[3], buttonColor);          // button1
    graphics->drawQuad(button2Verts[0], button2Verts[1], button2Verts[2],
        button2Verts[3], buttonColor);          // button2
    graphics->drawQuad(inTextVerts[0], inTextVerts[1], inTextVerts[2], inTextVerts[3],
        textBackColor);         // input text area

    graphics->spriteBegin();

    if (text.size() == 0)
    {
        return;
    }

    // display text on MessageDialog
    font.setFontColor(fontColor);
    font.print(text, textRect, format);

    // display text on buttons
    font.setFontColor(buttonFontColor);
    font.print(messageDialogNS::BUTTON1_TEXT[buttonType], button1Rect,
        ALIGNMENT::SINGLELINE | ALIGNMENT::HCENTER | ALIGNMENT::VCENTER);
    font.print(messageDialogNS::BUTTON2_TEXT[buttonType], button2Rect,
        ALIGNMENT::SINGLELINE | ALIGNMENT::HCENTER | ALIGNMENT::VCENTER);

    // display input text
    font.setFontColor(textFontColor);
    tempRect = inTextRect;          // save
    // No text is printed with CALDRECT option. It moves rect.max.x
    font.print(textIn, tempRect, ALIGNMENT::SINGLELINE | ALIGNMENT::LEFT |
        ALIGNMENT::VCENTER | ALIGNMENT::CALCRECT);

    if (tempRect.max.x > inTextRect.max.x)            // if text too long, right justify
    {
        font.print(textIn, inTextRect, ALIGNMENT::SINGLELINE | ALIGNMENT::RIGHT |
            ALIGNMENT::VCENTER);
    }
    else            // else, left justify
    {
        font.print(textIn, inTextRect, ALIGNMENT::SINGLELINE | ALIGNMENT::LEFT |
            ALIGNMENT::VCENTER);
    }

    graphics->spriteEnd();
}

//=============================================================================
// Save the char just entered in textIn string
//=============================================================================
void InputDialog::keyIn(unsigned int key)
{
    if (input->wasKeyPressed(BACKSPACE_KEY) == true)            // backspace
    {
        if (textIn.length() > 0)
        {
            textIn.erase(textIn.size() - 1);
        }
    }
    else
    {
        // don't process console key
        if (key == '`')
        {
            return;
        }

        if (textIn.length() + 1 < 256)
        {
            textIn.insert(textIn.length(), 1, (key & 0xFF));
        }
    }
}

//=============================================================================
// Return input text.
//=============================================================================
std::string InputDialog::getText()
{
    if (!visible)
    {
        return textIn;
    }
    else
    {
        return "";
    }
}

//=============================================================================
// Set input text font color
//=============================================================================
void InputDialog::setTextFontColor(vector4_t fc)
{
    textFontColor = fc;
}

//=============================================================================
// Set input text background color
//=============================================================================
void InputDialog::setTextBackColor(vector4_t bc)
{
    textBackColor = bc;
}

//=============================================================================
// Checks for DIALOG_CLOSE_KEY and mouse click on buttons
//=============================================================================
void InputDialog::update()
{
    MessageDialog::update();

    if (!initialized || !visible)
    {
        if (buttonClicked == 2)         // cancel button
        {
            textIn = "";
        }

        return;
    }

    if (textIn.length() == 0)
    {
        return;
    }

    if (input->wasKeyPressed(BACKSPACE_KEY) == true)            // backspace
    {
        textIn.erase(textIn.size() - 1);
    }
}

//=============================================================================
// Set text string, rectangle and format, size dialog bottom to fit text and
// set visible = true
//=============================================================================
void InputDialog::print(const std::string& str, rect_t& rect, unsigned int format)
{
    if (!initialized || visible)
    {
        return;
    }

    offset.x = (float)(rect.min.x);
    offset.y = (float)(rect.min.y);
    extent.x = (float)(rect.max.x - rect.min.x);
    extent.y = (float)(rect.max.y - rect.min.y);

    text = str + "\n\n\n\n\n";          // leave some room for input text and buttons

    // set textRect to text area of dialog
    textRect.min.x = (offset.x + m);
    textRect.max.x = ((offset.x + extent.x) - m);
    textRect.min.y = (offset.y + m);
    textRect.max.y = ((offset.y + extent.y) - m);

    // set textRect.max.y to precise height required for text
    // no text is printed with CALDRECT option.
    this->format = format;
    font.print(text, textRect, format | ALIGNMENT::CALCRECT);
    extent.y = (textRect.max.y - textRect.min.y) - (float)(b + m);

    InputDialog::prepareVerts();

    textIn = "";                    // clear old input
    buttonClicked = 0;              // clear buttonClicked
    visible = true;
}

//=============================================================================
// Set text string, x and y, size dialog bottom to fit text and
// set visible = true
//=============================================================================
void InputDialog::print(const std::string& str, int x, int y)
{
    rect_t rect = {};            // text rectangle

    rect.min.x = (float)(x);
    rect.max.x = (float)(x + messageDialogNS::W);
    rect.min.y = (float)(y);
    rect.max.y = (float)(y + messageDialogNS::H);

    InputDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}

//=============================================================================
// Set text string, size dialog bottom to fit text and set visible = true
//=============================================================================
void InputDialog::print(const std::string& str)
{
    rect_t rect = {};            // text rectangle

    viewport3d = graphics->get3DViewport();

    rect.min.x = (((viewport3d.w - viewport3d.x) / 2) - (messageDialogNS::X));
    rect.max.x = (((viewport3d.w - viewport3d.x) / 2) - (messageDialogNS::X) +
        messageDialogNS::W);
    rect.min.y = (((viewport3d.h - viewport3d.y) / 4) - (messageDialogNS::Y));
    rect.max.y = (((viewport3d.h - viewport3d.y) / 4) - (messageDialogNS::Y) +
        messageDialogNS::H);

    InputDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}
