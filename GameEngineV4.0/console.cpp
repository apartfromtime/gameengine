#include "console.h"

//=============================================================================
// Constructor
//=============================================================================
Console::Console()
{
    initialized = false;            // set true when successfully initialized
    graphics = NULL;
    input = NULL;
    visible = false;            // not visible
    fontColor = consoleNS::FONT_COLOR;
    backColor = consoleNS::BACK_COLOR;

    x = consoleNS::X;           // starting console position
    y = consoleNS::Y;
    w = consoleNS::W;
    h = consoleNS::H;
    b = consoleNS::BORDER;
    m = consoleNS::MARGIN;

    textRect.max[1] = (int)((y + h) - m);
    textRect.min[0] = (int)(x + m);
    textRect.max[0] = (int)((x + w) - m);
    textRect.min[1] = (int)(y + m);

    SDL_memset(&vtx, 0, 4 * sizeof(VERTEX));

    textIn = "";
    rows = 0;
    scrollAmount = 0;
}

//=============================================================================
// Destructor
//=============================================================================
Console::~Console()
{
    font.deleteAll();
}

//=============================================================================
// Initialize the console
//=============================================================================
bool Console::initialize(Graphics* pGraphics, Input* pInput)
{
    graphics = pGraphics;
    input = pInput;

    // top left
    vtx[0].position.x = x;
    vtx[0].position.y = y;
    vtx[0].color = backColor;

    // top right
    vtx[1].position.x = x + w;
    vtx[1].position.y = y;
    vtx[1].color = backColor;

    // bottom right
    vtx[2].position.x = x + w;
    vtx[2].position.y = y + h;
    vtx[2].color = backColor;

    // bottom left
    vtx[3].position.x = x;
    vtx[3].position.y = y + h;
    vtx[3].color = backColor;

    // initialize SDL font
    if (font.initialize(graphics, consoleNS::FONT_HEIGHT, false, false,
        consoleNS::FONT) == false)
    {
        return false;
    }

    font.setFontColor(fontColor);

    initialized = true;

    return true;
}

//=============================================================================
// draw console
// Pre: Inside BeginScene/EndScene
//=============================================================================
void Console::draw()
{
    if (!visible || graphics == NULL || !initialized)
    {
        return;
    }

    graphics->drawQuad(vtx[0].position, vtx[1].position, vtx[2].position,
        vtx[3].position, backColor);            // draw backdrop

    if (text.size() == 0)
    {
        return;
    }

    graphics->spriteBegin();

    // display text on console
    textRect.min[0] = 0;
    textRect.min[1] = 0;

    // sets textRect bottom to height of 1 row
    font.print("|", textRect, ALIGNMENT::CALCRECT);
    int rowHeight = (textRect.max[1] - textRect.min[1]) + 2;         // height of 1 row (+2 is row spacing)

    if (rowHeight <= 0)         // this should never be true
    {
        rowHeight = 20;         // force a workable result
    }

    // number of rows that will fit on console
    rows = (int)((h - (2 * m)) / (float)rowHeight);
    rows -= 2;          // room for input prompt at bottom

    if (rows <= 0)          // this should never be true
    {
        rows = 5;           // force a workable result
    }

    // set text display rect for one row
    textRect.min[0] = (long)(x + m);
    textRect.max[0] = (long)(x + (w - m));
    // -2*rowHeight is room for input prompt
    textRect.max[1] = (long)(y + ((h - (2 * m)) - (2 * rowHeight)));
    textRect.min[1] = (long)(textRect.max[1] - rowHeight);

    // for all rows (max text.size()) from bottom to top
    for (int r = scrollAmount; r < rows + scrollAmount && r < (int)(text.size()); r++)
    {
        // set text display rect top for this row
        textRect.min[1] = (long)(textRect.max[1] - rowHeight);
        // display one row of text
        font.print(text[r], textRect, ALIGNMENT::LEFT);
        // adjust text display rect bottom for next row
        textRect.max[1] -= rowHeight;
    }

    // display command prompt and current command string
    // set text display rect for prompt
    textRect.max[1] = (long)(y + (h - m));
    textRect.min[1] = (long)(textRect.max[1] - rowHeight);
    std::string prompt = ">";           // build prompt string
    prompt += textIn;
    font.print(prompt, textRect, ALIGNMENT::LEFT);         // display prompt and command

    graphics->spriteEnd();
}

