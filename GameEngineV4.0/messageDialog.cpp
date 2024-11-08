#include "messageDialog.h"

//=============================================================================
// Constructor
//=============================================================================
MessageDialog::MessageDialog()
{
    // System
    graphics = NULL;
    input = NULL;
    // View
    viewport3d = Viewport();
    // Dialog
    offset.x = messageDialogNS::X;
    offset.y = messageDialogNS::Y;
    extent.x = messageDialogNS::H;
    extent.y = messageDialogNS::W;
    b = messageDialogNS::BORDER;
    m = messageDialogNS::MARGIN;
    screenRatioX = 1.0f;
    screenRatioY = 1.0f;
    text = "";
    textRect = Rectangle();
    button1Rect = Rectangle();
    button2Rect = Rectangle();
    fontColor = messageDialogNS::FONT_COLOR;
    borderColor = messageDialogNS::BORDER_COLOR;
    backColor = messageDialogNS::BACK_COLOR;
    buttonColor = messageDialogNS::BUTTON_COLOR;
    buttonFontColor = messageDialogNS::BUTTON_FONT_COLOR;
    SDL_memset(&dialogVerts,  0, 4 * sizeof(VERTEX));
    SDL_memset(&borderVerts,  0, 4 * sizeof(VERTEX));
    SDL_memset(&button1Verts, 0, 4 * sizeof(VERTEX));
    SDL_memset(&button2Verts, 0, 4 * sizeof(VERTEX));
    format = TOP | LEFT;
    buttonClicked = 0;
    buttonType = 0;     // OK/Cancel
    initialized = false;
    visible = false;
}

//=============================================================================
// Destructor
//=============================================================================
MessageDialog::~MessageDialog()
{
    font.deleteAll();
}

//=============================================================================
// Initialize the MessageDialog
//=============================================================================
bool MessageDialog::initialize(Graphics* pGraphics, Input* pInput)
{
    graphics = pGraphics;
    input = pInput;

    // initialize SDL font
    if (font.initialize(graphics, messageDialogNS::FONT_HEIGHT, false,
        false, messageDialogNS::FONT) == false)
    {
        return false;
    }

    font.setFontColor(fontColor);

    viewport3d = graphics->get3DViewport();
    offset.x = ((float)(viewport3d.w - viewport3d.x) / 2) - (float)(messageDialogNS::X);
    offset.y = ((float)(viewport3d.h - viewport3d.y) / 4) - (float)(messageDialogNS::Y);

    initialized = true;

    return true;
}

