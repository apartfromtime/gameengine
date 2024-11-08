#include "inputDialog.h"

//=============================================================================
// Constructor
//=============================================================================
InputDialog::InputDialog()
{
    textIn = "";
    SDL_memset(&inTextVerts, 0, 4 * sizeof(VERTEX));
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
    inTextVerts[0].position.x = offset.x + b * 2;
    inTextVerts[0].position.y = offset.y + extent.y - b - m - messageDialogNS::BUTTON_HEIGHT * 2.5f;
    inTextVerts[0].color = textBackColor;

    // inText top right
    inTextVerts[1].position.x = offset.x + extent.x - b * 2;
    inTextVerts[1].position.y = inTextVerts[0].position.y;
    inTextVerts[1].color = textBackColor;

    // inText bottom right
    inTextVerts[2].position.x = inTextVerts[1].position.x;
    inTextVerts[2].position.y = inTextVerts[0].position.y +
        messageDialogNS::BUTTON_HEIGHT;
    inTextVerts[2].color = textBackColor;

    // inText bottom left
    inTextVerts[3].position.x = inTextVerts[0].position.x;
    inTextVerts[3].position.y = inTextVerts[2].position.y;
    inTextVerts[3].color = textBackColor;

    // set inTextRect
    inTextRect.min[0] = (long)inTextVerts[0].position.x;
    inTextRect.max[0] = (long)inTextVerts[2].position.x;
    inTextRect.min[1] = (long)inTextVerts[0].position.y;
    inTextRect.max[1] = (long)inTextVerts[2].position.y;
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

    graphics->drawQuad(borderVerts[0].position, borderVerts[1].position,
        borderVerts[2].position, borderVerts[3].position, borderColor);         // border
    graphics->drawQuad(dialogVerts[0].position, dialogVerts[1].position,
        dialogVerts[2].position, dialogVerts[3].position, backColor);           // backdrop
    graphics->drawQuad(button1Verts[0].position, button1Verts[1].position,
        button1Verts[2].position, button1Verts[3].position, buttonColor);       // button1
    graphics->drawQuad(button2Verts[0].position, button2Verts[1].position,
        button2Verts[2].position, button2Verts[3].position, buttonColor);       // button2
    graphics->drawQuad(inTextVerts[0].position, inTextVerts[1].position,
        inTextVerts[2].position, inTextVerts[3].position, textBackColor);       // input text area

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
    // No text is printed with CALDRECT option. It moves rect.max[0]
    font.print(textIn, tempRect, ALIGNMENT::SINGLELINE | ALIGNMENT::LEFT |
        ALIGNMENT::VCENTER | ALIGNMENT::CALCRECT);

    if (tempRect.max[0] > inTextRect.max[0])            // if text too long, right justify
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
void InputDialog::setTextFontColor(color_t fc)
{
    textFontColor = fc;
}

//=============================================================================
// Set input text background color
//=============================================================================
void InputDialog::setTextBackColor(color_t bc)
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

    offset.x = (float)(rect.min[0]);
    offset.y = (float)(rect.min[1]);
    extent.x = (float)(rect.max[0] - rect.min[0]);
    extent.y = (float)(rect.max[1] - rect.min[1]);

    text = str + "\n\n\n\n\n";          // leave some room for input text and buttons

    // set textRect to text area of dialog
    textRect.min[0] = (long)(offset.x + m);
    textRect.max[0] = (long)((offset.x + extent.x) - m);
    textRect.min[1] = (long)(offset.y + m);
    textRect.max[1] = (long)((offset.y + extent.y) - m);

    // set textRect.max[1] to precise height required for text
    // no text is printed with CALDRECT option.
    this->format = format;
    font.print(text, textRect, format | ALIGNMENT::CALCRECT);
    extent.y = (textRect.max[1] - textRect.min[1]) - (float)(b + m);

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

    rect.min[0] = (long)(x);
    rect.max[0] = (long)(x + messageDialogNS::W);
    rect.min[1] = (long)(y);
    rect.max[1] = (long)(y + messageDialogNS::H);

    InputDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}

//=============================================================================
// Set text string, size dialog bottom to fit text and set visible = true
//=============================================================================
void InputDialog::print(const std::string& str)
{
    rect_t rect = {};            // text rectangle

    viewport3d = graphics->get3DViewport();

    rect.min[0] = (long)((float)((viewport3d.w - viewport3d.x) / 2) -
        (float)(messageDialogNS::X));
    rect.max[0] = (long)((float)((viewport3d.w - viewport3d.x) / 2) -
        (float)(messageDialogNS::X)+messageDialogNS::W);
    rect.min[1] = (long)((float)((viewport3d.h - viewport3d.y) / 4) -
        (float)(messageDialogNS::Y));
    rect.max[1] = (long)((float)((viewport3d.h - viewport3d.y) / 4) -
        (float)(messageDialogNS::Y)+messageDialogNS::H);

    InputDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}