//=============================================================================
// show/hide console
//=============================================================================
void Console::showHide()
{
    if (!initialized)
    {
        return;
    }

    visible = !visible;
    textIn.clear();
    input->clear(inputNS::KEYS_PRESSED | inputNS::TEXT_IN);
}

//=============================================================================
// Return visible.
//=============================================================================
bool Console::getVisible()
{
    return visible;
}

//=============================================================================
// Set visible = true;
//=============================================================================
void Console::show()
{
    visible = true;
}

//=============================================================================
// Set visible = false;
//=============================================================================
void Console::hide()
{
    visible = false;
}

//=============================================================================
// print variable format string
//=============================================================================
void Console::print(const char* fmt, ...)
{
    if (!initialized)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    char str[256];

    vsnprintf(str, 256, fmt, args);
    va_end(args);

    text.push_front(str);           // add str to deque of text

    if (text.size() > consoleNS::MAX_LINES)
    {
        text.pop_back();            // delete oldest line
    }
}

//=============================================================================
// Add text to console
// Only the first line of text in str will be displayed.
//=============================================================================
void Console::print(const std::string& str)
{
    if (!initialized)
    {
        return;
    }

    text.push_front(str);           // add str to deque of text

    if (text.size() > consoleNS::MAX_LINES)
    {
        text.pop_back();            // delete oldest line
    }
}

//=============================================================================
// Save the char just entered in textIn string
//=============================================================================
void Console::keyIn(unsigned int key)
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
// Return console command
// Handles console single key commands.
// Returns all other commands to game.
//=============================================================================
std::string Console::getCommand()
{
    // if console not initialized or not visible
    if (!initialized || !visible)
    {
        return "";
    }

    // check for console key
    if (input->wasKeyPressed(CONSOLE_KEY))
    {
        hide();         // turn off console
    }

    // check for Esc key
    if (input->wasKeyPressed(ESC_KEY))
    {
        return "";
    }

    // check for scroll
    if (input->wasKeyPressed(UPARROW_KEY))
    {
        scrollAmount++;
    }
    else if (input->wasKeyPressed(DNARROW_KEY))
    {
        scrollAmount--;
    }
    else if (input->wasKeyPressed(PGUP_KEY))
    {
        scrollAmount += rows;
    }
    else if (input->wasKeyPressed(PGDN_KEY))
    {
        scrollAmount -= rows;
    }

    if (scrollAmount < 0)
    {
        scrollAmount = 0;
    }

    if (scrollAmount > consoleNS::MAX_LINES - 1)
    {
        scrollAmount = consoleNS::MAX_LINES - 1;
    }

    if (scrollAmount > (int)(text.size()) - 1)
    {
        scrollAmount = (int)(text.size()) - 1;
    }

    if (textIn.length() == 0)
    {
        return "";
    }

    if (input->wasKeyPressed(BACKSPACE_KEY) == true)            // backspace
    {
        textIn.erase(textIn.size() - 1);
    }

    if (input->wasKeyPressed(ENTER_KEY) == false)           // if 'Enter' key not pressed
    {
        return "";
    }

    // do not pass keys through to game
    input->clear(inputNS::KEYS_DOWN | inputNS::KEYS_PRESSED
        | inputNS::MOUSE);

    inputStr = textIn;
    textIn.clear();

    return inputStr;
}

//=============================================================================
// Return Console Input text
//=============================================================================
std::string Console::getInput()
{
    return textIn;
}

//=============================================================================
// Clear Input text
//=============================================================================
void Console::clearInput()
{
    textIn = "";
}