//=============================================================================
// Prepare the vertex buffers for drawing dialog background and buttons
//=============================================================================
void MessageDialog::prepareVerts()
{
    // border top left
    borderVerts[0].position.x = offset.x;
    borderVerts[0].position.y = offset.y;
    borderVerts[0].color = borderColor;

    // border top right
    borderVerts[1].position.x = offset.x + extent.x;
    borderVerts[1].position.y = offset.y;
    borderVerts[1].color = borderColor;

    // border bottom right
    borderVerts[2].position.x = offset.x + extent.x;
    borderVerts[2].position.y = offset.y + extent.y;
    borderVerts[2].color = borderColor;

    // border bottom left
    borderVerts[3].position.x = offset.x;
    borderVerts[3].position.y = offset.y + extent.y;
    borderVerts[3].color = borderColor;

    // background top left
    dialogVerts[0].position.x = offset.x + b;
    dialogVerts[0].position.y = offset.y + b;
    dialogVerts[0].color = backColor;

    // background top right
    dialogVerts[1].position.x = offset.x + extent.x - b;
    dialogVerts[1].position.y = offset.y + b;
    dialogVerts[1].color = backColor;

    // background bottom right
    dialogVerts[2].position.x = offset.x + extent.x - b;
    dialogVerts[2].position.y = offset.y + extent.y - b;
    dialogVerts[2].color = backColor;

    // background bottom left
    dialogVerts[3].position.x = offset.x + b;
    dialogVerts[3].position.y = offset.y + extent.y - b;
    dialogVerts[3].color = backColor;

    // button top left
    button1Verts[0].position.x = offset.x + (extent.x / 2.0f) - (messageDialogNS::BUTTON_WIDTH / 2.0f) - m;
    button1Verts[0].position.y = offset.y + extent.y - b - m - messageDialogNS::BUTTON_HEIGHT;
    button1Verts[0].color = buttonColor;

    // button top right
    button1Verts[1].position.x = offset.x + (extent.x / 2.0f) + (messageDialogNS::BUTTON_WIDTH / 2.0f);
    button1Verts[1].position.y = button1Verts[0].position.y;
    button1Verts[1].color = buttonColor;

    // button bottom right
    button1Verts[2].position.x = button1Verts[1].position.x;
    button1Verts[2].position.y = button1Verts[0].position.y + messageDialogNS::BUTTON_HEIGHT;
    button1Verts[2].color = buttonColor;

    // button bottom left
    button1Verts[3].position.x = button1Verts[0].position.x;
    button1Verts[3].position.y = button1Verts[2].position.y;
    button1Verts[3].color = buttonColor;

    // set buttonRect
    button1Rect.min[0] = (long)button1Verts[0].position.x;
    button1Rect.max[0] = (long)button1Verts[2].position.x;
    button1Rect.min[1] = (long)button1Verts[0].position.y;
    button1Rect.max[1] = (long)button1Verts[2].position.y;

    // button2 top left
    button2Verts[0].position.x = offset.x + extent.x - messageDialogNS::BUTTON_WIDTH * 1.2f;
    button2Verts[0].position.y = offset.y + extent.y - b - m - messageDialogNS::BUTTON_HEIGHT;
    button2Verts[0].color = buttonColor;

    // button2 top right
    button2Verts[1].position.x = button2Verts[0].position.x + messageDialogNS::BUTTON_WIDTH;
    button2Verts[1].position.y = button2Verts[0].position.y;
    button2Verts[1].color = buttonColor;

    // button2 bottom right
    button2Verts[2].position.x = button2Verts[1].position.x;
    button2Verts[2].position.y = button2Verts[0].position.y + messageDialogNS::BUTTON_HEIGHT;
    button2Verts[2].color = buttonColor;

    // button2 bottom left
    button2Verts[3].position.x = button2Verts[0].position.x;
    button2Verts[3].position.y = button2Verts[2].position.y;
    button2Verts[3].color = buttonColor;

    // set button2Rect
    button2Rect.min[0] = (long)button2Verts[0].position.x;
    button2Rect.max[0] = (long)button2Verts[2].position.x;
    button2Rect.min[1] = (long)button2Verts[0].position.y;
    button2Rect.max[1] = (long)button2Verts[2].position.y;
}

//=============================================================================
// Draw the MessageDialog
//=============================================================================
const void MessageDialog::draw()
{
    if (!visible || graphics == NULL || !initialized)
    {
        return;
    }

    graphics->drawQuad(borderVerts[0].position, borderVerts[1].position,
        borderVerts[2].position, borderVerts[3].position, borderColor);         // draw border
    graphics->drawQuad(dialogVerts[0].position, dialogVerts[1].position,
        dialogVerts[2].position, dialogVerts[3].position, backColor);           // draw backdrop
    graphics->drawQuad(button1Verts[0].position, button1Verts[1].position,
        button1Verts[2].position, button1Verts[3].position, buttonColor);           // draw button1
    graphics->drawQuad(button2Verts[0].position, button2Verts[1].position,
        button2Verts[2].position, button2Verts[3].position, buttonColor);           // draw button2

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

    graphics->spriteEnd();
}

//=============================================================================
// Return button clicked
// 0 = no button clicked
// 1 is left button, 2 is right button
//=============================================================================
int MessageDialog::getButtonClicked() const
{
    return buttonClicked;
}

//=============================================================================
// Return visible.
//=============================================================================
bool MessageDialog::getVisible() const
{
    return visible;
}

//=============================================================================
// Set font color
//=============================================================================
void MessageDialog::setFontColor(color_t fc)
{
    fontColor = fc;
}

//=============================================================================
// Set border color
//=============================================================================
void MessageDialog::setBorderColor(color_t bc)
{
    borderColor = bc;
}

//=============================================================================
// Set background color
//=============================================================================
void MessageDialog::setBackColor(color_t bc)
{
    backColor = bc;
}

//=============================================================================
// Set button color
//=============================================================================
void MessageDialog::setButtonColor(color_t bc)
{
    buttonColor = bc;
}

//=============================================================================
// Set buitton font color
//=============================================================================
void MessageDialog::setButtonFontColor(color_t bfc)
{
    buttonFontColor = bfc;
}

//=============================================================================
// Set visible;
//=============================================================================
void MessageDialog::setVisible(bool v)
{
    visible = v;
}

//=============================================================================
// Set button type 0 = OK/CANCEL, 1 = YES/NO
//=============================================================================
void MessageDialog::setButtonType(unsigned int t)
{
    if (t < messageDialogNS::MAX_TYPE)
    {
        buttonType = t;
    }
}

