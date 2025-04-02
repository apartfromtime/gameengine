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
    viewport3d = Viewport3d();
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
    SDL_memset(&dialogVerts,  0, 4 * sizeof(vector4_t));
    SDL_memset(&borderVerts,  0, 4 * sizeof(vector4_t));
    SDL_memset(&button1Verts, 0, 4 * sizeof(vector4_t));
    SDL_memset(&button2Verts, 0, 4 * sizeof(vector4_t));
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
    borderVerts[0].x = offset.x;
    borderVerts[0].y = offset.y;

    // border top right
    borderVerts[1].x = offset.x + extent.x;
    borderVerts[1].y = offset.y;

    // border bottom right
    borderVerts[2].x = offset.x + extent.x;
    borderVerts[2].y = offset.y + extent.y;

    // border bottom left
    borderVerts[3].x = offset.x;
    borderVerts[3].y = offset.y + extent.y;

    // background top left
    dialogVerts[0].x = offset.x + b;
    dialogVerts[0].y = offset.y + b;

    // background top right
    dialogVerts[1].x = offset.x + extent.x - b;
    dialogVerts[1].y = offset.y + b;

    // background bottom right
    dialogVerts[2].x = offset.x + extent.x - b;
    dialogVerts[2].y = offset.y + extent.y - b;

    // background bottom left
    dialogVerts[3].x = offset.x + b;
    dialogVerts[3].y = offset.y + extent.y - b;

    // button top left
    button1Verts[0].x = offset.x + (extent.x / 2.0f) - (messageDialogNS::BUTTON_WIDTH / 2.0f) - m;
    button1Verts[0].y = offset.y + extent.y - b - m - messageDialogNS::BUTTON_HEIGHT;

    // button top right
    button1Verts[1].x = offset.x + (extent.x / 2.0f) + (messageDialogNS::BUTTON_WIDTH / 2.0f);
    button1Verts[1].y = button1Verts[0].y;

    // button bottom right
    button1Verts[2].x = button1Verts[1].x;
    button1Verts[2].y = button1Verts[0].y + messageDialogNS::BUTTON_HEIGHT;

    // button bottom left
    button1Verts[3].x = button1Verts[0].x;
    button1Verts[3].y = button1Verts[2].y;

    // set buttonRect
    button1Rect.min.x = button1Verts[0].x;
    button1Rect.max.x = button1Verts[2].x;
    button1Rect.min.y = button1Verts[0].y;
    button1Rect.max.y = button1Verts[2].y;

    // button2 top left
    button2Verts[0].x = offset.x + extent.x - messageDialogNS::BUTTON_WIDTH * 1.2f;
    button2Verts[0].y = offset.y + extent.y - b - m - messageDialogNS::BUTTON_HEIGHT;

    // button2 top right
    button2Verts[1].x = button2Verts[0].x + messageDialogNS::BUTTON_WIDTH;
    button2Verts[1].y = button2Verts[0].y;

    // button2 bottom right
    button2Verts[2].x = button2Verts[1].x;
    button2Verts[2].y = button2Verts[0].y + messageDialogNS::BUTTON_HEIGHT;

    // button2 bottom left
    button2Verts[3].x = button2Verts[0].x;
    button2Verts[3].y = button2Verts[2].y;

    // set button2Rect
    button2Rect.min.x = button2Verts[0].x;
    button2Rect.max.x = button2Verts[2].x;
    button2Rect.min.y = button2Verts[0].y;
    button2Rect.max.y = button2Verts[2].y;
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

    graphics->drawQuad(borderVerts[0], borderVerts[1], borderVerts[2], borderVerts[3],
        borderColor);           // draw border
    graphics->drawQuad(dialogVerts[0], dialogVerts[1], dialogVerts[2], dialogVerts[3],
        backColor);         // draw backdrop
    graphics->drawQuad(button1Verts[0], button1Verts[1], button1Verts[2],
        button1Verts[3], buttonColor);          // draw button1
    graphics->drawQuad(button2Verts[0], button2Verts[1], button2Verts[2],
        button2Verts[3], buttonColor);          // draw button2

    graphics->spriteBegin(SPRITE_ALPHABLEND);

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
        if (input->getMouseX() * screenRatioX >= button1Rect.min.x &&
            input->getMouseX() * screenRatioX <= button1Rect.max.x &&
            input->getMouseY() * screenRatioY >= button1Rect.min.y &&
            input->getMouseY() * screenRatioY <= button1Rect.max.y)
        {
            visible = false;            // hide message dialog
            buttonClicked = 1;          // button1 was clicked
            return;
        }

        // if mouse clicked inside button2 (cancel)
        if (input->getMouseX() * screenRatioX >= button2Rect.min.x &&
            input->getMouseX() * screenRatioX <= button2Rect.max.x &&
            input->getMouseY() * screenRatioY >= button2Rect.min.y &&
            input->getMouseY() * screenRatioY <= button2Rect.max.y)
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

    offset.x = rect.min.x;
    offset.y = rect.min.y;
    extent.x = rect.max.x - rect.min.x;
    extent.y = rect.max.y - rect.min.y;

    text = str + "\n\n\n\n";        // leave some room for buttons

    // Set textRect to text area of dialog
    textRect.min.x = offset.x + m;
    textRect.max.x = (offset.x + extent.x) - m;
    textRect.min.y = offset.y + m;
    textRect.max.y = (offset.y + extent.y) - m;

    // set rect.min.y to precise height required for text
    // no text is printed with CALDRECT option.
    this->format = format;
    font.print(text, textRect, format | ALIGNMENT::CALCRECT);
    extent.y = (textRect.max.y - textRect.min.y) - (b + m);

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

    rect.min.x = (float)(x);
    rect.max.x = (float)(x + messageDialogNS::W);
    rect.min.y = (float)(y);
    rect.max.y = (float)(y + messageDialogNS::H);

    MessageDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}

//=============================================================================
// Set text string, size dialog bottom to fit text and set visible = true
//=============================================================================
void MessageDialog::print(const std::string& str)
{
    rect_t rect = {};            // text rectangle

    viewport3d = graphics->get3DViewport();

    rect.min.x = (((viewport3d.w - viewport3d.x) / 2) - (messageDialogNS::X));
    rect.max.x = (((viewport3d.w - viewport3d.x) / 2) - (messageDialogNS::X) +
        messageDialogNS::W);
    rect.min.y = (((viewport3d.h - viewport3d.y) / 4) - (messageDialogNS::Y));
    rect.max.y = (((viewport3d.h - viewport3d.y) / 4) - (messageDialogNS::Y) +
        messageDialogNS::H);

    MessageDialog::print(str, rect, ALIGNMENT::HCENTER | ALIGNMENT::WORDBOUNDS);
}