//=============================================================================
// Checks for DIALOG_CLOSE_KEY and mouse click on OK button
//=============================================================================
void MessageDialog::update()
{
    if (!initialized || !visible)
    {
        return;
    }

    if (input->wasKeyPressed(messageDialogNS::DIALOG_CLOSE_KEY))
    {
        visible = false;
        buttonClicked = 1;          // button1 was clicked

        return;
    }

    if (graphics->getFullscreen() == false)
    {
        // calculate screen ratios incase window was resized
        SDL_Rect clientRect = graphics->getWindowRect();
        screenRatioX = (float)graphics->getWidth()  / clientRect.w;
        screenRatioY = (float)graphics->getHeight() / clientRect.h;

    }

    // mouse
    if (input->getMouseLButton() == true && input->wasMouseButtonPressed(MOUSE_L_BUTTON) == true)
    {
        // if mouse clicked inside button1 (OK)
        if (input->getMouseX() * screenRatioX >= button1Rect.min[0] &&
            input->getMouseX() * screenRatioX <= button1Rect.max[0] &&
            input->getMouseY() * screenRatioY >= button1Rect.min[1] &&
            input->getMouseY() * screenRatioY <= button1Rect.max[1])
        {
            visible = false;            // hide message dialog
            buttonClicked = 1;          // button1 was clicked
            return;
        }

        // if mouse clicked inside button2 (cancel)
        if (input->getMouseX() * screenRatioX >= button2Rect.min[0] &&
            input->getMouseX() * screenRatioX <= button2Rect.max[0] &&
            input->getMouseY() * screenRatioY >= button2Rect.min[1] &&
            input->getMouseY() * screenRatioY <= button2Rect.max[1])
        {
            visible = false;            // hide message dialog
            buttonClicked = 2;          // button2 was clicked
        }
    }

    // controller
    for (unsigned int i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (input->getGamepadA(i) == true)
        {
            visible = false;
            buttonClicked = 1;

            return;
        }

        if (input->getGamepadB(i) == true)
        {
            visible = false;
            buttonClicked = 2;
        }
    }
}

//=============================================================================
// Set text string, rectangle and format, size dialog bottom to fit text and
// set visible = true
//=============================================================================
void MessageDialog::print(const std::string& str, rect_t& rect, unsigned int format)
{
    if (!initialized || visible)
    {
        return;
    }

    offset.x = (float)(rect.min[0]);
    offset.y = (float)(rect.min[1]);
    extent.x = (float)(rect.max[0] - rect.min[0]);
    extent.y = (float)(rect.max[1] - rect.min[1]);

    text = str + "\n\n\n\n";        // leave some room for buttons

    // Set textRect to text area of dialog
    textRect.min[0] = (long)(offset.x + m);
    textRect.max[0] = (long)((offset.x + extent.x) - m);
    textRect.min[1] = (long)(offset.y + m);
    textRect.max[1] = (long)((offset.y + extent.y) - m);

    // set rect.min[1] to precise height required for text
    // no text is printed with CALDRECT option.
    this->format = format;
    font.print(text, textRect, format | ALIGNMENT::CALCRECT);
    extent.y = (textRect.max[1] - textRect.min[1]) - (float)(b + m);

    prepareVerts();

    buttonClicked = 0;              // clear buttonClicked
    visible = true;
}

//=============================================================================
// Set text string, x and y, size dialog bottom to fit text and
// set visible = true
//=============================================================================
void MessageDialog::print(const std::string& str, int x, int y)
{
    rect_t rect = {};            // text rectangle

    rect.min[0] = (long)(x);
    rect.max[0] = (long)(x + messageDialogNS::W);
    rect.min[1] = (long)(y);
    rect.max[1] = (long)(y + messageDialogNS::H);

    MessageDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}

//=============================================================================
// Set text string, size dialog bottom to fit text and set visible = true
//=============================================================================
void MessageDialog::print(const std::string& str)
{
    rect_t rect = {};            // text rectangle

    viewport3d = graphics->get3DViewport();

    rect.min[0] = (long)((float)((viewport3d.w - viewport3d.x) / 2) -
        (float)(messageDialogNS::X));
    rect.max[0] = (long)((float)((viewport3d.w - viewport3d.x) / 2) -
        (float)(messageDialogNS::X)+ messageDialogNS::W);
    rect.min[1] = (long)((float)((viewport3d.h - viewport3d.y) / 4) -
        (float)(messageDialogNS::Y));
    rect.max[1] = (long)((float)((viewport3d.h - viewport3d.y) / 4) -
        (float)(messageDialogNS::Y)+ messageDialogNS::H);

    MessageDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}
